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
#include <NewRemoteTransmitter.h>
#include "Config.h"
#include "Utils.h"
#include "Data.h"
#include "CLI.h"
#include "Led.h"

// Create a RCSwitch instance
RCSwitch rcSwitch = RCSwitch();

// Create instance type 1 data
Type1Data t1Data;
// Create instance type 2 data
Type2Data t2Data;
// Flag for type 2 data availability
bool type2DataAvailable = false;

// Init RGB led
Led rgbLed = Led(new RGBCC(RGB_LED_RED_PIN, RGB_LED_GREEN_PIN, RGB_LED_BLUE_PIN));

// Init transmitter repeater
Repeater transmitRepeater = Repeater();

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

  CLI::printHeader();
  CLI::printMenu();
  CLI::printPromptPrefix();

  // Init led state
  refreshLedState();
}

/**
 * Loop function
 * See https://docs.arduino.cc/language-reference/en/structure/sketch/loop/
 */
void loop () {
  // Always update led and repeater
  rgbLed.update();
  transmitRepeater.update();

  // Try to read serial command
  String input = readSerialCommand();

  // If transmitter is currently sending signals
  if (transmitRepeater.isRunning()) {
    if (input.length() > 0 && CLI::handleQuitCommand(input)) {
      transmitRepeater.stop();
    }
  } else {
    // Handle user command
    if (input.length() > 0) {
      CLI::handleSerialCommands(input);
    }

    // Type 1 data available
    if (CLI::currentType == OLD_STYLE && rcSwitch.available()) {
      rgbLed.receivingState();
      rgbLed.setTimeout([]() {
        refreshLedState();
      }, 400 /* duration of receivingState */);
      // Get data
      t1Data = createData(
        rcSwitch.getReceivedValue(), // decimal
        rcSwitch.getReceivedProtocol(), // protocol
        rcSwitch.getReceivedDelay(), // delay
        rcSwitch.getReceivedBitlength(), // length
        rcSwitch.getReceivedRawdata() // raw
      );
      // Show data
      Serial.println(F("\r\n------------ DECODED SIGNAL ------------"));
      logData(t1Data);
      Serial.println(F("----------------------------------------"));

      rcSwitch.resetAvailable();
      // Always clear data
      t1Data.clear();

      CLI::printPromptPrefix();
    }

    // Type 2 data available
    if (CLI::currentType == NEW_STYLE && type2DataAvailable) {
      rgbLed.receivingState();
      rgbLed.setTimeout([]() {
        refreshLedState();
      }, 400 /* duration of receivingState */);
      // Show data
      Serial.println(F("\r\n------------ DECODED SIGNAL ------------"));
      logData(t2Data);
      Serial.println(F("----------------------------------------"));

      type2DataAvailable = false;
      // Always clear data
      t2Data.clear();

      CLI::printPromptPrefix();
    }
  }

}

/**
 * Apply the specific led state based on current mode
 */
void refreshLedState () {
  if (CLI::currentMode == NONE_MODE) {
    rgbLed.mainMenuState();
  } else if (CLI::currentMode == RECEIVE_MODE) {
    if (CLI::currentType == NONE_TYPE) {
      rgbLed.receiveMenuState();
    } else {
      rgbLed.readyToReceiveState();
    }
  } else {
    if (CLI::currentType == NONE_TYPE) {
      rgbLed.transmitMenuState();
    } else {
      rgbLed.readyToTransmitState();
    }
  }
  rgbLed.update();
}

void CLI::onHelp () {
  printMenu();
  printPromptPrefix();
}

void CLI::onQuit () {
  stopAndBack();
  printMenu();
  printPromptPrefix();
}

void CLI::onModeChosen () {
  refreshLedState();
}

void CLI::onTypeChosen () {
  if (currentMode == RECEIVE_MODE) {
    startReceiveMode();
  } else {
    startTransmitMode();
  }
}

void CLI::onSend (String input) {
  if (currentType == OLD_STYLE) {
    Type1Data data;
    try {
      data = parseType1SendCommand(input);
    } catch (String error) {
      Serial.println(F("ERROR: unable to parse the given command. Type ? to show help."));
    }
    Serial.println(F("----------------------------------------"));
    logData(data);
    Serial.println(F("----------------------------------------"));

    Serial.print("Sending");
    transmitRepeater.start([data](unsigned long count) {
      Serial.print(".");
      rgbLed.sendingState();
      rgbLed.setTimeout([data]() {
        refreshLedState();
        unsigned long timeStart = millis();
        // This function is "blocking": it uses delay() under the hood
        sendType1Data(data); // ≃ 370 ms but consider 1400 ms to match same interval as Type2Data sending
      }, 400 /* duration of sendingState */);
    }, 10, 2000); // Every 1400 + 400 + 200 (safety margin) ms
    transmitRepeater.onStop([](unsigned long count) {
      Serial.print("Stopped. Signal has been sent "); Serial.print(count); Serial.println(" times");
      refreshLedState();
      CLI::printPromptPrefix();
    });
  } else {
    Type2Data data;
    try {
      data = parseType2SendCommand(input);
    } catch (String error) {
      Serial.println(F("ERROR: unable to parse the given command. Type ? to show help."));
    }
    Serial.println(F("----------------------------------------"));
    logData(data);
    Serial.println(F("----------------------------------------"));

    Serial.print("Sending");
    transmitRepeater.start([data](unsigned long count) {
      Serial.print(".");
      rgbLed.sendingState();
      rgbLed.setTimeout([data]() {
        refreshLedState();
        // This function is "blocking": it uses delay() under the hood
        sendType2Data(data); // ≃ 1400 ms
      }, 400 /* duration of sendingState */);
    }, 10, 2000); // Every 1400 + 400 + 200 (safety margin) ms
    transmitRepeater.onStop([](unsigned long count) {
      Serial.print("Stopped. Signal has been sent "); Serial.print(count); Serial.println(" times");
      refreshLedState();
      CLI::printPromptPrefix();
    });
  }
}

/**
 * Start the receiver based on currentType
 */
void startReceiveMode () {
  if (CLI::currentType == OLD_STYLE) {
    rcSwitch.enableReceive(digitalPinToInterrupt(RX_PIN));
  } else {
    NewRemoteReceiver::init(RX_PIN, 2, nrsInitCallback);
  }
  refreshLedState();
  Serial.println(F("Listening..."));
}

/**
 * Start the transmitter based on currentType
 */
void startTransmitMode () {
  if (CLI::currentType == OLD_STYLE) {
    rcSwitch.enableTransmit(digitalPinToInterrupt(TX_PIN));
  } else {
    // No need to setup NewRemoteSwitch...
    // See sendType2Data function below
  }
  refreshLedState();
  Serial.println(F("Waiting for send command..."));
}

/**
 * Stop current receiver or transmitter and back to previous menu
 */
void stopAndBack () {
  switch (CLI::currentMode) {
    case NONE_MODE:
      // Nothing...
      break;
    case RECEIVE_MODE:
      // No type selected? back to main menu
      if (CLI::currentType == NONE_TYPE) {
        CLI::currentMode = NONE_MODE;
      } else {
        if (CLI::currentType == OLD_STYLE) {
          rcSwitch.disableReceive();
        } else if (CLI::currentType == NEW_STYLE) {
          NewRemoteReceiver::deinit();
        }
        Serial.println(F("Receiver stopped"));
        CLI::currentType = NONE_TYPE;
      }
      refreshLedState();
      break;
    case TRANSMIT_MODE:
      // No type selected? back to main menu
      if (CLI::currentType == NONE_TYPE) {
        CLI::currentMode = NONE_MODE;
      } else {
        // "Old" style? disable transmit
        if (CLI::currentType == OLD_STYLE) {
          rcSwitch.disableTransmit();
        }
        // Reset current type only if transmitRepeater is not running
        if (!transmitRepeater.isRunning()) {
          CLI::currentType = NONE_TYPE;
        }
      }
      refreshLedState();
      break;
  }
}

/**
 * The callback function to pass to NewRemoteSwitch::init method.
 * Called every time a signal is decoded by NewRemoteSwitch library
 */
void nrsInitCallback (unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType, boolean dimLevelPresent, byte dimLevel) {
  t2Data = createData(
    period,
    address,
    groupBit,
    unit,
    switchType,
    dimLevelPresent,
    dimLevel
  );
  type2DataAvailable = true;
}

/**
 * Create and return a Type1Data object
 */
Type1Data createData (unsigned long decimal, unsigned int protocol, unsigned int delay, unsigned int length, unsigned int* raw) {
  Type1Data data;
  data.decimal = decimal;
  data.protocol = protocol;
  data.delay = delay;
  data.length = length;
  data.raw = raw;
  data.bin = dec2binWzerofill(decimal, length);
  data.triState = bin2tristate(data.bin);
  return data;
}

/**
 * Create and return a Type2Data object
 */
Type2Data createData (unsigned int period, unsigned long address, unsigned long groupBit, unsigned long unit, unsigned long switchType, boolean dimLevelPresent, byte dimLevel) {
  Type2Data data;
  data.period = period;
  data.address = address;
  data.groupBit = groupBit;
  data.unit = unit;
  switch (switchType) {
    case Type2Data::off:
      data.switchType = Type2Data::off;
      break;
    case Type2Data::on:
      data.switchType = Type2Data::on;
      break;
    case Type2Data::dim:
      data.switchType = Type2Data::dim;
      break;
  }
  data.dimLevelPresent = dimLevelPresent;
  if (dimLevelPresent) {
    data.dimLevel = dimLevel;
  } else {
    data.dimLevel = 0;
  }
  return data;
}

/**
 * Command syntax: <decimal> <protocol> <delay> <length>
 * Example: 5592332 1 350 24
 */
Type1Data parseType1SendCommand (String input) {
  String tokens[4] = { "", "", "", "" };
  int i = 0;
  while (input.length() > 0 && i < 4) {
    int index = input.indexOf(' ');
    if (index == -1) {
      tokens[i++] = input;
      break;
    } else {
      tokens[i++] = input.substring(0, index);
      input = input.substring(index + 1);
    }
  }

  Type1Data data = createData(
    strtoul(tokens[0].c_str(), NULL, 10), // decimal
    tokens[1].toInt(), // protocol
    tokens[2].toInt(), // delay
    tokens[3].toInt(), // length
    nullptr // raw
  );

  return data;
}

/**
 * Send (type 1) data with rc-switch
 */
void sendType1Data (Type1Data data) {
  // Configure transmitter
  rcSwitch.setProtocol(data.protocol);
  rcSwitch.setPulseLength(data.delay);
  // Send signals
  rcSwitch.send(data.decimal, data.length);
}

/**
 * <address> <period> <group> <unit> <state> [<dimLevel>]
 */
Type2Data parseType2SendCommand (String input) {
  String tokens[6] = { "", "", "", "", "", "" };
  int i = 0;
  // Split the input into substrings
  while (input.length() > 0) {
    int index = input.indexOf(' ');
    if (index == -1) { // No space found
      tokens[i++] = input;
      break;
    } else {
      tokens[i++] = input.substring(0, index);
      input = input.substring(index+1);
    }
  }

  bool dimLevelPresent = (tokens[5] && tokens[5].length() > 0);

  Type2Data data = createData(
    tokens[1].toInt(), // period
    tokens[0].toInt(), // address
    tokens[2].toInt(), // groupbit
    tokens[3].toInt(), // unit
    tokens[4].toInt(), // switchType
    dimLevelPresent,
    dimLevelPresent ? tokens[5].toInt() : 0 // dimLevel
  );

  return data;
}

/**
 * Send (type 2) data with NewRemoteTransmitter
 */
void sendType2Data (Type2Data data) {
  // Create a new transmitter with the received address and period, use a digital pin as output pin
  NewRemoteTransmitter transmitter(data.address, TX_PIN, data.period);

  if (data.switchType == Type2Data::dim ||
    (data.switchType == Type2Data::on && data.dimLevelPresent)) {
    // Dimmer signal received
    if (data.groupBit) {
      transmitter.sendGroupDim(data.dimLevel);
    } else {
      transmitter.sendDim(data.unit, data.dimLevel);
    }
  } else {
    // On/Off signal
    bool isOn = data.switchType == Type2Data::on;
    if (data.groupBit) {
      // Send to the group
      transmitter.sendGroup(isOn);
    } else {
      // Send to a single unit
      transmitter.sendUnit(data.unit, isOn);
    }
  }
}
