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
 * ** web link to doxygen (all Examples)
 * ** web link to ClearCore Manual (all Examples)  <<FUTURE links to Getting started webpage/ ClearCore videos>>
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
