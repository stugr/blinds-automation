#include <Servo.h>

Servo servo;

const int buttonOpenPin = 2;
const int buttonClosePin = 3;
const int servoPin = 9;

int buttonOpenState = 0;
int buttonCloseState = 0;

int position = 0;

void setup() {
  // attach servo and set its range
  servo.attach(servoPin, 1000, 2000); // TODO: adjust range depending on servo used, and see if default start position can be avoided

  // setup buttons as inputs
  pinMode(buttonOpenPin, INPUT);
  pinMode(buttonClosePin, INPUT);
}

void loop() {
  buttonOpenState = digitalRead(buttonOpenPin);

  if (buttonOpenState == HIGH) {
    rotateClockwise();
  } else {
    // nothing
  }

  buttonCloseState = digitalRead(buttonClosePin);

  if (buttonCloseState == HIGH) {
    rotateCounterClockwise();
  } else {
    // nothing
  }
}

void rotateClockwise() {
  rotateServo(180);
}

void rotateCounterClockwise() {
  rotateServo(0);
}

void rotateServo(int pos) {
  position = pos;
  servo.write(position);
  delay(15);  
}
