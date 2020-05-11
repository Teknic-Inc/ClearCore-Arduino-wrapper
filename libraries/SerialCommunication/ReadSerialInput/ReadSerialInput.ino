/*
 * Title: ReadSerialInput
 *
 * Objective:
 *    This example demonstrates how to read and display incoming data from a
 *    serial port.
 *
 * Description:
 *    This example will read one byte per second from the serial input buffer.
 *    During operation, if a byte has been received, it will be printed to the
 *    USB serial port as a character.
 *
 * Requirements:
 * ** A serial input source connected to Serial0 (COM-0 on the ClearCore)
 *
 * Links:
 * ** web link to doxygen (all Examples)
 * ** web link to ClearCore Manual (all Examples)  <<FUTURE links to Getting started webpage/ ClearCore videos>>
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

// Select the baud rate to match the target device.
#define baudRateSerialPort 115200
#define baudRateInputPort  115200

// When using COM ports, is the device TTL or RS232?
#define isTtlInputPort  false

// Container for the byte to be read-in
int input;

void setup() {
    // Put your setup code here, it will run once:

    // Set up serial communication to print out the serial input.
    Serial.begin(baudRateSerialPort);
    while (!Serial) {
        continue;
    }

    // Set up serial communication to send serial input over.
    Serial0.begin(baudRateInputPort);
    Serial0.ttl(isTtlInputPort);
    while (!Serial0) {
        continue;
    }
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Read the input.
    input = Serial0.read();

    // If there was a valid byte read-in, print it.
    if (input != -1) {
        // Display the input character received.
        Serial.print("Received: ");
        Serial.println((char)input);
    }
    else {
        Serial.println("No data received...");
    }

    // Wait a second then repeat...
    delay(1000);
}