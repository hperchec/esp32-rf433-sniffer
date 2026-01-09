/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <functional>
#include "Data.h"

// Utils for Serial

String readSerialCommand ();

// Utils for RCSwitch data

/**
 * Convert binary to tristate.
 * See https://github.com/sui77/rc-switch/blob/master/examples/ReceiveDemo_Advanced/output.ino
 */
const char* bin2tristate(const char* bin);

/**
 * Convert decimal to binary with zero fill.
 * See https://github.com/sui77/rc-switch/blob/master/examples/ReceiveDemo_Advanced/output.ino
 */
char* dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

// Common utils

/**
 * Log the available data for type 1
 * @param data The data to log
 */
void logData (Type1Data data);

/**
 * Log the available data for type 2
 * @param data The data to log
 */
void logData (Type2Data data);

/**
 * Repeater class
 */
class Repeater {
  public:
    /**
     * Constructor
     */
    Repeater ();

    /**
     * Start the repetitions
     *
     * @param callback The callback to execute at each repetition
     * @param repeat The number of repetitions. Default: 1
     * @param delay The delay between each repetition (in ms). Default: 1000
     */
    void start (std::function<void(unsigned long count)> callback, unsigned long repeat = 1, unsigned long delay = 1000);

    /**
     * Stop the repeater
     */
    void stop ();

    /**
     * To know if the repeater is currently running
     */
    bool isRunning ();

    /**
     * Must be called in the "loop".
     */
    void update ();

    /**
     * Apply a callback when repeater is stopped
     *
     * @param callback The callback to execute when the repeater is stopped
     * @param remove Auto-remove the callback when executed. Set to false for permanent callback. Default: true
     */
    void onStop (std::function<void(unsigned long count)> callback, bool remove = true);

  private:
    /**
     * Flag to know if the repeater is currently running
     */
    bool _isRunning = false;
    /**
     * Save the exact time the repeater has been started
     */
    unsigned long _startedAt = 0;
    /**
     * The delay between each repetition (in ms)
     */
    unsigned long _delay = 1000;
    /**
     * The wanted number of repetition
     */
    unsigned long _numRepetitions = 1;
    /**
     * The current repetition count
     */
    unsigned long _repeatCount = 1;
    /**
     * The callback to execute at each repetition
     */
    std::function<void(unsigned long count)> _callback = nullptr;
    /**
     * The callback to execute when repeater is stopped
     */
    std::function<void(unsigned long count)> _stopCallback = nullptr;
};

#endif
