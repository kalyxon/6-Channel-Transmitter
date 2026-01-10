/* Joystick Test Code for B5K Potentiometers */
/* Paste this in src/main.cpp of a NEW PlatformIO project */

#include <Arduino.h>

void setup() {
  Serial.begin(9600);
  Serial.println("=== Joystick Module Test ===");
  Serial.println("Move each joystick through full range");
  Serial.println("Record MIN, CENTER, MAX values");
  Serial.println("=============================");
  delay(2000);
}

void loop() {
  // Read all joystick values
  int throttle = analogRead(A3);
  int yaw = analogRead(A1);
  int pitch = analogRead(A2);
  int roll = analogRead(A6);
  
  // Read switches
  int aux1 = digitalRead(5);
  int aux2 = digitalRead(3);
  
  // Display with labels
  Serial.print("THROTTLE(A3):");
  Serial.print(throttle);
  
  Serial.print(" | YAW(A1):");
  Serial.print(yaw);
  
  Serial.print(" | PITCH(A2):");
  Serial.print(pitch);
  
  Serial.print(" | ROLL(A6):");
  Serial.print(roll);
  
  Serial.print(" | AUX1:");
  Serial.print(aux1);
  
  Serial.print(" | AUX2:");
  Serial.println(aux2);
  
  delay(200); // Delay between readings
}