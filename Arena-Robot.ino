#include <SendOnlySoftwareSerial.h>
#include <ArduinoInit.h>

//motor 1 is left
//motor 2 is right

unsigned int floorSensor;
unsigned int leftSensor;
unsigned int frontSensor;
unsigned int rightSensor;
unsigned int beaconSensor;
char base;
boolean onWhite;
boolean change = false; //has the color of the floor changed since the last reading?
float coords[2]; // x coord = 0, y coord = 1;
char motorStatus = 'x'; // indicates which way the robot is currently going; x == stopped, w == forward, a == left, s == backward, d == right
unsigned long startTime;

void setup() {
  configArduino();
  attachInterrupt(0, leftBumper, LOW);
  attachInterrupt(1, rightBumper, LOW);   
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
  else  { //if we're not home, seek enemy light
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
    if (base == 'w') return true;
    else return false;
  }
  else { //currently on black
    if (base == 'b') return true;
    else return false;
  }
}

void beacon() {
  int beaconInit, beaconFinal;
  beaconInit = readADC(4);
  leftD(30);
  beaconFinal = readADC(4);
  if (beaconFinal - beaconInit > 0) { // if the beacon light increased, keep turning left
    left();
    startTime = millis();
    while (beaconSensor < 18000) { //turn until within a certain range of the beacon light
      if (millis() - startTime >= 684) { //if the robot has made a full rotation without exiting, something went wrong, go forward a bit and start over
        forwardD(.25);
        break;
      }
      beaconSensor = readADC(4);
    }
  }
  else {
    right();
    startTime = millis();
    while (beaconSensor < 18000) { //same as above, except in the opposite direction
      if (millis() -startTime >= 684) {
        forwardD(.25);
        break;
      }
      beaconSensor = readADC(4);
    }
  }
  return;
}

void enemy() {
  leftSensor = readADC(3) - 300;
  frontSensor = readADC(2);
  rightSensor = readADC(1) + 500;
  if (leftSensor < 16000 || frontSensor < 18000 || rightSensor < 16000) { //if we are close to a light, seek it out
    if ((frontSensor >= leftSensor && frontSensor >= rightSensor) || abs(frontSensor - leftSensor) <= 500 || abs(frontSensor - leftSensor) <= 500) forward(); //if the front sensor is receiving the most light, or very similar to another sensor, go forward
    else {
      startTime = millis();
      if (leftSensor <= rightSensor) { //if the left sensor is stronger than the right, turn left until the front sensor is stronger
        left();
        while (leftSensor < frontSensor) {
          if (millis() - startTime >= 684) {
            forwardD(.25);
            break;
          }
          leftSensor = readADC(3) - 300;
          frontSensor = readADC(2);
          rightSensor = readADC(1) + 500;
        }
      }
      else { //if the left sensor is not stronger than the right, turn right until the front sensor is stronger than the right one
        right();
        
        while (rightSensor < frontSensor) {
          if (millis() - startTime >= 684) {
            forwardD(.25);
            break;
          }
          leftSensor = readADC(3) - 300;
          frontSensor = readADC(2);
          rightSensor = readADC(1) + 500;
        }
      }
    }
  }
  else forward(); //probably should replace this with something more complicated and productive in the future
  return;
}

//x coordinate = coords[0]
//y coordinate = coords[1] 
float lightPos(int x) { //gives approximate position of the closest light, assuming (coords[0], coords[1]) is a point on a plane where the robot is at (0,0)
  leftSensor = readADC(3);
  frontSensor = readADC(2);
  rightSensor = readADC(1);
  coords[0] = (pow(leftSensor, 2) - pow(rightSensor,2)) / 4000;
  coords[1] = .25 * sqrt(-pow(leftSensor, 4) + 2*pow(leftSensor, 2)*pow(leftSensor, 2) + 8*pow(leftSensor, 2) - pow(rightSensor, 4) + 8*pow(rightSensor, 2) - 16); //gives y coordinate because of circles or something, doesn't work right now
  if (abs(sqrt(pow(coords[0], 2) + pow(coords[1] - 1, 2)) - frontSensor) > 1000) coords[1] = coords[1] * -1; //same thing, doesn't work either
  return coords[x];
}

void leftBumper() { //replace with something more complicated in the future
  backD(.25);
  rightD(45);
  forwardD(.25);
  return;
  
  /*
  pause(5);
  motors('b', 'o', 0);
  backD(.25);
  xcoord = coords[0];
  ycoord = coords[1];
  if (xcoord > 0) {
    if (ycoord > 0) rightD(90 - 180/PI * atan(ycoord/xcoord));
    else rightD(90 + 180/PI * atan(ycoord/xcoord));
  }
  else {
    if (ycoord > 0) leftD(90 - 180/PI * atan(ycoord/xcoord));
    else leftD(90 + 180/PI * atan(ycoord/xcoord));
  }
  */
}

void rightBumper() { //see leftBumper
  backD(.25);
  leftD(45);
  forwardD(.25);
  return;
}

