#include <Stepper.h>
#include <Bounce2.h>
#include <EEPROM.h>

// manual mode is used to make position adjustments
// use auto mode to put the blinds in either the closed or open position then toggle this boolean to true and reupload
// hold down the buttons to adjust the blind position then change boolean back to false and reupload
bool manualMode = false;

const int buttonOpenPin = 2;
const int buttonClosePin = 3;
const int switchPin = 4;
const int ledPin = 13;

const int steps = 2048; // motor 28BYJ-48
const int openSteps = (steps * 4.75);
const int closedSteps = 0;
const int stepInterval = steps / 64;

int stepperOffAfter = 5000; // milliseconds

int requestedPosition = 0;
bool stepperOn = false;
bool currentlyMoving = false;

int position = 0;

int eepromValue = 0;

const int stepperPins[4] = {8, 10, 9, 11};

Stepper stepper(steps, stepperPins[0], stepperPins[1], stepperPins[2], stepperPins[3]);

const int buttonCount = 3;
const int debounceDelay = 50;
const int buttonPins[buttonCount] = {buttonOpenPin, buttonClosePin, switchPin};

unsigned long currentTimer = 0; 
unsigned long stepperLastRanTimer = 0;

Bounce buttons[buttonCount];

void setup() {
  // setup buttons as inputs
  for (int i = 0; i < buttonCount; i++) {
      buttons[i].attach(buttonPins[i], INPUT_PULLUP);
      buttons[i].interval(debounceDelay);
  }

  pinMode(ledPin, OUTPUT);

  stepper.setSpeed(15); // up to 20

  Serial.begin(9600);
  
  // read EEPROM
  readEEPROM();

  // get switch starting position
  buttons[2].update();
  switchToggled(buttons[2].read());
}

void loop() {
  currentTimer = millis();

  // turn off stepper if interval has passed and stepper is on
  if (stepperOn) {
    if (currentTimer - stepperLastRanTimer >= stepperOffAfter) {
      Serial.print("Turning off stepper after idle time of ");
      Serial.println(stepperOffAfter);
      turnOffStepper();
      stepperLastRanTimer += stepperOffAfter;
    }
  }
  
  // update debouncers
  for (int i = 0; i < buttonCount; i++) {
    buttons[i].update();
  }

  // check if switch has changed position
  if (buttons[2].fell()) {
    readEEPROM();
    switchToggled(false);
  } else if (buttons[2].rose()) {
    switchToggled(true);
  }

  // manual mode 
  if (manualMode) {
    if (buttons[0].read() == LOW || buttons[1].read() == LOW) {
      // yellow
      if (buttons[0].read() == LOW) {
        rotateStepper(stepInterval);
      }
      // red
      else if (buttons[1].read() == LOW) {
        rotateStepper(-stepInterval);
      }
    }
  }
  // automation mode
  else {
    if (buttons[0].fell() || buttons[1].fell()) {
      if (currentlyMoving) {
        buttonPressed("Cancel", position);
        currentlyMoving = false;
      } else {
        // yellow
        if (buttons[0].fell()) {
          buttonPressed("Close", closedSteps);
        }
        // red
        else if (buttons[1].fell()) {
          buttonPressed("Open", openSteps);
        }
      }
    }
    // next loop through after button press
    else {
      if (requestedPosition > position) {
        rotateStepper(stepInterval);
      } else if (requestedPosition < position) {
        rotateStepper(-stepInterval);
      } else {
        if (currentlyMoving) {
          Serial.println("Reached destination");
          currentlyMoving = false;
          int newEepromValue = (position/openSteps)+1;
  
          // if eeprom value is different then write current position to EEPROM where 1 = closed and 2 = open
          // will save some writes, particularly when testing
          if (eepromValue != newEepromValue) {
            eepromValue = newEepromValue;
            Serial.print("Writing ");
            Serial.print(eepromValue);
            Serial.println(" to EEPROM");
            EEPROM.write(0, eepromValue);
          }
        }
      }
    }
  }
}

void buttonPressed(String whichButton, int reqPos) {
  Serial.print(whichButton);
  Serial.println(" button pressed");
  requestedPosition = reqPos;
}

void switchToggled(bool mode) {
  if (mode) {
    Serial.println("Manual mode");
    manualMode = true;
  } else {
    Serial.println("Auto mode");
    manualMode = false;
  }
}

void readEEPROM() {
  eepromValue = EEPROM.read(0);
  Serial.print("EEPROM(0) value is: ");
  Serial.println(eepromValue);

  if (eepromValue != 1 && eepromValue != 2) {
    // ignore EEPROM for reading current position
    // human should make sure blinds are in the close position
    Serial.println("EEPROM not set so starting at closed position of 0");
    EEPROM.write(0, 1);
  } else {
    position = (eepromValue - 1) * openSteps;
    requestedPosition = position;
    Serial.print("EEPROM was set so setting starting position to: ");
    Serial.println(position);
  }
}

void rotateStepper(int pos) {
  stepperOn = true;
  if (!manualMode) { 
    currentlyMoving = true;
  }
  position += pos;
  Serial.print("Moving ");
  Serial.print(pos);
  Serial.print(" to ");
  Serial.print(position);
  if (!manualMode) {
    Serial.print(" (requested position is ");
    Serial.print(requestedPosition);
    Serial.println(")");
  } else {
    Serial.println("");
  }
  stepper.step(pos);
  stepperLastRanTimer = millis();
  //delay(15);
}

void turnOffStepper() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(stepperPins[i],LOW);
  }
  stepperOn = false;
}
