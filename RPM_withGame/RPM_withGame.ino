#include "UnoJoy.h"
#include <TimerOne.h>

// Pins for RPM Counter
const int IRSensorPin = 22; 
const int ledPin = 13;

// Variables for RPM Counter
int inputState;
int lastInputState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 2;

long endTime = 0;
long startTime = 0;
int RPM = 0;
int TrianglePin = 4;
int SquarePin = 5;
int CrossPin = 3;
int CirclePin = 2;

int LeftPin = 8;
int UpPin = 9;
int RightPin = 10;
int DownPin = 11;

int StartPin =  12;
// UnoJoy setup and pin configuration
void setup() {
  setupPins();
  setupUnoJoy();

  // RPM Counter setup
  pinMode(IRSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("RPM and Game Controller Starting...");
  delay(2000);

  Timer1.initialize(1000000);  // Set the timer to 1 second
  Timer1.attachInterrupt(timerIsr);  // Attach the timer interrupt
}

void loop() {
  // RPM Counter Logic
  int currentSwitchState = digitalRead(IRSensorPin);
  if (currentSwitchState != lastInputState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentSwitchState != inputState) {
      inputState = currentSwitchState;
      if (inputState == LOW) {
        digitalWrite(ledPin, LOW);
        calculateRPM(); // Calculate RPM from sensor
      } else {
        digitalWrite(ledPin, HIGH);
      }
    }
  }
  lastInputState = currentSwitchState;

  // Update UnoJoy controller data
  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
}

void setupPins(void) {
  // Configure input pins for buttons and joystick
  for (int i = 3; i <= 12; i++) { // Avoid pin 2 for RPM sensor
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
}

// RPM Calculation
void calculateRPM() {
  startTime = lastDebounceTime;
  RPM = 60000 / (startTime - endTime); // Calculate RPM
  endTime = startTime;
}

void timerIsr() {
  // Print RPM every second
  Serial.println("---------------");
  Serial.print("RPM: ");
  Serial.println(RPM);
   RPM = 0; // Reset RPM after printing
}

// UnoJoy Controller Data
dataForController_t getControllerData(void) {
  // Get a blank controller structure
  dataForController_t controllerData = getBlankDataForController();

  // Map RPM to game car speed
  // Map RPM (0 to 3000, for example) to joystick range (0 to 255)
  int speedValue = map(RPM, 0, 320, 0, 255);
  speedValue = constrain(speedValue, 0, 255); // Ensure within joystick range

  // Use mapped speed value to control leftStickY for throttle
  controllerData.leftStickY = speedValue;

  Serial.print("speedValue");
  Serial.println(speedValue);
  // Set up buttons
  controllerData.triangleOn = !digitalRead(3);
  controllerData.circleOn = !digitalRead(4);
  controllerData.squareOn = !digitalRead(5);
  controllerData.crossOn = !digitalRead(6);

  controllerData.dpadUpOn = !digitalRead(7);
  controllerData.dpadDownOn = !digitalRead(8);
  controllerData.dpadLeftOn = !digitalRead(9);
  controllerData.dpadRightOn = !digitalRead(10);

  controllerData.l1On = !digitalRead(11);
  controllerData.r1On = !digitalRead(12);

  // Return controller data
  return controllerData;
}
