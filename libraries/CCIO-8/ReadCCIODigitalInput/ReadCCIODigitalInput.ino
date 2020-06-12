/*
 * Title: ReadCCIODigitalInput
 *
 * Objective:
 *    This example demonstrates how to initialize a CCIO-8 Expansion Board and
 *    read from one of its inputs.
 *
 * Description:
 *    This example sets up COM-0 to control a CCIO-8 Expansion Board then reads
 *    the state of an input on the CCIO-8's connector 0. During operation, the
 *    state of the input is printed to the USB serial port.
 *
 * Requirements:
 * ** A CCIO-8 Expansion Board powered and connected to COM-0.
 * ** An digital input device such as a switch connected to the CCIO-8's 
 *    connector 0.
 *
 * Links:
 * ** web link to doxygen (all Examples)
 * ** web link to ClearCore Manual (all Examples)  <<FUTURE links to Getting started webpage/ ClearCore videos>>
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"


// Specify which ClearCore serial COM port is connected to the COM IN port
// of the CCIO-8 board: ConnectorCOM0 or ConnectorCOM1.
#define CcioPort ConnectorCOM0

// Select the baud rate to match the target serial device.
#define baudRate 9600

void setup() {
    // Put your setup code here, it will run once:

    // Set up serial communication to display CCIO-8 state.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Set up the CCIO-8 COM port.
    CcioPort.Mode(Connector::CCIO);
    CcioPort.PortOpen();

    // Make sure the input connector is in input mode (the default for all
    // CCIO-8 pins).
    pinMode(CLEARCORE_PIN_CCIOA0, INPUT);
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Read the state of the input connector
    PinStatus state = digitalRead(CLEARCORE_PIN_CCIOA0);

    // Display the state of the input connector.
    Serial.print("CCIOA0 Input state: ");
    if (state) {
        Serial.println("ON");
    }
    else {
        Serial.println("OFF");
    }

    // Wait a second then repeat...
    delay(1000);
}
