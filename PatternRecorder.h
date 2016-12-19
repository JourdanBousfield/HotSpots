#ifndef PATTERN_RECORDER_H
#define PATTERN_RECORDER_H

#include "Arduino.h"

class PatternRecorder
{
  public:
    PatternRecorder(unsigned int size) {
      this->_size  = size;
      this->_pattern = new unsigned int[this->_size];
      this->_tail = -1;
    }
    ~PatternRecorder() {
      delete this->_pattern;
    }
    bool popIfValid(unsigned int id) {
      if(this->isEmpty())return false;
      if(this->_pattern[this->_tail] != id) return false;
      this->_tail--;
      return true;
    }
    bool record(unsigned int id) {
      if(this->isFull())return false;
      this->_pattern[++_tail] = id;
      return true; 
    }
    bool isFull() {
      Serial.print(this->_tail);
      Serial.print("*");
      Serial.print(this->_size);
      Serial.println();
      
      return (this->_tail >= (int)(this->_size - 1) ? true : false);
    }
    bool isEmpty() {
      return (this->_tail == -1 ? true : false);
    }
  private:
    unsigned int* _pattern;
    unsigned int _size;
    int _tail;
};

#endif
