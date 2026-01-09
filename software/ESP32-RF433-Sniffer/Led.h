/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#ifndef LED_H
#define LED_H

#include <functional>
#include <jled.h>
#include "RGBCC.h"

/**
 * Led class
 */
class Led {
  public:
    /**
     * Constructor
     *
     * @param rgbLed The instance of RGBCC
     */
    Led (RGBCC* rgbLed);

    /**
     * Wrapper for RGBCC.update() and timeout management.
     * Must be called in the "loop".
     */
    void update ();

    /**
     * Start led effect for main menu
     */
    void mainMenuState ();
    /**
     * Start led effect for transmit mode
     */
    void transmitMenuState ();
    /**
     * Start led effect for receive mode
     */
    void receiveMenuState ();
    /**
     * Start led effect for "ready to transmit"
     */
    void readyToTransmitState ();
    /**
     * Start led effect for "ready to receive"
     */
    void readyToReceiveState ();
    /**
     * Start led effect for sending signal
     */
    void sendingState ();
    /**
     * Start led effect for receiving signal
     */
    void receivingState ();

    /**
     * Do something after the given duration in a non-blocking way
     *
     * @param callback The callback to execute after the given duration
     * @param duration The duration (in ms)
     */
    void setTimeout (std::function<void()> callback, unsigned long duration);

  private:
    RGBCC* _rgbLed;

    unsigned long _timerStart = 0;
    unsigned long _duration = 0;
    bool _timerActive = false;
    std::function<void()> _callback = nullptr;
};

#endif
