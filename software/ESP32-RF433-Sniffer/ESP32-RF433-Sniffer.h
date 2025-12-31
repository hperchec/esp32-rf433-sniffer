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

// Define the serial connection baud rate
const int SERIAL_BAUDRATE = 115200;

// Mode
enum Mode { NONE_MODE, RECEIVE_MODE, TRANSMIT_MODE };
// Type
enum Type { NONE_TYPE, OLD_STYLE, NEW_STYLE };

/**
 * Data structure for type 2 ("new style" with NewRemoteSwitch)
 */
struct Type2Data {
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
    memset(this, 0, sizeof(Type2Data));
  }
};

// Utils for RCSwitch data

/**
 * Convert binary to tristate.
 * See https://github.com/sui77/rc-switch/blob/master/examples/ReceiveDemo_Advanced/output.ino
 */
static const char* bin2tristate(const char* bin) {
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

/**
 * Convert decimal to binary with zero fill.
 * See https://github.com/sui77/rc-switch/blob/master/examples/ReceiveDemo_Advanced/output.ino
 */
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
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
