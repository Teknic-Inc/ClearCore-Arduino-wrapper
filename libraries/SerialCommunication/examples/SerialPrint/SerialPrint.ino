/*
 * Title: SerialPrint
 *
 * Objective:
 *    This example demonstrates how to print text using serial output.
 *
 * Description:
 *    This example will print the basic "Hello World" message to the USB serial
 *    port.
 *
 * Requirements:
 * ** None
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

// ClearCore provides three separate serial interfaces for communications.
// The three options are Serial (USB), Serial0 (COM-0), or Serial1 (COM-1)
#define SerialPort Serial

void setup() {
    // Put your setup code here, it will run once:

    // Set up serial communication to print.
    SerialPort.begin(9600);
    while (!SerialPort) {
        continue;
    }

    // The port is open and ready to talk.
    SerialPort.println("Hello World!");
}

void loop() {
    // This example doesn't have any ongoing tasks to perform.
}
