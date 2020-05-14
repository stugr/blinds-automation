#include <Servo.h>

Servo servo;

const int buttonOpenPin = 2;
const int buttonClosePin = 3;
const int servoPin = 9;
const int lightPin = A0;
const int lightThreshold = 450;
const int darkThreshold = 500;

int buttonOpenState = 0;
int buttonCloseState = 0;
bool dark = false;

int position = 0;

void setup() {
  // attach servo and set its range
  servo.attach(servoPin, 1000, 2000); // TODO: adjust range depending on servo used, and see if default start position can be avoided

  // setup buttons as inputs
  pinMode(buttonOpenPin, INPUT);
  pinMode(buttonClosePin, INPUT);

  Serial.begin(9600);
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

  int lightLevel = analogRead(lightPin);
  if (lightLevel > darkThreshold && !dark) {
    dark = true;
    Serial.println("dark");
    Serial.println(lightLevel);
    rotateCounterClockwise();
  } else if (lightLevel < lightThreshold & dark ) {
    dark = false;
    Serial.println("light");
    Serial.println(lightLevel);
    rotateClockwise();
  }
}

void rotateClockwise() {
  rotateServo(180);
}

void rotateCounterClockwise() {
  rotateServo(0);
}

void rotateServo(int pos) {
  if (position != pos) {
    Serial.print("Moving from ");
    Serial.print(position);
    Serial.print(" to ");
    Serial.println(pos);
    position = pos;
    servo.write(position);
    delay(15);
  }  
}
