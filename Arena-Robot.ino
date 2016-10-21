#include <SendOnlySoftwareSerial.h>
#include <ArduinoInit.h>

//motor 1 is left
//motor 2 is right

unsigned int voltage;
unsigned int leftSensor;
unsigned int frontSensor;
unsigned int rightSensor;
float xcoord;
float ycoord;
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
  if (voltage <= 20000) base = 'w';
  if (voltage > 20000) base = 'b';
  pause(100);
}

void loop() {
  if (isHome() == true) seekEnemy();
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

boolean isHome() { 
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

void seekEnemy() {
  forward();
  while (isHome() == true) forward();
  return;
}

boolean lightIncrease() {
  int vIS = 0, vFS = 0;
  float vIA, vFA;
  for (int i; i < 5; i++) {
    vIS += readADC(0);
    pause(50);
  }
  vIA = vIS / 5;
  pause(100);
  for (int i; i < 5; i++) {
    vIS += readADC(0);
    pause(50);
  }
  vFA = vFS / 5;
}

//x coordinate = coords[0]
//y coordinate = coords[1] 
float lightPos(int x) { //gives approximate position of the closest light, assuming (coords[0], coords[1]) is a point on a plane where the robot is at (0,0)
  leftSensor = readADC(0);
  frontSensor = readADC(0);
  rightSensor = readADC(0);
  coords[0] = .25 * (pow(leftSensor, 2) - pow(rightSensor,2));
  coords[1] = .25 * sqrt(-pow(leftSensor, 4) + 2*pow(leftSensor, 2)*pow(leftSensor, 2) + 8*pow(leftSensor, 2) - pow(rightSensor, 4) + 8*pow(rightSensor, 2) - 16);
  if (abs(sqrt(pow(xcoord, 2) + pow(ycoord - 1, 2)) - frontSensor) > 1000) ycoord = ycoord * -1;
  return coords[x];
}

void leftBumper() {
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

void rightBumper() {
  pause(5);
  motors('b', 'o', 0);
  backD(.25);
  leftD(45);
  return;
}

