#ifndef GAME_CELL_H
#define GAME_CELL_H

#include "Arduino.h"
#include "elapsedMillis.h"

typedef enum CELL_STATE {
  INACTIVE,
  WAITING_FOR_DARK,
  DARK_DETECTED,
  DARK,
  WAITING_FOR_LIGHT,
  LIGHT_DETECTED,
  REVEALED
}CELL_STATE;

class GameCell
{
  public:
    GameCell(unsigned int ledId, unsigned int sensorPin, unsigned int sensorLightRangeMin = 300, unsigned int sensorLightRangeMax = 900, unsigned int revealTL = 2000, unsigned int recordTL = 2000) {
      this->_sensorPin = sensorPin;
      this->_ledId = ledId;
      this->_rangeMax = sensorLightRangeMax;
      this->_rangeMin = sensorLightRangeMin;
      this->_recordingTimeLimit = recordTL;
      this->_revealingTimeLimit = revealTL;
      this->_cellState = INACTIVE;
      this->_timer = 0;
    }
    void setRangeMin(unsigned int newMin) {
      if(newMin >= 0 && newMin <= 1024) this->_rangeMin = newMin;
    }
    void setRangeMax(unsigned int newMax) {
      if(newMax >= 0 && newMax <= 1024) this->_rangeMax = newMax;
    }
    void sensorReset() {
      this->_cellState = INACTIVE;
    }
    unsigned int getId() {
      return this->_ledId;
    }
    void sensorStartRecording() {
      this->_cellState = WAITING_FOR_DARK;
    }
    void sensorStartRevealing() {
      this->_cellState = WAITING_FOR_LIGHT;
    }
    CELL_STATE getCellState() {
      return this->_cellState;
    }
    void setCellState(CELL_STATE s) {
      this->_cellState = s;
    }
    void updateCell() {
      float currentLightVal = analogRead(this->_sensorPin);
      switch(this->_cellState) {
        case INACTIVE:
          Serial.print("Sensor ");
          Serial.print(this->_sensorPin);
          Serial.print(" is in INACTIVE state!");
          Serial.println();
        break;
        case WAITING_FOR_DARK:
          if(currentLightVal < this->_rangeMin) {
            this->_timer = 0;
            this->_cellState = (CELL_STATE)((int)this->_cellState + 1);
          }
        break;
        case DARK_DETECTED:
          if(currentLightVal > this->_rangeMin) {
            this->_cellState = (CELL_STATE)((int)this->_cellState - 1);
          }else if(this->_timer > this->_recordingTimeLimit){
            this->_cellState = (CELL_STATE)((int)this->_cellState + 1);
          }
        break;
        case DARK:
          Serial.print("Sensor ");
          Serial.print(this->_sensorPin);
          Serial.print(" is in DARK state!");
          Serial.println();
        break;
        case WAITING_FOR_LIGHT:
           if(currentLightVal > this->_rangeMax) {
            this->_timer = 0;
            this->_cellState = (CELL_STATE)((int)this->_cellState + 1);
          }
        break;
        case LIGHT_DETECTED:
           if(currentLightVal < this->_rangeMax) {
            this->_cellState = (CELL_STATE)((int)this->_cellState - 1);
          }else if(this->_timer > this->_revealingTimeLimit){
            this->_cellState = (CELL_STATE)((int)this->_cellState + 1);
          }
        break;
         case REVEALED:
          Serial.print("Sensor ");
          Serial.print(this->_sensorPin);
          Serial.print(" is in REVEALED state!");
          Serial.println();
        break;
      }

    }
  private:
    elapsedMillis _timer;
    int _sensorPin;
    int _ledId;
    unsigned int _rangeMin;
    unsigned int _rangeMax;
    unsigned int _recordingTimeLimit;
    unsigned int _revealingTimeLimit;
    CELL_STATE _cellState;
};

#endif
