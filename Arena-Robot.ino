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
boolean onWhite;
char motorStatus = 'x'; // indicates which way the robot is currently going; x == stopped, w == forward, a == left, s == backward, d == right
unsigned long startTime; //for use with millis()
unsigned long leftTimer = 0;
unsigned long rightTimer = 0;
unsigned int currentBeacon = 0;
unsigned int lastBeacon = 0;
unsigned long beaconTimer = 0;
unsigned long lastBeaconCheck = 0;
char lastTurn = 'l';
boolean firstRunBeacon = true;
boolean firstRunEnemy = true;
boolean directionChange = false;
unsigned long firstRunTimer = 0;
boolean bumperActivate = false;
unsigned long lastForwards[4];
unsigned long forwardTimer = 0;
unsigned int forwardCount = 0;

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

void moveDistance(char movement, float distance) {
  switch (movement) {
    case 'w':
      forward();
      break;
    case 'a':
      left();
      break;
    case 's':
      back();
      break;
    case 'd':
      right();
      break;
  }
  if (movement == 'w' || movement == 's') pause(distance * (1 / .009093));
  else pause(distance * 3.8);
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
  firstRunEnemy = false;
  if (firstRunBeacon == true) {
    moveDistance('w', 1);
    firstRunBeacon = false;
    return;
  }
  currentBeacon = readADC(5);
  if (lastBeacon == 0) lastBeacon = currentBeacon;
  if (currentBeacon < 8000) {
    if (motorStatus == 'a' || motorStatus == 'd') {
      forwardCount++;
      forwardTimer = millis();
    }
    forward();
    beaconTimer = 0;
    directionChange = false;
  }
  else {
    if (motorStatus == 'w') {
      lastForwards[0] = lastForwards[1];
       lastForwards[1] = lastForwards[2];
       lastForwards[2] = lastForwards[3];
       lastForwards[3] = millis() - forwardTimer;
      if (forwardCount > 3 && lastForwards[0] + lastForwards[1] + lastForwards[2] + lastForwards[3] < 1500) {
        forwardCount = 0;
        moveDistance('w', 4);
      }
      if (lastTurn == 'l') left();
      if (lastTurn == 'r') right();
      
    }
    if (beaconTimer == 0) {
      beaconTimer = millis();
      lastBeaconCheck = millis();
    }
    if (millis() - beaconTimer > 1600) {
      moveDistance('w', 1.5);
      beaconTimer = 0;
      directionChange = false;
      return;
    }
    if (millis() - lastBeaconCheck >= 200) {
      if (currentBeacon >= lastBeacon) {
        if (motorStatus == 'a') {
          right();
          lastTurn = 'r';
          directionChange = true;
        }
        else {
          left();
          lastTurn = 'l';
          directionChange = true;
        }
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
      moveDistance('w', 1);
      break;
    }
    beaconSensor = readADC(5);
  }
  return;
}*/

void enemy() {
  firstRunBeacon = false;
  currentBeacon = 0;
  lastBeacon = 0;
  beaconTimer = 0;
  lastBeaconCheck = 0;
  directionChange = false;
  if (firstRunEnemy == true){
    bumperActivate = false;
    firstRunTimer = millis();
    forward();
    while (bumperActivate == false || (millis() - firstRunTimer <= 4000)) {
    }
    return;
  }
  leftSensor = readADC(3);
  frontSensor = readADC(2);
  rightSensor = readADC(1);
  char turn;
  if (leftSensor < 20000 || frontSensor < 20000 || rightSensor < 20000){
    if (leftSensor < rightSensor && leftSensor <frontSensor) turn = 'l';
    if (rightSensor < leftSensor && rightSensor < frontSensor) turn = 'r';
    if (frontSensor < leftSensor && frontSensor < rightSensor) turn = 'f';
    Serial.println("Enemy");
    switch (turn) {
      case 'l':
        rightTimer = 0;
        if (leftTimer == 0) leftTimer = millis();
        if (millis() - leftTimer > 1000) {
          moveDistance('w', 4);
          leftTimer = 0;
          return;
        }
        Serial.println("Left Target");
        left();
        break;
      case 'r':
        leftTimer = 0;
        if (rightTimer == 0) rightTimer = millis();
        if (millis() - rightTimer > 1000) {
          moveDistance('w', 4);
          rightTimer = 0;
          return;
        }
        Serial.println("Right Target");
        right();
        break;
      case 'f':
        leftTimer = 0;
        rightTimer = 0;
        Serial.println("Front Target");
        forward();
    }
  }
  else forward();
  return;
}

void bumpers() {
  pause(200);
  boolean leftHit = false, rightHit = false, bothHit = false;
  if (readInput(3) == 0) leftHit = true;
  if (readInput(2) == 0) rightHit = true;
  if (leftHit && rightHit) bothHit = true;
  if (leftHit || rightHit) bumperActivate = true;
  moveDistance('s', 3);
  if (bothHit == true) {
    if (random(0,1) == 0) moveDistance('a', 50);
    else moveDistance('d', 50);
  }
  else {
    if (leftHit == true) moveDistance('d', 60);
    if (rightHit == true) moveDistance('a', 60);
  }
  moveDistance('w', 1);
  return;
}
