/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#ifndef DATA_H
#define DATA_H

#include <Arduino.h>
#include <NewRemoteReceiver.h>

/**
 * Data structure for type 1 ("old style" with RCSwitch)
 */
struct Type1Data {
  unsigned long decimal;
  unsigned int length;
  unsigned int delay;
  unsigned int* raw;
  unsigned int protocol;
  char* bin;
  const char* triState;

  // Clear data
  void clear() {
    memset(this, 0, sizeof(Type1Data));
  }
};

/**
 * Data structure for type 2 ("new style" with NewRemoteSwitch)
 */
struct Type2Data : NewRemoteCode {
  // Clear data
  void clear() {
    memset(this, 0, sizeof(Type2Data));
  }
};

#endif
