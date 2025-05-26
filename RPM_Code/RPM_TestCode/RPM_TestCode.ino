#include <TimerOne.h>

const int IRSensorPin = 2; 
const int ledPin = 13;

int inputState;
int lastInputState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 2;

long time;
long endTime;
long startTime;
int RPM = 0;

void setup(void) {
  pinMode(IRSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("RPM Counter Starting...");
  delay(2000);

  endTime = 0;
  Timer1.initialize(1000000);  // Set the timer to 1 second
  Timer1.attachInterrupt(timerIsr);  // Attach the timer interrupt
}

// ---------------------------------------------------------------
void loop(void) {
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
}

// ---------------------------------------------------------------
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
