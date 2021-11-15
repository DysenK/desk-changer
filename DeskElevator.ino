#include <EEPROM.h>
#include <Servo.h>

// Define pins
const int homeSensor1 = 2;
const int homeSensor2 = 3;
const int returnEmpty = 8;
const int shelf1 = 9;
const int shelf2 = 10;
const int driverPUL = 7;
const int driver1DIR = 6;
const int driver2DIR = 4;
const int servoPin = 5;
const int dcMotorEnable = 11;
const int IN1 = 12;
const int IN2 = 13;

// Variables
int pulseDelay = 2500;            // Speed of stepper
int stepDir = LOW;                // Direction of stepper
const int shelf1Distance = 200;   // Number of steps from home to shelf 1
const int shelf2Distance = 400;   // Number of steps from home to shelf 2

// Create servo object for fetching arm
Servo armServo;

// Extends servo arm and pulls item onto carriage with dc motor
void retrieveItem() {
  armServo.write(90);
  delay(500);
  analogWrite(dcMotorEnable, 50);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(3000);
  analogWrite(dcMotorEnable, 0);
  armServo.write(0);
  delay(500);
}

// Extends servo arm, pushes item onto dc motor wheel, runs motor to push item onto shelf
void returnItem() {
  armServo.write(90);
  delay(500);
  analogWrite(dcMotorEnable, 50);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(3000);
  analogWrite(dcMotorEnable, 0);
  armServo.write(0);
  delay(500);
}

// Returns the distance from home to the last shelf retrieved from
int shelfDistance() {
    switch (EEPROM.read(0)) {
      case 0:
        return 0;
      case 1:
        return shelf1Distance;
      case 2:
        return shelf2Distance;
    }
}

// Performs one step. stepperDir: HIGH = carriage up, LOW = carriage down
void stepperStep(bool stepperDir) {
  digitalWrite(driver1DIR, stepperDir);
  digitalWrite(driver2DIR, !stepperDir);
  digitalWrite(driverPUL, LOW);
  delayMicroseconds(pulseDelay);
  digitalWrite(driverPUL, HIGH);
  delayMicroseconds(pulseDelay);
}

void returnAndRetrieve(int distance, int lastShelf) {
  // Move carriage down to the last shelf retrieved from and return the item
  for (int i = 0; i < shelfDistance(); i++) {
    stepperStep(LOW);
  }
  returnItem();

  // Set direction carriage must move based on previous shelf location
  if (shelfDistance() > distance) {
    stepDir = HIGH;
  }
  else {
    stepDir = LOW;
  }
    
    // Move carriage from last shelf to selected shelf, retrieve new item, and return home
  for (int i = 0; i < abs(shelfDistance() - distance); i++) {
    stepperStep(stepDir);
  }
    
  retrieveItem();

  while (digitalRead(homeSensor1) == true && digitalRead(homeSensor2) == true) {
    stepperStep(HIGH);
  }

  // Keep track of which shelf was just retrieved from
  EEPROM.update(0, lastShelf);
}

void setup() {
  // Set pinmodes
  pinMode(driverPUL, OUTPUT);
  pinMode(driver1DIR, OUTPUT);
  pinMode(driver2DIR, OUTPUT);
  pinMode(shelf1, INPUT_PULLUP);
  pinMode(shelf2, INPUT_PULLUP);
  pinMode(returnEmpty, INPUT_PULLUP);
  pinMode(homeSensor1, INPUT);
  pinMode(homeSensor2, INPUT);

  // Set servo pin and move it to the default position
  armServo.attach(servoPin);
  armServo.write(0);
  
  // Home carriage to top of assembly
  while (digitalRead(homeSensor1) == true && digitalRead(homeSensor2) == true) {
    stepperStep(HIGH);
  }
}


void loop() {
  // Set a process to run depending on which button is pressed (buttons will do nothing while current process is running)
  
  if (digitalRead(returnEmpty) == LOW && EEPROM.read(0) != 0) {
    // Move carriage down to last shelf, return the item, move up to home
    for (int i = 0; i < shelfDistance(); i++) {
      stepperStep(LOW);
    }

    returnItem();

    while (digitalRead(homeSensor1) == true && digitalRead(homeSensor2) == true) {
      stepperStep(HIGH);
    }

    // Write 0 to EEPROM address 0. 0 meaning there is no item on carriage.
    EEPROM.update(0, 0);
  }
  
  if (digitalRead(shelf1) == LOW && EEPROM.read(0) != 1) {
    returnAndRetrieve(shelf1Distance, 1);
  }
  
  if (digitalRead(shelf2) == LOW && EEPROM.read(0) != 2) {
    returnAndRetrieve(shelf2Distance, 2);
  }
}
