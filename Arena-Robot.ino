#include <SendOnlySoftwareSerial.h>
#include <ArduinoInit.h>

//motor 1 is left
//motor 2 is right

unsigned int voltage;
boolean onWhite;
boolean change = false;
char base;

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

void forwardD(int distance){
  pause(100);
  motors('1', 'a', 50);
  motors('2', 'a', 60);
  pause(distance * (1 / .009093));
  motors('b', 'o', 0);
  return;
}

void backD(int distance) {
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
}

void seekEnemy() {
  forward();
  while (isHome() == true) {
    if (lightIncrease() == true) forward();
    else {
      leftD(180);
      forward();
      pause(200);
    }
  }
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

void leftBumper() {
  
}

void rightBumper() {
  
}

