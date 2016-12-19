#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "Arduino.h"
#include "elapsedMillis.h"
#include "GameCell.h"
#include "PatternRecorder.h"

enum GAME_STATE {
  INITIAL,
  RECORDING,
  REVEALING
};

class GameController {
  public:
    GameController(unsigned int sp[], unsigned int size) {
      this->_gameState = INITIAL;
      this->_timer = 0;
      this->_recorder = new PatternRecorder(size);
      this->_size = size;
      this->_cells = new GameCell* [size];
      // change this part to add the ids like this :
      // this->cells[0] = new GameCell(0, 12, 400, 800,2000,1000);
      // GameCell(ledId,sensorPin,sensorLightRangeMin = 300, sensorLightRangeMax = 900, revealTimeLimit = 2000,recordTimeLimit = 2000)
      for(int i = 0 ; i < size ; ++i) {
        this->_cells[i] = new GameCell(i, sp[i]);
      }
      this->_ledState = 0;
    }
    byte getGamePattern() {
      return this->_ledState;
    }
    void update() {
      Serial.print("GAME STATE: ");
      Serial.print(this->_gameState);
      Serial.println();
      switch(this->_gameState) {
        case INITIAL:
          if(this->_timer > 5000){
            for(int i = 0 ; i < this->_size ; ++i) {
              this->_cells[i]->sensorStartRecording();
            }
             this->_gameState = (GAME_STATE)((int)this->_gameState + 1);
          }
        break;
        case RECORDING:
          if(!this->_recorder->isFull()) {
            for(int i = 0 ; i < this->_size ; ++i) {
              if(this->_cells[i]->getCellState() < DARK) {
                this->_cells[i]->updateCell();
                if(this->_cells[i]->getCellState() == DARK) { // now record this
                  if(this->_recorder->record(this->_cells[i]->getId())){
                    this->_ledState = this->_ledState | (1<<this->_cells[i]->getId());
                  } else {
                    Serial.print("PATTER IS FULL!!!");
                    Serial.println();
                  }
                  
                }
              }
            }
          } else { // now need to start revealing so alert with blinking or ... and start revealing
            Serial.print("PATTERN IS FULLY RECORDED, STARTING THE REVEAL PERIOD");
            Serial.println();
            for(int i = 0 ; i < this->_size ; ++i) {
              this->_cells[i]->sensorStartRevealing();
            }
            this->_gameState = (GAME_STATE)((int)this->_gameState + 1);
          }
        break;
        case REVEALING:
          if(this->_recorder->isEmpty()) { // game is successfully over , alert Win and go back to initial State!
            Serial.print("PATTER SUCCESSFULLY REVEALED! WINNNNNNNN!!!");
            Serial.println();
            for(int i = 0 ; i < this->_size ; ++i) {
              this->_cells[i]->sensorReset();
            }
            this->_gameState = INITIAL;
          } else { 
            for(int i = 0 ; i < this->_size ; ++i) {
              if(this->_cells[i]->getCellState() < REVEALED) {
                this->_cells[i]->updateCell();
                if(this->_cells[i]->getCellState() == REVEALED) { // now record this
                  if(this->_recorder->popIfValid(this->_cells[i]->getId())) {
                    this->_ledState = this->_ledState & ~(1<<this->_cells[i]->getId());
                    Serial.print("LED ");
                    Serial.print(this->_cells[i]->getId());
                    Serial.print(" was successfully revealed! GO FOR NEXT");
                    Serial.println();
                  } else {
                    Serial.print("THIS IS NOT THE RIGHT LED OR PATTERN EMPTY!! LOSERRR OR ...!!");
                    Serial.println();
                  }
                }
              }
            }
          }
        break;
      }
      
    }
  private:
  elapsedMillis _timer;
  GAME_STATE _gameState;
  PatternRecorder* _recorder;
  unsigned int _size;
  GameCell** _cells;
  byte _ledState;
};

#endif
