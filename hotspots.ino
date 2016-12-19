#include "elapsedMillis.h"
#include "GameController.h"

GameController* controller;
byte currentState = 0;
unsigned int sensorPins[] = {A0,A1};

int latchPin = 13;
int clockPin = 12;
int dataPin = 11;


void printLedsState() {
  Serial.println("% LEDS STATE%");
  Serial.print(currentState, BIN);
  Serial.println("*END*");
}
void clearLeds() {
  digitalWrite(latchPin, 0);
  shiftOut(0);
  digitalWrite(latchPin, 1);
  currentState = 0;
}
void shiftOut(byte myDataOut) {
  digitalWrite(latchPin, 0);
  int i=0;
  int pinState;
  digitalWrite(dataPin, 0);
  digitalWrite(clockPin, 0);
  for (i=7; i>=0; i--)  {
    digitalWrite(clockPin, 0);
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {  
      pinState= 0;
    }
    digitalWrite(dataPin, pinState);
    digitalWrite(clockPin, 1);
    digitalWrite(dataPin, 0);
  }
  digitalWrite(clockPin, 0);
  digitalWrite(latchPin, 1);
}
byte evaluateState(byte cs) {

  return cs;
}
void setup() {
  Serial.begin(9600);
  
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  for(int i = 0; i < 2 ; i++) {
    pinMode(sensorPins[i], INPUT);
  } 
  controller = new GameController(sensorPins,2);
  clearLeds();
}
void loop() {
  controller->update();
  byte newState = controller->getGamePattern();
  if(currentState != newState) {
    shiftOut(newState);
    currentState = newState;
  }
  printLedsState();
  delay(200);
}
