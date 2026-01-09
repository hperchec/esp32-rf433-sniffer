/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#ifndef RGBCC_H
#define RGBCC_H

#include <jled.h>

struct RGBColor {
  int red;
  int green;
  int blue;
};

/**
 * RGB Led (Common Cathode)
 */
class RGBCC {
  public:
    /**
     * Constructor.
     *
     * @param rPin GPIO pin number for led red pin
     * @param gPin GPIO pin number for led green pin
     * @param bPin GPIO pin number for led blue pin
     */
    RGBCC (unsigned int rPin, unsigned int gPin, unsigned int bPin);

    /**
     * Set the breathe mode
     *
     * @param duration The duration (in ms). Default: 2000
     */
    void breathe (unsigned long duration = 2000);

    /**
     * Set the blink mode
     *
     * @param durationOn The duration of "on" state (in ms)
     * @param durationOff The duration of "off" state (in ms)
     * @param repeat Repeat number. Pass negative number for "forever". Default: -1 (forever)
     */
    void blink (unsigned long durationOn, unsigned long durationOff, long repeat = -1);

    /**
     * Turn on
     *
     * @param intensity The intensity in percent. Default: 100
     */
    void on (float intensity = 100);

    /**
     * Set the color values (R,G,B)
     *
     * @param r The red value
     * @param g The green value
     * @param b The blue value
     */
    void color (unsigned int r, unsigned int g, unsigned int b);
    /**
     * Set the color values (R,G,B)
     *
     * @param color The RGBColor object
     */
    void color (RGBColor color);

    /**
     * Update all JLed objects
     */
    void update ();

  protected:
    /**
     * GPIO pin number for led red pin
     */
    unsigned int _rPin;
    /**
     * GPIO pin number for led green pin
     */
    unsigned int _gPin;
    /**
     * GPIO pin number for led blue pin
     */
    unsigned int _bPin;
    /**
     * The JLed object for led red pin
     */
    JLed* _ledR;
    /**
     * The JLed object for led green pin
     */
    JLed* _ledG;
    /**
     * The JLed object for led blue pin
     */
    JLed* _ledB;
    /**
     * Red color value
     */
    unsigned int _rValue = 0;
    /**
     * Green color value
     */
    unsigned int _gValue = 0;
    /**
     * Blue color value
     */
    unsigned int _bValue = 0;
};

#endif
