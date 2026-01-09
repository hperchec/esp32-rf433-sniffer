/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "RGBCC.h"

// You can adapt these values to fill your needs

// By default, it uses the GPIO27 to receive signals and GPIO14 to emit
// You can adapt these values to fill your needs
const int RX_PIN = 27;
const int TX_PIN = 14;

// Define the serial connection baud rate
const int SERIAL_BAUDRATE = 115200;

// R: GPIO18, G: GPIO19, B: GPIO21
const int RGB_LED_RED_PIN = 18;
const int RGB_LED_GREEN_PIN = 19;
const int RGB_LED_BLUE_PIN = 21;

// Custom led colors: { R, G, B }
const RGBColor PURPLE_COLOR = { 150, 0, 255 };
const RGBColor TURQUOISE_COLOR = { 0, 100, 200 };
const RGBColor GREEN_COLOR = { 50, 255, 0 };

#endif
