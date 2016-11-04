#include <SendOnlySoftwareSerial.h>
#include <ArduinoInit.h>

//motor 1 is left
//motor 2 is right

unsigned int voltage;
unsigned int leftSensor;
unsigned int frontSensor;
unsigned int rightSensor;
char base;
boolean onWhite;
boolean change = false;
float coords[2]; // x coord = 0, y coord = 1;


void setup() {
  configArduino();
  attachInterrupt(0, leftBumper, LOW);
  attachInterrupt(1, rightBumper, LOW);   
  outputHigh(4);
  pause(100);
  voltage = readADC(0);
  if (voltage <= 19000) base = 'w';
  if (voltage > 19000) base = 'b';
  pause(100);
}

void loop() {
  if (isHome() == true) beacon();
  else enemy();
}

void forward() {
  pause(100);
  motors('1', 'a', 50);
  motors('2', 'a', 60);
  return;
}

void back() {
  pause(100);
  motors('1', 'b', 50);
  motors('2', 'b', 60);
  return;
}

void left() {
  pause(100);
  motors('1', 'a', 50);
  motors('2', 'b', 60);
  return;
}

void right() {
  pause(100);
  motors('1', 'b', 50);
  motors('2', 'a', 60);
  return;
}

void forwardD(float distance){
  pause(100);
  motors('1', 'a', 50);
  motors('2', 'a', 60);
  pause(distance * (1 / .009093));
  motors('b', 'o', 0);
  return;
}

void backD(float distance) {
  pause(100);
  motors('1', 'b', 50);
  motors('2', 'b', 60);
  pause(distance * (1 / .009093));
  motors('b', 'o', 0);
  return;
}

void leftD(float degree) {
  pause(100);
  motors('1', 'a', 50);
  motors('2', 'b', 60);
  pause(degree * 3.8);
  motors('b', 'o', 0);
  return;
}

void rightD(float degree) {
  pause(100);
  motors('1', 'b', 50);
  motors('2', 'a', 60);
  pause(degree * 3.75);
  motors('b', 'o', 0);
  return;
}

boolean isHome() { //tests if the robot is currently on friendly ground
  voltage = readADC(0);
  if (voltage <= 20000) {
    onWhite = true;
  }
  if (voltage > 20000) {
    onWhite = false;
  }
  if (base == 'w') {
    if (onWhite == true) return true;
    else return false;
  }
  if (base == 'b') {
    if (onWhite == false) return true;
    else return false;
  }
  motors('b', 'o', 0);
}

void beacon() {
  forward();
    while (isHome() == true) forward();
  return;
}

void enemy() {
  leftSensor = readADC(3) - 300;
  frontSensor = readADC(2);
  rightSensor = readADC(1) + 500;
  if (leftSensor < 16000 || frontSensor < 18000 || rightSensor < 16000) { //seeks out a light (just goes forward for now) until it gets within a certain range of a light
    if (frontSensor <= leftSensor && frontSensor <= rightSensor) forward(); //if the front sensor is receiving the most light, go forward
    else {
      motors('b', 'o', 0);
      pause(100);
      if (leftSensor <= rightSensor) { //if the left sensor is stronger than the right, turn left until the front sensor is stronger
        left();
        while (leftSensor < frontSensor) {
          leftSensor = readADC(3) - 300;
          frontSensor = readADC(2);
          rightSensor = readADC(1) + 500;
        }
      }
      else { //if the left sensor is not stronger than the right, turn right until the front sensor is stronger than the right one
        right();
        while (rightSensor < frontSensor) {
          leftSensor = readADC(3) - 300;
          frontSensor = readADC(2);
          rightSensor = readADC(1) + 500;
        }
      }
      pause(100); 
      motors('b', 'o', 0);
      pause(100);
    }
  }
  else forward(); //probably should replace this with something more complicated and productive in the future
  return;
}

/*
  if (leftSensor >= rightSensor) {
      if (lightPos(1) >= 0) leftD((180 * PI) * atan(lightPos(1) / lightPos(2)));
      else leftD(90 + (180 * PI) * atan(lightPos(1) / lightPos(2)));
    }
    else {
      if (lightPos(1) >= 0) rightD((180 * PI) * atan(lightPos(1) / lightPos(2)));
      else rightD(90 + (180 * PI) * atan(lightPos(1) / lightPos(2)));
    }
*/

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
  pause(5);
  motors('b', 'o', 0);
  backD(.25);
  pause(100);
  rightD(45);
  pause(100);
  forward();
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
  pause(5);
  motors('b', 'o', 0);
  backD(.25);
  pause(100);
  leftD(45);
  pause(100);
  forward();
  return;
}

