#include <SendOnlySoftwareSerial.h>
#include <ArduinoInit.h>

//motor 1 is left
//motor 2 is right

unsigned int floorSensor; //readADC(0)
unsigned int leftSensor; //readADC(3)
unsigned int frontSensor; //readADC(2)
unsigned int rightSensor; //readADC(1)
unsigned int beaconSensor; //readADC(5)
char base; // 'b' for black home, 'w' for white home
boolean onWhite;s
char motorStatus = 'x'; // indicates which way the robot is currently going; x == stopped, w == forward, a == left, s == backward, d == right
unsigned long startTime; //for use with millis()
unsigned long leftTimer = NULL;
unsigned long rightTimer = NULL;
unsigned int currentBeacon = NULL;
unsigned int lastBeacon = NULL;
unsigned long beaconTimer = NULL;
unsigned long lastBeaconCheck = NULL;
char lastTurn = 'l';

void setup() {
  configArduino();
  attachInterrupt(0, bumpers, LOW);
  attachInterrupt(1, bumpers, LOW);   
  outputHigh(4);
  pause(100);
  floorSensor = readADC(0);
  if (floorSensor <= 14000) base = 'w'; //home is white
  if (floorSensor > 14000) base = 'b'; //home is black
}

void loop() {
  if (isHome() == true) { //if we're home, seek beacon light
    outputHigh(12);
    outputLow(10);
    beacon();
  }
  else { //if we're not home, seek enemy light
    outputHigh(10);
    outputLow(12);
    enemy();
  }
}

void halt() {
  motors('b', 'o', 0);
  motorStatus = 'x';
  return;
}

void forward() {
  if (motorStatus != 'w') {
    if (motorStatus = 'a') motors('1', 'o', 0); //only stops motor(s) not turning the desired direction
    if (motorStatus = 's') motors('b', 'o', 0);
    if (motorStatus = 'd') motors('2', 'o', 0);
    if (motorStatus != 'x') pause(100); //the pause is unneccessary if the motors are stopped
    motors('1', 'a', 80);
    motors('2', 'a', 90);
    motorStatus = 'w';
  }
  return;
}

void back() {
  if (motorStatus != 's') {
    if (motorStatus = 'w') motors('b', 'o', 0);
    if (motorStatus = 'a') motors('2', 'o', 0);
    if (motorStatus = 'd') motors('1', 'o', 0);
    if (motorStatus != 'x') pause(100);
    motors('1', 'b', 80);
    motors('2', 'b', 90);
    motorStatus = 's';
  }
  return;
}

void left() {
  if (motorStatus != 'a') {
    if (motorStatus = 'w') motors('1', 'o', 0);
    if (motorStatus = 's') motors('2', 'o', 0);
    if (motorStatus = 'd') motors('b', 'o', 0);
    if (motorStatus != 'x') pause(100);
    motors('1', 'a', 80);
    motors('2', 'b', 90);
    motorStatus = 'a';
  }
  return;
}

void right() {
  if (motorStatus != 'd') {
    if (motorStatus = 'w') motors('2', 'o', 0);
    if (motorStatus = 'a') motors('b', 'o', 0);
    if (motorStatus = 's') motors('1', 'o', 0);
    if (motorStatus != 'x') pause(100);
    motors('1', 'b', 80);
    motors('2', 'a', 90);
    motorStatus = 'd';
  }
  return;
}

void forwardD(float distance){
  forward();
  pause(distance * (1 / .009093));
  halt();
  return;
}

void backD(float distance) {
  back();
  pause(distance * (1 / .009093));
  halt();
  return;
}

void leftD(float degree) {
  left();
  pause(degree * 3.8);
  halt();
  return;
}

void rightD(float degree) {
  right();
  pause(degree * 3.75);
  halt();
  return;
}

boolean isHome() { //tests if the robot is currently on friendly ground
  floorSensor = readADC(0);
  if (floorSensor <= 15000) { //currently on white
    return (base == 'w');
  }
  else { //currently on black
    return (base == 'b');
  }
}

void beacon() {
  currentBeacon = readADC(5);
  if (lastBeacon == NULL) lastBeacon = currentBeacon;
  if (currentBeacon < 8000) {
    forward();
    beaconTimer = NULL;
  }
  else {
    if (motorStatus == 'w') {
      if (lastTurn == 'l') left();
      if (lastTurn == 'r') right();
    }
    if (beaconTimer == NULL) {
      beaconTimer = millis();
      lastBeaconCheck = millis();
    }
    if (millis() - beaconTimer > 1000) {
      forwardD(1.5);
      beaconTimer = NULL;
      return;
    }
    if (millis() - lastBeaconCheck >= 100) {
      if (currentBeacon >= lastBeacon) {
        if (motorStatus == 'a') {
          right();
          lastTurn = 'r';
        }
        else {
          left();
          lastTurn = 'l';
        }
        beaconTimer = NULL;
      }
      lastBeacon = currentBeacon;
      lastBeaconCheck = millis();
    }
  }
  return;
}

/*void beacon() {
  signed int beaconInit, beaconFinal;
  beaconInit = readADC(5);
  if (beaconInit > 8000) {
    left();
    startTime = millis();
    pause(200);
    /*
    while ((millis() - startTime) >= 114) {
      if (readADC(5) < 14000) {
        forward();
        return;
      }
    }
    */
    /*beaconFinal = readADC(5);
    if ((beaconFinal < beaconInit)) left();
    else right();
  }
  else forward();
  return;
}

void turnBeacon(char turn) {
  if (readADC(5) > 16000) return;
  startTime = millis();
  if (turn == 'l') left();
  if (turn == 'r') right();
  while (beaconSensor > 16000) { //turn until within a certain range of the beacon light
    if ((millis() - startTime) > 1000) { //if the robot has made a full rotation without exiting, something went wrong, go forward a bit and start over
      forwardD(1);
      break;
    }
    beaconSensor = readADC(5);
  }
  return;
}*/

void enemy() {
  leftSensor = readADC(3);
  frontSensor = readADC(2);
  rightSensor = readADC(1);
  char turn;
  if (leftSensor < rightSensor && leftSensor <frontSensor) turn = 'l';
  if (rightSensor < leftSensor && rightSensor < frontSensor) turn = 'r';
  if (frontSensor < leftSensor && frontSensor < rightSensor) turn = 'f';
  Serial.println("Enemy");
  switch (turn) {
    case 'l':
      rightTimer = NULL;
      if (leftTimer == NULL) leftTimer = millis();
      if (millis() - leftTimer > 1000) {
        forwardD(4);
        leftTimer = NULL;
        return;
      }
      Serial.println("Left Target");
      left();
      pause(200);
      break;
    case 'r':
      leftTimer = NULL;
      if (rightTimer == NULL) rightTimer = millis();
      if (millis() - rightTimer > 1000) {
        forwardD(4);
        rightTimer = NULL;
        return;
      }
      Serial.println("Right Target");
      right();
      pause(200);
      break;
    case 'f':
      leftTimer = NULL;
      rightTimer = NULL;
      Serial.println("Front Target");
      forward();
      pause(200);
  }
  return;
}

void bumpers() {
  pause(200);
  boolean leftHit = false, rightHit = false, bothHit = false;
  if (readInput(3) == 0) leftHit = true;
  if (readInput(2) == 0) rightHit = true;
  if (leftHit && rightHit) bothHit = true;
  backD(7);
  if (bothHit == true) {
    if (random(0,1) == 0) leftD(100);
    else rightD(100);
  }
  else {
    if (leftHit == true) rightD(100);
    if (rightHit == true) leftD(100);
  }
  forwardD(1);
}

