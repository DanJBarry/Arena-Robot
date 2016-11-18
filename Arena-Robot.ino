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
boolean change = false; //has the color of the floor changed since the last reading?
float coords[2]; // x coord = 0, y coord = 1;
char motorStatus = 'x'; // indicates which way the robot is currently going; x == stopped, w == forward, a == left, s == backward, d == right
unsigned long startTime; //for use with millis()

void setup() {
  configArduino();
  attachInterrupt(0, bumpers, LOW);
  attachInterrupt(1, bumpers, LOW);   
  outputHigh(4);
  pause(100);
  floorSensor = readADC(0);
  if (floorSensor <= 19000) base = 'w'; //home is white
  if (floorSensor > 19000) base = 'b'; //home is black
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
    motors('1', 'a', 50);
    motors('2', 'a', 60);
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
    motors('1', 'b', 50);
    motors('2', 'b', 60);
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
    motors('1', 'a', 50);
    motors('2', 'b', 60);
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
    motors('1', 'b', 50);
    motors('2', 'a', 60);
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
  if (floorSensor <= 20000) { //currently on white
    return (base == 'w');
  }
  else { //currently on black
    return (base == 'b');
  }
}

void beacon() {
  signed int beaconInit, beaconFinal;
  beaconInit = readADC(5);
  if (beaconInit > 16000) {
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
    beaconFinal = readADC(5);
    if ((beaconFinal < beaconInit)) turnBeacon('l');
    else turnBeacon('r');
  }
  else forward();
  return;
}

void turnBeacon(char turn) {
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
}

void enemy() {
  leftSensor = readADC(3) - 300;
  frontSensor = readADC(2);
  rightSensor = readADC(1) + 500;
  if (leftSensor < 16000 && frontSensor < 18000 && rightSensor < 16000) { //if we are close to a light, seek it out
    if ((frontSensor <= leftSensor && frontSensor <= rightSensor) || abs(frontSensor - leftSensor) <= 500 || abs(frontSensor - leftSensor) <= 500) forward(); //if the front sensor is receiving the most light, or very similar to another sensor, go forward
    else {
      if (leftSensor <= rightSensor) turnEnemy('l');
      else turnEnemy('r');
    }
  }
  else forward(); //probably should replace this with something more complicated and productive in the future
  return;
}

void turnEnemy(char turn) { //turns the given direction until the front sensor is stronger
  startTime = millis();
  if (turn == 'l') left();
  if (turn == 'r') right();
  int targetSensor;
  while (abs(frontSensor - targetSensor) >= 250 && leftSensor < frontSensor) { 
    if (millis() - startTime >= 684) { //if the robot makes a full rotation without getting within range, move forward a bit and start over
      forwardD(1);
      break;
    }
    frontSensor = readADC(2);
    if (turn == 'l') targetSensor = readADC(3) - 300;
    if (turn == 'r') targetSensor = readADC(1) + 500;
  }
  return;
}

//x coordinate = coords[0]
//y coordinate = coords[1] 
float lightPos(int x) { //gives approximate position of the closest light, assuming (coords[0], coords[1]) is a point on a plane where the robot is at (0,0), doesn't work right now
  leftSensor = readADC(3);
  frontSensor = readADC(2);
  rightSensor = readADC(1);
  coords[0] = (pow(leftSensor, 2) - pow(rightSensor,2)) / 4000;
  coords[1] = .25 * sqrt(-pow(leftSensor, 4) + 2*pow(leftSensor, 2)*pow(leftSensor, 2) + 8*pow(leftSensor, 2) - pow(rightSensor, 4) + 8*pow(rightSensor, 2) - 16); //gives y coordinate because of circles or something
  if (abs(sqrt(pow(coords[0], 2) + pow(coords[1] - 1, 2)) - frontSensor) > 1000) coords[1] = coords[1] * -1; //same thing, doesn't work either
  return coords[x];
}

void bumpers() { //replace with something more complicated in the future
  pause(200);
  boolean leftHit = false, rightHit = false, bothHit = false;
  if (readInput(3) == 0) leftHit = true;
  if (readInput(2) == 0) rightHit = true;
  if (leftHit && rightHit == true) bothHit = true;
  backD(4);
  if (bothHit == true) {
    if (random(0,1) == 0) leftD(75);
    else rightD(45);
  }
  else {
    if (leftHit == true) rightD(45);
    if (rightHit == true) leftD(45);
  }
  forwardD(1);
}

