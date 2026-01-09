/**
 * ESP32-RF433-Sniffer
 *
 * @author Hervé Perchec (https://github.com/hperchec)
 *
 * Copyright (c) 2025 Hervé Perchec. All right reserved.
 * License: GNU AFFERO GENERAL PUBLIC LICENSE (see LICENSE file)
 */

#ifndef CLI_H
#define CLI_H

#include <Arduino.h>
#include "Data.h"

// Mode
enum Mode { NONE_MODE, RECEIVE_MODE, TRANSMIT_MODE };
// Type
enum Type { NONE_TYPE, OLD_STYLE, NEW_STYLE };

class CLI {
  public:
    // Static variables
    static Mode currentMode;
    static Type currentType;

    /**
     * Print the header
     */
    static void printHeader ();
    /**
     * Print the corresponding menu based on currentMode and currentType
     */
    static void printMenu ();
    /**
     * Print the main menu (choose mode)
     */
    static void printMainMenu ();
    /**
     * Print the type menu (choose type)
     */
    static void printTypeMenu ();
    /**
     * Print the send menu (send command)
     */
    static void printSendMenu ();
    /**
     * Print prompt prefix "> "
     */
    static void printPromptPrefix ();

    /**
     * Handle serial commands sent by user
     *
     * @param input The user prompt
     */
    static void handleSerialCommands (String input);
    /**
     * Handle any "quit" command
     *
     * @param input The user prompt
     */
    static boolean handleQuitCommand (String input);
    /**
     * Handle any "help" command
     *
     * @param input The user prompt
     */
    static boolean handleHelpCommand (String input);
    /**
     * Handle serial commands for main menu
     *
     * @param input The user prompt
     */
    static void handleModeCommand (String input);
    /**
     * Handle serial commands for type menu
     *
     * @param input The user prompt
     */
    static void handleTypeCommand (String input);
    /**
     * Handle serial commands to send signals
     *
     * @param input The user prompt
     */
    static void handleSendCommand (String input);

    /**
     * Do something when "help" command is readen
     */
    static void onHelp ();
    /**
     * Do something when "quit" command is readen
     */
    static void onQuit ();
    /**
     * Do something when mode is chosen
     */
    static void onModeChosen ();
    /**
     * Do something when type is chosen
     */
    static void onTypeChosen ();
    /**
     * Do something when "send" command is readen
     *
     * @param input The user prompt
     */
    static void onSend (String input);
};

#endif