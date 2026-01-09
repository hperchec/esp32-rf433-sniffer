/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#include "Utils.h"

String readSerialCommand () {
  if (Serial.available() > 0) {
    // Read entire line
    String input = Serial.readStringUntil('\n');

    // Clean entry and force uppercase
    input.trim();
    input.toUpperCase();

    return input;
  } else {
    return "";
  }
}

const char* bin2tristate(const char* bin) {
  static char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
};

char* dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';

  return bin;
};

void logData (Type1Data data) {
  Serial.print(F("Decimal     : ")); Serial.print(data.decimal); Serial.print(F(" (")); Serial.print(data.length); Serial.println(F("Bit)"));
  Serial.print(F("Binary      : ")); Serial.println(data.bin);
  Serial.print(F("Tri-State   : ")); Serial.println(data.triState);
  Serial.print(F("PulseLength : ")); Serial.print(data.delay); Serial.println(F(" microseconds"));
  Serial.print(F("Protocol    : ")); Serial.println(data.protocol);
  Serial.print(F("Raw data    : "));
  if (data.raw) {
    for (unsigned int i=0; i<= data.length*2; i++) {
      Serial.print(data.raw[i]);
      Serial.print(",");
    }
  } else {
    Serial.print(F("-"));
  }
  Serial.println();
};

void logData (Type2Data data) {
  Serial.print(F("ID         : ")); Serial.println(data.address);
  Serial.print(F("Period     : ")); Serial.print(data.period); Serial.println(F(" microseconds"));
  Serial.print(F("Unit       : ")); Serial.println(data.unit);
  Serial.print(F("GroupBit   : ")); Serial.println(data.groupBit);
  Serial.print(F("State      : ")); Serial.println(data.switchType ? "ON" : "OFF");
  Serial.print(F("DIM level  : ")); Serial.println(data.dimLevel);
};

Repeater::Repeater () {
  // ...
}

void Repeater::start (std::function<void(unsigned long count)> callback, unsigned long repeat, unsigned long delay) {
  _callback = callback;
  _delay = delay;
  _numRepetitions = repeat;
  _repeatCount = 0;
  _startedAt = millis();
  _isRunning = true;
}

void Repeater::stop () {
  if (_stopCallback) {
    _stopCallback(_repeatCount);
  }
  // Reset values
  _callback = nullptr;
  _delay = 1000;
  _numRepetitions = 1;
  _repeatCount = 0;
  _isRunning = false;
}

bool Repeater::isRunning () {
  return _isRunning;
}

void Repeater::update () {
  if (_isRunning) {
    if (millis() - _startedAt >= (_delay * _repeatCount)) {
      if (_callback) {
        _callback(_repeatCount);
      }
      _repeatCount++;
      // Stop if repeat limit has been reached
      if (_repeatCount == _numRepetitions) {
        stop();
      }
    }
  }
}

void Repeater::onStop (std::function<void(unsigned long count)> callback, bool remove) {
  // Create new lambda function
  _stopCallback = [this, callback, remove](unsigned long count) {
    if (callback) {
      callback(count);
    }

    if (remove) {
      this->_stopCallback = nullptr;
    }
  };
}
