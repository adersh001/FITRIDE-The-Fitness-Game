#include "UnoJoy.h"
#include <TimerOne.h>

// Pins for RPM Counter
const int IRSensorPin = 22; 
const int ledPin = 13;

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



// UnoJoy setup and pin configuration
void setup() {
  setupPins();
  setupUnoJoy();

  // RPM Counter setup
   //pinMode(IRSensorPin, INPUT_PULLUP); // Use internal pull-up resistor for more stable readings

  pinMode(IRSensorPin,INPUT); // Use internal pull-up resistor for more stable readings
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
  //Serial.println("RPM and Game Controller Starting...");
  delay(2000);

  Timer1.initialize(1000000);  // Set the timer to 1 second
  Timer1.attachInterrupt(timerIsr);  // Attach the timer interrupt
}

void loop() {
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
  speedValue = map(RPM, 0, 300, 0, 255); // Map RPM to speed value

  // If RPM is zero or too low, set speedValue to 0 to prevent the joystick from moving
  if (RPM <= 0) {
    speedValue = 0;
  }

  speedValue = constrain(speedValue, 0, 255); // Ensure within joystick range
  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
}

void setupPins(void) {
  // Configure input pins for buttons and joystick
  for (int i = 2; i <= 12; i++) { // Avoid pin 2 for RPM sensor
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
}

// RPM Calculation
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
      speedValue = 0;
    }
   /*
    // Debug information
    Serial.print("startTime: ");
    Serial.print(startTime);
    Serial.print(" endTime: ");
    Serial.print(endTime);

    */
   // Serial.print(" RPM: ");
   // Serial.println(RPM);
    
    
    endTime = currentTime;
   
  }
}

// Timer Interrupt Service Routine
void timerIsr() {
  // Print RPM every second
 // Serial.println("---------------");
 // Serial.print("RPM: ");
//  Serial.println(RPM);
  RPM = 0; // Reset RPM after printing
}

// UnoJoy Controller Data
dataForController_t getControllerData(void) {
  // Get a blank controller structure
  dataForController_t controllerData = getBlankDataForController();

  // Set the left stick Y value to speedValue
  controllerData.leftStickY = speedValue;


 // Serial.print("leftStickY: ");
 // Serial.println(speedValue);

  // Print pressed buttons
  /*
  if (!digitalRead(3)) Serial.println("Triangle button pressed");
  if (!digitalRead(4)) Serial.println("Circle button pressed");
  if (!digitalRead(5)) Serial.println("Square button pressed");
  if (!digitalRead(6)) Serial.println("Cross button pressed");
  if (!digitalRead(7)) Serial.println("DPad Up pressed");
  if (!digitalRead(8)) Serial.println("DPad Down pressed");
  if (!digitalRead(9)) Serial.println("DPad Left pressed");
  if (!digitalRead(10)) Serial.println("DPad Right pressed");
  if (!digitalRead(11)) Serial.println("L1 button pressed");
  if (!digitalRead(12)) Serial.println("R1 button pressed");
*/
  // Update controller data with button states
  controllerData.triangleOn = digitalRead(3);
  controllerData.circleOn = digitalRead(4);
  controllerData.squareOn = digitalRead(5);
  controllerData.crossOn = digitalRead(6);

  controllerData.dpadUpOn = digitalRead(7);
  controllerData.dpadDownOn =digitalRead(8);
  controllerData.dpadLeftOn = digitalRead(9);
  controllerData.dpadRightOn = digitalRead(10);

  controllerData.l1On = digitalRead(11);
  controllerData.r1On = digitalRead(12);

  return controllerData;
}


void setupUnoJoy(void){
    // First, let's zero out our controller data buffer (center the sticks)
    controllerDataBuffer = getBlankDataForController();
  
    // Start the serial port at the specific, low-error rate UnoJoy uses.
    //  If you want to change the rate, you'll have to change it in the
    //  firmware for the ATmega8u2 as well.  250,000 is actually the best rate,
    //  but it's not supported on Macs, breaking the processing debugger.
    //Serial.begin(38400);
    
    // Now set up the Timer 0 compare register A
    //  so that Timer0 (used for millis() and such)
    //  also fires an interrupt when it's equal to
    //  128, not just on overflow.
    // This will fire our timer interrupt almost
    //  every 1 ms (1024 us to be exact).
    OCR0A = 128;
    TIMSK0 |= (1 << OCIE0A);
  }