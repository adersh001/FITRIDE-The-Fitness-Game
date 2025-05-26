#include <TimerOne.h>
#include "UnoJoy.h"

const int IRSensorPin = 22; 
const int ledPin = 13;


// Define our pins
int TrianglePin = 4;
int SquarePin = 5;
int CrossPin = 3;
int CirclePin = 2;

int LeftPin = 8;
int UpPin = 9;
int RightPin = 10;
int DownPin = 11;

int StartPin =  12;

// Variables for RPM Counter
int inputState;
int lastInputState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 2;  // Increased debounce delay to filter out noise

long endTime = 0;
long startTime = 0;
int RPM = 0;
int speedValue = 0;  // Initialize speedValue




void setup(){
  setupPins();
  setupUnoJoy();

  pinMode(IRSensorPin,INPUT); // Use internal pull-up resistor for more stable readings
  pinMode(ledPin, OUTPUT);

  //Serial.begin(9600);
  //Serial.println("RPM and Game Controller Starting...");
  delay(2000);

  Timer1.initialize(1000000);  // Set the timer to 1 second
  Timer1.attachInterrupt(timerIsr);  // Attach the timer interrupt

}

void loop(){
  // Always be getting fresh data
    // RPM Counter Logic
  int currentSwitchState = digitalRead(IRSensorPin);
  
  // Debugging sensor state: print when the sensor changes state
  if (currentSwitchState != lastInputState) {
    //Serial.print("Sensor state changed: ");
   // Serial.println(currentSwitchState);
    lastDebounceTime = millis();
  }

  // Only consider the state change if it's stable for the debounce delay time
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (currentSwitchState != inputState) {
      inputState = currentSwitchState;
      
      // Only consider LOW (sensor detection) if the signal is stable for a threshold time
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
  speedValue = map(RPM, 0, 200, 128, 255); // Map RPM to speed value

  // If RPM is zero or too low, set speedValue to 0 to prevent the joystick from moving
 // if (RPM <= 0) {
  //  speedValue = 0;
 // }

  speedValue = constrain(speedValue, 0, 255); // Ensure within joystick range


  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
}

void setupPins(void){
  // Set all the digital pins as inputs
  // with the pull-up enabled, except for the 
  // two serial line pins
  for (int i = 2; i <= 12; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
  // pinMode(A1, INPUT);

}

dataForController_t getControllerData(void){
  
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins


  controllerData.leftStickY = speedValue;
 // Serial.print("leftStickY: ");
  //Serial.println(speedValue);
  
  controllerData.triangleOn = !digitalRead(TrianglePin);
  controllerData.circleOn = !digitalRead(CirclePin);
  controllerData.squareOn = !digitalRead(SquarePin);
  controllerData.crossOn = !digitalRead(CrossPin);
  controllerData.dpadUpOn = !digitalRead(UpPin);
  controllerData.dpadDownOn = !digitalRead(DownPin);
  controllerData.dpadLeftOn = !digitalRead(LeftPin);
  controllerData.dpadRightOn = !digitalRead(RightPin);
  
  controllerData.startOn = !digitalRead(StartPin);
  //controllerData.leftStickX = analogRead(A1) >> 2;
  
  // And return the data!
  return controllerData;
}
void calculateRPM() {
  long currentTime = millis();
  
  // Only calculate RPM if enough time has passed to detect a change in the sensor
  if (currentTime - startTime > 1000) {  // Allow 1-second interval for RPM update
    if (endTime > startTime) {  // Prevent division by zero or negative RPM
      RPM = 60000 / (currentTime - endTime); // Calculate RPM
    } else {
      RPM = 0;  // If no movement detected, set RPM to 0
    }

    // Check if RPM is zero and reset the RPM-related controller speed
    if (RPM == 0) {
      speedValue = 128;
    }
     endTime = currentTime;
   /*
    // Debug information
    Serial.print("startTime: ");
    Serial.print(startTime);
    Serial.print(" endTime: ");
    Serial.print(endTime);

    */
   // Serial.print(" RPM: ");
   // Serial.println(RPM);
    
  }
}

void timerIsr() {
  // Print RPM every second
 // Serial.println("---------------");
 // Serial.print("RPM: ");
 // Serial.println(RPM);
  RPM = 0; // Reset RPM after printing
}
