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
  outputHigh(4);
  pause(100);
  voltage = readADC(0);
  if (voltage <= 20000) base = 'w';
  if (voltage > 20000) base = 'b';
}

void loop() {
  left();
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
    if (voltage <= 20000) return true;
    else return false;
  }
  if (base == 'b') {
    if (voltage > 20000) return true;
    else return false;
  }
}

void forward() {
  motors('1', 'a', 50);
  motors('2', 'a', 60);
}

void back() {
  motors('1', 'b', 50);
  motors('2', 'b', 60);
}

void left() {
  motors('1', 'a', 50);
  motors('2', 'b', 60);
}

void right() {
  motors('1', 'b', 50);
  motors('2', 'a', 60);
}

void forwardD(float distance){
  
}

void backD(float distance) {
  
}

void leftD(float degree) {
  
}

void rightD(float degree) {
  
}

