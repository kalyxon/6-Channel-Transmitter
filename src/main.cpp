/*A basic 6 channel transmitter using the nRF24L01 module.*/


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>

// Global variables
float vol = 0;
int input = 0;
int potRawValue = 0;      // Raw reading from A0 (0-1023)
int potMappedValue = 0;   // Mapped value (0-255) for AUX3
int potPercent = 0;       // Percentage (0-100%)

// Pin definitions
int vdividerPin = A7;
int potentiometerPin = A0;

// Switch pins
int toggleSwitch1_pin1 = 4;
int toggleSwitch1_pin2 = 5;
int toggleSwitch2_pin1 = 3;
int toggleSwitch2_pin2 = 2;

// Joystick button pins
int leftJoystickSW = 7;
int rightJoystickSW = 6;

LiquidCrystal_I2C lcd(0x27, 16, 2);
const uint64_t pipeOut = 0xE8E8F0F0E1LL;
RF24 radio(10, 9); // CE = D10, CSN = D9

struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
  byte AUX3;  // Potentiometer value (0-255)
  byte AUX4;  // Joystick buttons
};

MyData data;

unsigned long lcdUpdateTime = 0;
const unsigned long lcdUpdateInterval = 300; // Update LCD every 300ms

// Variables for scrolling display
int displayState = 0;
unsigned long lastDisplayChange = 0;
const unsigned long displayChangeInterval = 2500; // Change display every 2.5 seconds

// Function prototypes
void updateScrollingDisplay();
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse);
byte readToggleSwitch(int pin1, int pin2);
void readAllSensors();

void resetData()
{
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 1;
  data.AUX2 = 1;
  data.AUX3 = 127;
  data.AUX4 = 0;
}

void setup()
{
  Serial.begin(9600);
  
  // Initialize nRF24L01
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  resetData();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("POTENTIOMETER");
  lcd.setCursor(1, 1);
  lcd.print("READING ACTIVE");
  delay(2000);
  lcd.clear();
  
  // Setup pin modes
  pinMode(vdividerPin, INPUT);
  pinMode(potentiometerPin, INPUT);
  
  pinMode(toggleSwitch1_pin1, INPUT_PULLUP);
  pinMode(toggleSwitch1_pin2, INPUT_PULLUP);
  pinMode(toggleSwitch2_pin1, INPUT_PULLUP);
  pinMode(toggleSwitch2_pin2, INPUT_PULLUP);
  
  pinMode(leftJoystickSW, INPUT_PULLUP);
  pinMode(rightJoystickSW, INPUT_PULLUP);
}

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if (val < middle)
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return (reverse ? 255 - val : val);
}

byte readToggleSwitch(int pin1, int pin2) {
  bool state1 = digitalRead(pin1);
  bool state2 = digitalRead(pin2);
  
  if (state1 == LOW && state2 == HIGH) {
    return 0;  // Position 1
  } else if (state1 == HIGH && state2 == LOW) {
    return 2;  // Position 2
  } else {
    return 1;  // Middle position
  }
}

void readAllSensors() {
  // Read voltage divider for battery voltage
  input = analogRead(vdividerPin);
  vol = (input * 10.0) / 1024.0;  // For 10k+10k divider
  
  // Read potentiometer
  potRawValue = analogRead(potentiometerPin);
  potMappedValue = map(potRawValue, 0, 1023, 0, 255);
  potPercent = map(potRawValue, 0, 1023, 0, 100);
  data.AUX3 = potMappedValue;  // Send to receiver
  
  // Read joysticks
  data.throttle = mapJoystickValues(analogRead(A3), 13, 524, 1015, true);  // Left VRX
  data.roll     = mapJoystickValues(analogRead(A6), 34, 522, 1020, true);  // Left VRY
  data.yaw      = mapJoystickValues(analogRead(A1), 50, 505, 1020, true);  // Right VRX
  data.pitch    = mapJoystickValues(analogRead(A2), 12, 544, 1021, true);  // Right VRY
  
  // Read toggle switches
  data.AUX1 = readToggleSwitch(toggleSwitch1_pin1, toggleSwitch1_pin2);
  data.AUX2 = readToggleSwitch(toggleSwitch2_pin1, toggleSwitch2_pin2);
  
  // Read joystick buttons
  byte buttons = 0;
  if (digitalRead(leftJoystickSW) == LOW) buttons |= 0x01;
  if (digitalRead(rightJoystickSW) == LOW) buttons |= 0x02;
  data.AUX4 = buttons;
}

void updateScrollingDisplay() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDisplayChange >= displayChangeInterval) {
    displayState = (displayState + 1) % 4;  // Cycle through 4 display states
    lastDisplayChange = currentMillis;
    lcd.clear();
  }
  
  switch (displayState) {
    case 0:
      // DISPLAY 1: BIG POTENTIOMETER READOUT
      lcd.setCursor(0, 0);
      lcd.print("POTENTIOMETER:");
      lcd.setCursor(0, 1);
      lcd.print("VAL:");
      if (potMappedValue < 100) lcd.print(" ");
      if (potMappedValue < 10) lcd.print(" ");
      lcd.print(potMappedValue);
      lcd.print("/255");
      break;
      
    case 1:
      // DISPLAY 2: RAW VALUES DETAIL
      lcd.setCursor(0, 0);
      lcd.print("RAW:");
      lcd.print(potRawValue);
      lcd.print("/1023");
      lcd.setCursor(0, 1);
      lcd.print("PERCENT:");
      if (potPercent < 100) lcd.print(" ");
      if (potPercent < 10) lcd.print(" ");
      lcd.print(potPercent);
      lcd.print("%   ");
      break;
      
    case 2:
      // DISPLAY 3: BATTERY + POT VALUE
      lcd.setCursor(0, 0);
      lcd.print("BATTERY:");
      lcd.print(vol, 1);
      lcd.print("V");
      lcd.setCursor(0, 1);
      lcd.print("POT:");
      if (potMappedValue < 100) lcd.print(" ");
      if (potMappedValue < 10) lcd.print(" ");
      lcd.print(potMappedValue);
      lcd.print(" SW1:");
      lcd.print(data.AUX1);
      break;
      
    case 3:
      // DISPLAY 4: JOYSTICK VALUES
      lcd.setCursor(0, 0);
      lcd.print("T:");
      if (data.throttle < 100) lcd.print(" ");
      if (data.throttle < 10) lcd.print(" ");
      lcd.print(data.throttle);
      lcd.print(" Y:");
      if (data.yaw < 100) lcd.print(" ");
      if (data.yaw < 10) lcd.print(" ");
      lcd.print(data.yaw);
      
      lcd.setCursor(0, 1);
      lcd.print("P:");
      if (data.pitch < 100) lcd.print(" ");
      if (data.pitch < 10) lcd.print(" ");
      lcd.print(data.pitch);
      lcd.print(" R:");
      if (data.roll < 100) lcd.print(" ");
      if (data.roll < 10) lcd.print(" ");
      lcd.print(data.roll);
      break;
  }
}

void loop()
{
  unsigned long currentMillis = millis();

  // Read all sensors first
  readAllSensors();
  
  // Update LCD at regular intervals
  if (currentMillis - lcdUpdateTime >= lcdUpdateInterval) {
    updateScrollingDisplay();
    lcdUpdateTime = currentMillis;
  }
  
  // Send data via nRF24L01
  radio.write(&data, sizeof(MyData));
  
  // Optional serial monitor output
  static unsigned long lastSerialPrint = 0;
  if (currentMillis - lastSerialPrint >= 1000) {
    Serial.print("Pot - Raw:");
    Serial.print(potRawValue);
    Serial.print(" Map:");
    Serial.print(potMappedValue);
    Serial.print(" %:");
    Serial.print(potPercent);
    Serial.print(" Bat:");
    Serial.print(vol, 1);
    Serial.print("V | T:");
    Serial.print(data.throttle);
    Serial.print(" Y:");
    Serial.print(data.yaw);
    Serial.print(" P:");
    Serial.print(data.pitch);
    Serial.print(" R:");
    Serial.println(data.roll);
    lastSerialPrint = currentMillis;
  }
}
