/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#include "CLI.h"
#include "Utils.h"

// Init mode and type
Mode CLI::currentMode = NONE_MODE;
Type CLI::currentType = NONE_TYPE;

void CLI::printHeader () {
  Serial.println(F("\r\n========================================"));
  Serial.println(F("          ESP32 RF433 SNIFFER           "));
  Serial.println(F("           by Herve Perchec             "));
  Serial.println(F("========================================"));
}

void CLI::printMenu () {
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
      printSendMenu();
    }
  }
}

void CLI::printMainMenu () {
  Serial.println(F("--------------SELECT-MODE---------------"));
  Serial.println(F("Commands (case insensitive):"));
  Serial.println(F("  R     : Receiver mode (listening)"));
  Serial.println(F("  T     : Transmitter mode (emitting)"));
  Serial.println(F("  ?     : Show this help"));
  Serial.println(F("----------------------------------------"));
}

void CLI::printTypeMenu () {
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

void CLI::printSendMenu () {
  if (currentType == OLD_STYLE) {
    // ...
  } else {
    Serial.println(F("-----------TRANSMIT-COMMAND-------------"));
    Serial.println(F("<id> <period> <group> <state> <unit> [<dimLevel>]"));
    Serial.println(F("Example: 123456789 260 0 1 1"));
    Serial.println();
    Serial.println(F("<id>           :     Address (integer)"));
    Serial.println(F("<period>       :     Default period (260 microseconds) seems good"));
    Serial.println(F("<group>        :     Group bit (0/1)"));
    Serial.println(F("<state>        :     0: off, 1: on, 2: dim"));
    Serial.println(F("<unit>         :     Unit (0..15)"));
    Serial.println(F("[<dimLevel>]   :     [Optional] The dim level (0..255)"));
    Serial.println();
    Serial.println(F("  Q / QUIT  : Back to previous menu"));
    Serial.println(F("  ?         : Show this help"));
    Serial.println(F("----------------------------------------"));
  }
}

void CLI::printPromptPrefix () {
  Serial.print(F("> "));
}

void CLI::handleSerialCommands (String input) {
  // Stop/quit command
  if (handleQuitCommand(input)) {
    return;
  } else if (handleHelpCommand(input)) { // help command
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
      handleSendCommand(input);
    }
  }

  printPromptPrefix();
}

boolean CLI::handleQuitCommand (String input) {
  // Stop/quit command
  if (input == "Q" || input == "QUIT") {
    onQuit();
    return true;
  } else {
    return false;
  }
}

boolean CLI::handleHelpCommand (String input) {
  // Help command
  if (input == "?") {
    onHelp();
    return true;
  } else {
    return false;
  }
}

void CLI::handleModeCommand (String input) {
  if (input == "R") {
    currentMode = RECEIVE_MODE;
    onModeChosen();
  } else if (input == "T") {
    currentMode = TRANSMIT_MODE;
    onModeChosen();
  } else {
    Serial.print(F("ERROR: Unknown mode: ")); Serial.println(input);
  }
  printMenu();
}

void CLI::handleTypeCommand (String input) {
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
    onTypeChosen();
  }

  printMenu();
}

void CLI::handleSendCommand (String input) {
  onSend(input);
}
