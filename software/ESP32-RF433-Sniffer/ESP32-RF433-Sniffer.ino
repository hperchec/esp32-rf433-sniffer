/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#include <RCSwitch.h>
#include <NewRemoteReceiver.h>
#include "ESP32-RF433-Sniffer.h"

// Create a RCSwitch instance
RCSwitch rcSwitch = RCSwitch();
// Utils for RCSwitch data, see below
static const char* bin2tristate(const char* bin);
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

// Create instance protocol 2 data
Prot2Data p2Data;

// 0: NONE
// 1: Protocol 1
// 2: Protocol 2
int currentProtocol = 0;

/**
 * Setup function
 * See https://docs.arduino.cc/language-reference/en/structure/sketch/setup/
 */
void setup () {
  // Set baudrate for serial connection
  Serial.begin(115200);
  // Reduce serial timeout for Serial.readStringUntil method (default is 1000)
  Serial.setTimeout(100);
  // Arbitrary delay for PuTTY like tools
  delay(1000);

  printHeader();
  printHelp();
  printPromptPrefix();
}

/**
 * Loop function
 * See https://docs.arduino.cc/language-reference/en/structure/sketch/loop/
 */
void loop () {
  // Handle user command
  handleSerialCommands();

  // Protocol 1 data available
  if (currentProtocol == 1 && rcSwitch.available()) {
    // Show data
    logProt1Data();

    printPromptPrefix();
    rcSwitch.resetAvailable();
  }

  // Protocol 2 data available
  if (currentProtocol == 2 && p2Data.available) {
    // Show data
    logProt2Data();

    printPromptPrefix();
    // Always clear data
    p2Data.clear();
  }
}

/**
 * Print the header
 */
void printHeader () {
  Serial.println(F("\r\n========================================"));
  Serial.println(F("          ESP32 RF433 SNIFFER           "));
  Serial.println(F("           by Herve Perchec             "));
  Serial.println(F("========================================"));
}

/**
 * Print the help
 */
void printHelp () {
  Serial.println(F("----------------------------------------"));
  Serial.println(F("COMMANDS (case insensitive):"));
  Serial.println(F("  1     : Protocol 1 (RCSwitch)"));
  Serial.println(F("  2     : Protocol 2 (NewRemoteSwitch)"));
  Serial.println(F("  stop  : Stop listening"));
  Serial.println(F("  ?     : Show this help"));
  Serial.println(F("----------------------------------------"));
}

/**
 * Print prompt prefix "[Protocol X] >"
 */
void printPromptPrefix () {
  Serial.print(F("["));
  
  if (currentProtocol != 0) {
    Serial.print(F("Protocol "));
    Serial.print(currentProtocol);
  } else {
    Serial.print(F("NONE"));
  }
  
  Serial.print(F("] > "));
}

/**
 * Stop all "listeners"
 */
void stopAll () {
  if (currentProtocol == 1) {
    rcSwitch.disableReceive();
  } else if (currentProtocol == 2) {
    NewRemoteReceiver::deinit();
  }
  currentProtocol = 0;
}

/**
 * Handle serial commands sent by user
 */
void handleSerialCommands () {
  if (Serial.available() > 0) {
    // 1. Lire toute la ligne jusqu'au caractère de fin de ligne
    String input = Serial.readStringUntil('\n');
    
    // 2. Nettoyer la commande (enlever les \r, les espaces et mettre en majuscules)
    input.trim();
    input.toUpperCase();

    // Ignore empty lines
    if (input.length() == 0) {
      printPromptPrefix();
      return;
    }

    if (input == "1") {
      stopAll();
      rcSwitch.enableReceive(digitalPinToInterrupt(RX_PIN));
      currentProtocol = 1;
    } else if (input == "2") {
      stopAll();
      NewRemoteReceiver::init(RX_PIN, 2, nrsInitCallback);
      currentProtocol = 2;
    } else if (input == "STOP") {
      stopAll();
      Serial.println(F("Listening stopped"));
    } else if (input == "?") {
      printHelp();
    } else {
      Serial.println(F("Unknown command. Enter '?' to show help."));
    }
    printPromptPrefix();
  }
}

/**
 * The callback function to pass to NewRemoteSwitch::init method.
 * Called every time a signal is decoded by NewRemoteSwitch library
 */
void nrsInitCallback (unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType, boolean dimLevelPresent, byte dimLevel) {
  p2Data.period = period;
  p2Data.address = address;
  p2Data.groupBit = groupBit;
  p2Data.unit = unit;
  p2Data.state = switchType;
  if (dimLevelPresent){
    p2Data.dimLevel = dimLevel;
  }
  p2Data.available = true;
}

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
}

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
}

/**
 * Log the available data for protocol 1
 */
void logProt1Data () {
  const unsigned long decimal = rcSwitch.getReceivedValue();
  const unsigned int length = rcSwitch.getReceivedBitlength();
  const unsigned int delay = rcSwitch.getReceivedDelay();
  const unsigned int* raw = rcSwitch.getReceivedRawdata();
  const unsigned int protocol = rcSwitch.getReceivedProtocol();

  const char* b = dec2binWzerofill(decimal, length);

  Serial.println(F("\r\n------------ DECODED SIGNAL ------------"));
  Serial.print(F("Decimal     : ")); Serial.print(decimal); Serial.print(F(" (")); Serial.print(length); Serial.println(F("Bit)"));
  Serial.print(F("Binary      : ")); Serial.println(b);
  Serial.print(F("Tri-State   : ")); Serial.println(bin2tristate(b));
  Serial.print(F("PulseLength : ")); Serial.print(delay); Serial.println(F(" microseconds"));
  Serial.print(F("Protocol    : ")); Serial.println(protocol);
  Serial.print(F("Raw data    : "));
  for (unsigned int i=0; i<= length*2; i++) {
    Serial.print(raw[i]);
    Serial.print(",");
  }
  Serial.println();
  Serial.println(F("----------------------------------------"));
}

/**
 * Log the available data for protocol 2
 */
void logProt2Data () {
  Serial.println(F("\r\n------------ DECODED SIGNAL ------------"));
  Serial.print(F("ID       : ")); Serial.println(p2Data.address);
  Serial.print(F("Period   : ")); Serial.print(p2Data.period); Serial.println(F(" microseconds"));
  Serial.print(F("Unit     : ")); Serial.println(p2Data.unit);
  Serial.print(F("GroupBit : ")); Serial.println(p2Data.groupBit);
  Serial.print(F("State    : ")); Serial.println(p2Data.state ? "ON" : "OFF");
  Serial.println(F("----------------------------------------"));
}