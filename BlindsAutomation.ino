#include <Stepper.h>
#include <Bounce2.h>
#include <EEPROM.h>

const int buttonOpenPin = 2;
const int buttonClosePin = 3;
const int ledPin = 13;

const int steps = 2048; // motor 28BYJ-48
const int openSteps = -(steps * 5);
const int closedSteps = 0;

int requestedPosition = 0;
bool currentlyMoving = false;

int position = 0;

Stepper stepper(steps, 8, 10, 9, 11);

const int buttonCount = 2;
const int debounceDelay = 50;
const int buttonPins[buttonCount] = {buttonOpenPin, buttonClosePin};

Bounce buttons[buttonCount];

void setup() {
  // setup buttons as inputs
  //pinMode(buttonOpenPin, INPUT);
  //pinMode(buttonClosePin, INPUT);

  for (int i = 0; i < buttonCount; i++) {
      buttons[i].attach(buttonPins[i], INPUT);
      buttons[i].interval(debounceDelay);
  }

  pinMode(ledPin, OUTPUT);

  stepper.setSpeed(10);

  Serial.begin(9600);
  
  // read EEPROM
  int eepromValue = EEPROM.read(0);
  Serial.print("EEPROM(0) value is: ");
  Serial.println(eepromValue);

  if (eepromValue != 1 && eepromValue != 2) {
    // ignore EEPROM for reading current position
    // make sure blinds are in the close position
    Serial.println("EEPROM not set so starting at closed position of 0");
  } else {
    position = (eepromValue - 1) * openSteps;
    requestedPosition = position;
    Serial.print("EEPROM was set so setting starting position to: ");
    Serial.println(position);
  }
}

void loop() {
  // update debouncers
  for (int i = 0; i < buttonCount; i++) {
    buttons[i].update();
  }
  
  if (buttons[0].rose() || buttons[1].rose()) {
    if (currentlyMoving) {
      Serial.println("CANCEL button pressed");
      requestedPosition = position;
      currentlyMoving = false;
    } else {
      // yellow
      if (buttons[0].rose()) {
        Serial.println("Close button pressed");
        //rotateClockwise();
        requestedPosition = closedSteps;
      }
      // red
      else if (buttons[1].rose()) {
        Serial.println("Open button pressed");
        //rotateCounterClockwise();
        requestedPosition = openSteps;
      }
    }
  }
  // next loop through after button press
  else {
    if (requestedPosition > position) {
      rotateStepper(2048/16);
    } else if (requestedPosition < position) {
      rotateStepper(-2048/16);
    } else {
      if (currentlyMoving) {
        Serial.println("Reached destination, writing to EEPROM");
        currentlyMoving = false;
        // write current position to EEPROM where 1 = closed and 2 = open
        EEPROM.write(0, (position/openSteps)+1);
      }
    }
  }

}


void rotateClockwise() {
  rotateStepper(2048/16);
}

void rotateCounterClockwise() {
  rotateStepper(-2048/16);
}

void rotateStepper(int pos) {
  //if (position != pos) {
    currentlyMoving = true;
    position += pos;
    Serial.print("Moving ");
    Serial.print(pos);
    Serial.print(" to ");
    Serial.print(position);
    Serial.print(" (requested position is ");
    Serial.print(requestedPosition);
    Serial.println(")");
    stepper.step(pos);
    //delay(15);
  //}  
}
