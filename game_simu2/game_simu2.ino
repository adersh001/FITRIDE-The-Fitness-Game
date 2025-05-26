#include <BleGamepad.h>

BleGamepad bleGamepad("Test Gamepad", "Test Manufacturer");

void setup() {
  Serial.begin(9600);
  Serial.println("Starting BLE Gamepad...");
  
  bleGamepad.begin();
  Serial.println("BLE Gamepad started, advertising...");
}

void loop() {
  if (bleGamepad.isConnected()) {
    Serial.println("Device connected!");
    delay(1000);
  }
}
