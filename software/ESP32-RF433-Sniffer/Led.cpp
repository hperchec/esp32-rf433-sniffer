/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#include <Arduino.h>
#include "Led.h"
#include "Config.h"

// Led class constructor
Led::Led(RGBCC* rgbLed) : _rgbLed(rgbLed) {
  // ...
}

void Led::update() {
  _rgbLed->update();

  if (_timerActive && (millis() - _timerStart >= _duration)) {
    _timerActive = false;
    if (_callback) {
      _callback();
    }
  }
}

void Led::mainMenuState () {
  _rgbLed->color(PURPLE_COLOR);
  _rgbLed->breathe(2000);
}

void Led::transmitMenuState () {
  _rgbLed->color(TURQUOISE_COLOR);
  _rgbLed->breathe(2000);
}

void Led::receiveMenuState () {
  _rgbLed->color(GREEN_COLOR);
  _rgbLed->breathe(2000);
}

void Led::readyToTransmitState () {
  _rgbLed->color(TURQUOISE_COLOR);
  _rgbLed->on(50); // 50%
}

void Led::readyToReceiveState () {
  _rgbLed->color(GREEN_COLOR);
  _rgbLed->on(50); // 50%
}

void Led::sendingState () {
  _rgbLed->color(TURQUOISE_COLOR);
  _rgbLed->blink(25, 25, 8);
}

void Led::receivingState () {
  _rgbLed->color(GREEN_COLOR);
  _rgbLed->blink(25, 25, 8);
}

void Led::setTimeout (std::function<void()> callback, unsigned long duration) {
  _timerStart = millis();
  _duration = duration;
  _callback = callback;
  _timerActive = true;
}
