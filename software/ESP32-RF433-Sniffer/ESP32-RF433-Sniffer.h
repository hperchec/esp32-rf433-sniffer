/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

// By default, it uses the GPIO27 to receive signals and GPIO14 to emit
// You can adapt these values to fill your needs
const int RX_PIN = 27;
const int TX_PIN = 14;

/**
 * Data structure for protocol 1 ("old style" with RCSwitch)
 */
struct Prot1Data {
  unsigned long address;
  unsigned int unit;
  bool state;
  unsigned int period;
  // Flag to know if data is available
  volatile bool available = false;
};

/**
 * Data structure for protocol 2 ("new style" with NewRemoteSwitch)
 */
struct Prot2Data {
  unsigned int period;
  unsigned long address;
  unsigned long groupBit;
  unsigned int unit;
  bool state;
  unsigned char dimLevel;
  // Flag to know if data is available
  volatile bool available = false;
  // Clear data
  void clear() {
    memset(this, 0, sizeof(Prot2Data));
  }
};