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

// Create instance type 2 data
Type2Data p2Data;

// Init mode and type
Mode currentMode = NONE_MODE;
Type currentType = NONE_TYPE;

/**
 * Setup function
 * See https://docs.arduino.cc/language-reference/en/structure/sketch/setup/
 */
void setup () {
  // Set baudrate for serial connection
  Serial.begin(SERIAL_BAUDRATE);
  // Reduce serial timeout for Serial.readStringUntil method (default is 1000)
  Serial.setTimeout(100);
  // Arbitrary delay for PuTTY like tools
  delay(1000);

  printHeader();
  printMenu();
  printPromptPrefix();
}

/**
 * Loop function
 * See https://docs.arduino.cc/language-reference/en/structure/sketch/loop/
 */
void loop () {
  // Handle user command
  handleSerialCommands();

  // Type 1 data available
  if (currentType == OLD_STYLE && rcSwitch.available()) {
    // Show data
    logType1Data();

    printPromptPrefix();
    rcSwitch.resetAvailable();
  }

  // Type 2 data available
  if (currentType == NEW_STYLE && p2Data.available) {
    // Show data
    logType2Data();

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
 * Print the corresponding menu based on currentMode and currentType
 */
void printMenu () {
  // No type selected
  if (currentType == NONE_TYPE) {
    // No mode selected
    if (currentMode == NONE_MODE) {
      printMainMenu();
    } else {
      printTypeMenu();
    }
  } else {
    if (currentMode == RECEIVE_MODE) {
      // ...
    } else {
      // ...
    }
  }
}

/**
 * Print the main menu (choose mode)
 */
void printMainMenu () {
  Serial.println(F("--------------SELECT-MODE---------------"));
  Serial.println(F("Commands (case insensitive):"));
  Serial.println(F("  R     : Receiver mode (listening)"));
  Serial.println(F("  T     : Transmitter mode (emitting)"));
  Serial.println(F("  ?     : Show this help"));
  Serial.println(F("----------------------------------------"));
}

/**
 * Print the type menu (choose type)
 */
void printTypeMenu () {
  if (currentMode == RECEIVE_MODE) {
    Serial.println(F("--------------RECEIVE-TYPE--------------"));
  } else {
    Serial.println(F("-------------TRANSMIT-TYPE--------------"));
  }
  Serial.println(F("Commands (case insensitive):"));
  Serial.println(F("  1         : Type 1 (RCSwitch)"));
  Serial.println(F("  2         : Type 2 (NewRemoteSwitch)"));
  Serial.println(F("  Q / QUIT  : Back to previous menu"));
  Serial.println(F("  ?         : Show this help"));
  Serial.println(F("----------------------------------------"));
}

/**
 * Print prompt prefix "> "
 */
void printPromptPrefix () {
  Serial.print(F("> "));
}

/**
 * Stop current receiver or transmitter and back to previous menu
 */
void stopAndBack () {
  switch (currentMode) {
    case NONE_MODE:
      // Nothing...
      break;
    case RECEIVE_MODE:
      // No type selected? back to main menu
      if (currentType == NONE_TYPE) {
        currentMode = NONE_MODE;
      } else {
        if (currentType == OLD_STYLE) {
          rcSwitch.disableReceive();
        } else if (currentType == NEW_STYLE) {
          NewRemoteReceiver::deinit();
        }
        Serial.println(F("Receiver stopped"));
        currentType = NONE_TYPE;
      }
      break;
    case TRANSMIT_MODE:
      // No type selected? back to main menu
      if (currentType == NONE_TYPE) {
        currentMode = NONE_MODE;
      } else {
        if (currentType == OLD_STYLE) {
          // ... stop transmitter
        } else if (currentType == NEW_STYLE) {
          // ... stop transmitter
        }
        Serial.println(F("Transmitter stopped"));
        currentType = NONE_TYPE;
      }
      break;
  }
}

/**
 * Handle serial commands sent by user
 */
void handleSerialCommands () {
  if (Serial.available() > 0) {
    // Read entire line
    String input = Serial.readStringUntil('\n');

    // Clean entry and force uppercase
    input.trim();
    input.toUpperCase();

    // Ignore empty lines
    if (input.length() == 0) {
      printPromptPrefix();
      return;
    }

    // Stop/quit command
    if (input == "Q" || input == "QUIT") {
      stopAndBack();
      printMenu();
      printPromptPrefix();
      return;
    } else if (input == "?") { // Help command
      printMenu();
      printPromptPrefix();
      return;
    }

    // No type selected
    if (currentType == NONE_TYPE) {
      // No mode selected
      if (currentMode == NONE_MODE) {
        handleModeCommand(input);
      } else {
        handleTypeCommand(input);
      }
    } else {
      if (currentMode == RECEIVE_MODE) {
        // ...
      } else {
        // ...
      }
    }

    printPromptPrefix();
  }
}

/**
 * Handle serial commands for main menu
 */
void handleModeCommand (String input) {
  if (input == "R") {
    currentMode = RECEIVE_MODE;
  } else if (input == "T") {
    currentMode = TRANSMIT_MODE;
  } else {
    Serial.print(F("ERROR: Unknown mode: ")); Serial.println(input);
  }
  printMenu();
}

/**
 * Handle serial commands for type menu
 */
void handleTypeCommand (String input) {
  bool unknownCmd = false;
  if (input == "1") {
    currentType = OLD_STYLE;
  } else if (input == "2") {
    currentType = NEW_STYLE;
  } else {
    unknownCmd = true;
  }

  if (unknownCmd) {
    Serial.print(F("ERROR: Unknown command: ")); Serial.println(input);
  } else {
    if (currentMode == RECEIVE_MODE) {
      startReceiveMode();
    } else {
      // ... startTransmitMode();
    }
  }

  printMenu();
}

/**
 * Start the receiver based on currentType
 */
void startReceiveMode () {
  if (currentType == OLD_STYLE) {
    rcSwitch.enableReceive(digitalPinToInterrupt(RX_PIN));
  } else {
    NewRemoteReceiver::init(RX_PIN, 2, nrsInitCallback);
  }
  Serial.println(F("Listening..."));
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
 * Log the available data for type 1
 */
void logType1Data () {
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
 * Log the available data for type 2
 */
void logType2Data () {
  Serial.println(F("\r\n------------ DECODED SIGNAL ------------"));
  Serial.print(F("ID       : ")); Serial.println(p2Data.address);
  Serial.print(F("Period   : ")); Serial.print(p2Data.period); Serial.println(F(" microseconds"));
  Serial.print(F("Unit     : ")); Serial.println(p2Data.unit);
  Serial.print(F("GroupBit : ")); Serial.println(p2Data.groupBit);
  Serial.print(F("State    : ")); Serial.println(p2Data.state ? "ON" : "OFF");
  Serial.println(F("----------------------------------------"));
}