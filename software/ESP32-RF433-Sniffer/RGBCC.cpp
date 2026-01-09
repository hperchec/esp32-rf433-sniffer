/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#include <Arduino.h>
#include "RGBCC.h"

// See JLed sources for the "forever" value of num_repetitions (kRepeatForever)
static const uint16_t REPEAT_FOREVER = 65535;

// RGBCC class constructor
RGBCC::RGBCC (unsigned int rPin, unsigned int gPin, unsigned int bPin) {
  // Set GPIO pin numbers
  _rPin = rPin;
  _gPin = gPin;
  _bPin = bPin;

  // Set color values
  _rValue = 0;
  _gValue = 0;
  _bValue = 0;

  // Create JLed instances
  _ledR = new JLed(_rPin);
  _ledG = new JLed(_gPin);
  _ledB = new JLed(_bPin);

  // Turn off immediatly
  _ledR->Off();
  _ledG->Off();
  _ledB->Off();
}

void RGBCC::breathe (unsigned long duration) {
  _ledR->Breathe(duration).MaxBrightness(_rValue).Forever();
  _ledG->Breathe(duration).MaxBrightness(_gValue).Forever();
  _ledB->Breathe(duration).MaxBrightness(_bValue).Forever();
}

void RGBCC::blink (unsigned long durationOn, unsigned long durationOff, long repeat) {
  _ledR->Blink(durationOn, durationOff).MaxBrightness(_rValue).Repeat(repeat < 0 ? REPEAT_FOREVER : repeat);
  _ledG->Blink(durationOn, durationOff).MaxBrightness(_gValue).Repeat(repeat < 0 ? REPEAT_FOREVER : repeat);
  _ledB->Blink(durationOn, durationOff).MaxBrightness(_bValue).Repeat(repeat < 0 ? REPEAT_FOREVER : repeat);
}

void RGBCC::on (float intensity) {
  // Convert percent (0-100) to factor (0.0-1.0)
  float factor = constrain(intensity / 100.0f, 0.0f, 1.0f);

  unsigned int finalR = (unsigned int)(_rValue * factor);
  unsigned int finalG = (unsigned int)(_gValue * factor);
  unsigned int finalB = (unsigned int)(_bValue * factor);

  _ledR->On().MaxBrightness(finalR);
  _ledG->On().MaxBrightness(finalG);
  _ledB->On().MaxBrightness(finalB);
}

void RGBCC::color (unsigned int r, unsigned int g, unsigned int b) {
  _rValue = r;
  _gValue = g;
  _bValue = b;
}

void RGBCC::color (RGBColor color) {
  _rValue = color.red;
  _gValue = color.green;
  _bValue = color.blue;
}

void RGBCC::update () {
  _ledR->Update();
  _ledG->Update();
  _ledB->Update();
}