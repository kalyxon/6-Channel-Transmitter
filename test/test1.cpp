/*
  ===========================================
  6-CHANNEL DRONE TRANSMITTER WITH LCD
  Hardware: Arduino Nano + NRF24L01 + LCD
  ===========================================
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>

// ============================
// PIN DEFINITIONS - SAME AS BEFORE
// ============================
// Joystick Pins
#define THROTTLE_PIN A3  // Left stick - vertical (throttle)
#define ROLL_PIN     A6  // Left stick - horizontal (roll)
#define YAW_PIN      A1  // Right stick - horizontal (yaw)
#define PITCH_PIN    A2  // Right stick - vertical (pitch)

// Switch Pins - SAME WIRING
#define TOGGLE1_PIN1 4   // AUX1 toggle switch pin 1 (LEFT position)
#define TOGGLE1_PIN2 5   // AUX1 toggle switch pin 2 (RIGHT position)
#define TOGGLE2_PIN1 3   // AUX2 toggle switch pin 1 (LEFT position)
#define TOGGLE2_PIN2 2   // AUX2 toggle switch pin 2 (RIGHT position)

// Button Pins
#define BTN_LEFT    7    // Left joystick button
#define BTN_RIGHT   6    // Right joystick button

// Analog Pins
#define POT_PIN     A0   // Potentiometer (AUX3)
#define VOLTAGE_PIN A7   // Battery voltage divider

// ============================
// GLOBAL VARIABLES
// ============================
float batteryVoltage = 0.0;
int potRawValue = 0;
int potMappedValue = 127; // Center position
int potPercent = 50;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16x2 display

// NRF24L01
const uint64_t pipeOut = 0xE8E8F0F0E1LL; // Must match drone
RF24 radio(10, 9); // CE = D10, CSN = D9

// ============================
// DATA STRUCTURE - UPDATED
// ============================
struct MyData {
  byte throttle;  // 0-255
  byte yaw;       // 0-255
  byte pitch;     // 0-255
  byte roll;      // 0-255
  byte AUX1;      // Toggle switch 1 (0=LEFT/DOWN, 1=RIGHT/UP)
  byte AUX2;      // Toggle switch 2 (0=LEFT/DOWN, 1=RIGHT/UP)
  byte AUX3;      // Potentiometer (0-255)
  byte AUX4;      // Buttons (bit0=left, bit1=right)
};

MyData data;

// ============================
// TIMING VARIABLES
// ============================
unsigned long lcdUpdateTime = 0;
const unsigned long LCD_UPDATE_INTERVAL = 300; // Update LCD every 300ms

unsigned long lastDisplayChange = 0;
const unsigned long DISPLAY_CHANGE_INTERVAL = 2500; // Change display every 2.5s

int displayState = 0; // 0-3 for different display modes

// ============================
// FUNCTION DECLARATIONS
// ============================
void updateScrollingDisplay();
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse);
byte readToggleSwitch2Position(int pin1, int pin2); // UPDATED: For 2-position with 2 pins
void readAllSensors();
void resetData();
void printRadioInfo();
void printPadded(int value);

// ============================
// SETUP - SAME AS BEFORE
// ============================
void setup() {
  Serial.begin(9600);
  Serial.println("=== DRONE TRANSMITTER ===");
  Serial.println("Initializing...");
  
  // Initialize pins - SAME AS BEFORE
  pinMode(POT_PIN, INPUT);
  pinMode(VOLTAGE_PIN, INPUT);
  
  pinMode(TOGGLE1_PIN1, INPUT_PULLUP);
  pinMode(TOGGLE1_PIN2, INPUT_PULLUP);
  pinMode(TOGGLE2_PIN1, INPUT_PULLUP);
  pinMode(TOGGLE2_PIN2, INPUT_PULLUP);
  
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("DRONE");
  lcd.setCursor(3, 1);
  lcd.print("TRANSMITTER");
  delay(2000);
  
  // Initialize NRF24L01
  Serial.print("NRF24L01... ");
  if (!radio.begin()) {
    Serial.println("FAILED!");
    lcd.clear();
    lcd.print("RADIO ERROR");
    while(1); // Halt on radio failure
  }
  
  // Configure radio
  radio.setAutoAck(false);           // No auto acknowledgment
  radio.setDataRate(RF24_250KBPS);   // 250kbps data rate
  radio.openWritingPipe(pipeOut);    // Set writing pipe
  radio.stopListening();             // Transmitter mode
  
  Serial.println("OK");
  
  // Print radio info
  printRadioInfo();
  
  // Reset data to safe values
  resetData();
  
  // Display ready message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("READY");
  lcd.setCursor(0, 1);
  lcd.print("Pipe: E8E8F0F0E1");
  delay(2000);
  
  Serial.println("\n=== TRANSMITTER READY ===");
  Serial.println("AUX1: LEFT=Normal, RIGHT=Emergency Stop");
  Serial.println("AUX2: LEFT=Disarmed, RIGHT=Armed (with throttle=0)");
  Serial.println("Sending data...\n");
}

// ============================
// MAIN LOOP - SAME
// ============================
void loop() {
  unsigned long currentMillis = millis();
  
  // 1. Read all sensors and controls
  readAllSensors();
  
  // 2. Update LCD display
  if (currentMillis - lcdUpdateTime >= LCD_UPDATE_INTERVAL) {
    updateScrollingDisplay();
    lcdUpdateTime = currentMillis;
  }
  
  // 3. Send data to drone
  bool sent = radio.write(&data, sizeof(MyData));
  
  // 4. Debug output to Serial
  static unsigned long lastSerialPrint = 0;
  if (currentMillis - lastSerialPrint >= 1000) {
    Serial.print("Sent: ");
    Serial.print(sent ? "OK" : "FAIL");
    Serial.print(" | T:");
    Serial.print(data.throttle);
    Serial.print(" Y:");
    Serial.print(data.yaw);
    Serial.print(" P:");
    Serial.print(data.pitch);
    Serial.print(" R:");
    Serial.print(data.roll);
    Serial.print(" | A1:");
    Serial.print(data.AUX1 ? "RIGHT/UP" : "LEFT/DOWN");
    Serial.print(" A2:");
    Serial.print(data.AUX2 ? "RIGHT/UP" : "LEFT/DOWN");
    Serial.print(" A3:");
    Serial.print(data.AUX3);
    Serial.print(" A4:0x");
    Serial.print(data.AUX4, HEX);
    Serial.print(" | Bat:");
    Serial.print(batteryVoltage, 1);
    Serial.println("V");
    
    lastSerialPrint = currentMillis;
  }
  
  // 5. Small delay for timing
  delay(4); // Match drone's 250Hz reading rate
}

// ============================
// READ ALL SENSORS - UPDATED
// ============================
void readAllSensors() {
  // Read battery voltage
  int voltageRead = analogRead(VOLTAGE_PIN);
  batteryVoltage = (voltageRead * 10.0) / 1024.0; // For 10k+10k divider
  
  // Read potentiometer
  potRawValue = analogRead(POT_PIN);
  potMappedValue = map(potRawValue, 0, 1023, 0, 255);
  potPercent = map(potRawValue, 0, 1023, 0, 100);
  data.AUX3 = potMappedValue;
  
  // Read joysticks with deadzone compensation
  data.throttle = mapJoystickValues(analogRead(THROTTLE_PIN), 13, 524, 1015, true);
  data.roll     = mapJoystickValues(analogRead(ROLL_PIN), 34, 522, 1020, true);
  data.yaw      = mapJoystickValues(analogRead(YAW_PIN), 50, 505, 1020, true);
  data.pitch    = mapJoystickValues(analogRead(PITCH_PIN), 12, 544, 1021, true);
  
  // Read toggle switches - UPDATED FOR 2-POSITION
  data.AUX1 = readToggleSwitch2Position(TOGGLE1_PIN1, TOGGLE1_PIN2);
  data.AUX2 = readToggleSwitch2Position(TOGGLE2_PIN1, TOGGLE2_PIN2);
  
  // Read joystick buttons
  byte buttons = 0;
  if (digitalRead(BTN_LEFT) == LOW) buttons |= 0x01;  // Left button
  if (digitalRead(BTN_RIGHT) == LOW) buttons |= 0x02; // Right button
  data.AUX4 = buttons;
}

// ============================
// MAP JOYSTICK VALUES - SAME
// ============================
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse) {
  // Constrain to valid range
  val = constrain(val, lower, upper);
  
  // Split mapping for precision around center
  if (val < middle) {
    val = map(val, lower, middle, 0, 128);
  } else {
    val = map(val, middle, upper, 128, 255);
  }
  
  // Reverse if needed (some joysticks are inverted)
  return (reverse ? 255 - val : val);
}

// ============================
// READ 2-POSITION TOGGLE SWITCH (WITH 2 PINS)
// ============================
byte readToggleSwitch2Position(int pin1, int pin2) {
  // Your toggle switch wiring:
  // Middle pin = GND
  // Pin1 = Arduino pin (LEFT position when switch is LEFT)
  // Pin2 = Arduino pin (RIGHT position when switch is RIGHT)
  
  // With INPUT_PULLUP:
  // - When switch is LEFT: pin1=LOW, pin2=HIGH
  // - When switch is RIGHT: pin1=HIGH, pin2=LOW
  // - Never both LOW or both HIGH (no middle position)
  
  bool state1 = digitalRead(pin1);
  bool state2 = digitalRead(pin2);
  
  // Since middle pin is GND and we use INPUT_PULLUP:
  // Switch LEFT: pin1=LOW (0), pin2=HIGH (1)  -> Return 0
  // Switch RIGHT: pin1=HIGH (1), pin2=LOW (0) -> Return 1
  
  if (state1 == LOW && state2 == HIGH) {
    return 0;  // LEFT position (DOWN/OFF/DISARMED)
  } else if (state1 == HIGH && state2 == LOW) {
    return 1;  // RIGHT position (UP/ON/ARMED)
  } else {
    // This shouldn't happen with 2-position switch
    // But if it does, return 0 (safe position)
    return 0;
  }
}

// ============================
// UPDATE SCROLLING DISPLAY - UPDATED
// ============================
void updateScrollingDisplay() {
  unsigned long currentMillis = millis();
  
  // Change display mode every 2.5 seconds
  if (currentMillis - lastDisplayChange >= DISPLAY_CHANGE_INTERVAL) {
    displayState = (displayState + 1) % 4;
    lastDisplayChange = currentMillis;
    lcd.clear();
  }
  
  switch (displayState) {
    case 0:
      // Display 1: Control Values
      lcd.setCursor(0, 0);
      lcd.print("T:");
      printPadded(data.throttle);
      lcd.print(" Y:");
      printPadded(data.yaw);
      
      lcd.setCursor(0, 1);
      lcd.print("P:");
      printPadded(data.pitch);
      lcd.print(" R:");
      printPadded(data.roll);
      break;
      
    case 1:
      // Display 2: Switches & Battery
      lcd.setCursor(0, 0);
      lcd.print("A1:");
      lcd.print(data.AUX1 ? "R/UP" : "L/DN");
      lcd.print(" A2:");
      lcd.print(data.AUX2 ? "R/UP" : "L/DN");
      
      lcd.setCursor(0, 1);
      lcd.print("Bat:");
      lcd.print(batteryVoltage, 1);
      lcd.print("V A4:0x");
      lcd.print(data.AUX4, HEX);
      break;
      
    case 2:
      // Display 3: Potentiometer
      lcd.setCursor(0, 0);
      lcd.print("POTENTIOMETER");
      
      lcd.setCursor(0, 1);
      lcd.print("VAL:");
      printPadded(potMappedValue);
      lcd.print("/255");
      break;
      
    case 3:
      // Display 4: Status Info
      lcd.setCursor(0, 0);
      lcd.print("DRONE LINK ACTIVE");
      
      lcd.setCursor(0, 1);
      lcd.print("Ch:76 Rate:250K");
      break;
  }
}

// ============================
// HELPER FUNCTIONS - SAME
// ============================
void printPadded(int value) {
  if (value < 100) lcd.print(" ");
  if (value < 10) lcd.print(" ");
  lcd.print(value);
}

void resetData() {
  data.throttle = 0;    // Throttle at minimum
  data.yaw = 127;       // Yaw centered
  data.pitch = 127;     // Pitch centered
  data.roll = 127;      // Roll centered
  data.AUX1 = 0;        // Toggle 1 in LEFT (normal/safe)
  data.AUX2 = 0;        // Toggle 2 in LEFT (disarmed/safe)
  data.AUX3 = 127;      // Potentiometer centered
  data.AUX4 = 0;        // Buttons not pressed
}

void printRadioInfo() {
  Serial.println("Radio Configuration:");
  Serial.print("  Pipe: 0x");
  Serial.println((unsigned long)pipeOut, HEX);
  Serial.print("  Data Rate: ");
  switch(radio.getDataRate()) {
    case RF24_250KBPS: Serial.println("250KBPS"); break;
    case RF24_1MBPS: Serial.println("1MBPS"); break;
    case RF24_2MBPS: Serial.println("2MBPS"); break;
  }
  Serial.print("  Power Level: ");
  switch(radio.getPALevel()) {
    case RF24_PA_MIN: Serial.println("MIN"); break;
    case RF24_PA_LOW: Serial.println("LOW"); break;
    case RF24_PA_HIGH: Serial.println("HIGH"); break;
    case RF24_PA_MAX: Serial.println("MAX"); break;
  }
  Serial.print("  Channel: ");
  Serial.println(radio.getChannel());
}
