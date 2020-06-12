/*
 * Title: ReadDigitalInputRiseFall
 *
 * Objective:
 *    This example demonstrates how to read the transition state (risen or
 *    fallen since last checked) of a ClearCore digital input.
 *
 * Description:
 *    This example repeatedly reads the transition state of a defined digital
 *    input. Information on how the input state has transitioned is printed
 *    to the USB serial port every 2 seconds.
 *
 * Requirements:
 * ** An input device, such as a switch or sensor, connected to DI-6.
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

// Specify which input connector to use.
// ConnectorIO0 through ConnectorA12 all have digital input capability.
#define InputConnector ConnectorDI6

// Declares two boolean variables used to hold information on whether the input
// has risen or fallen
bool risen, fallen;

// Select the baud rate to match the target serial device
#define baudRate 9600

void setup() {
    // Put your setup code here, it will run once:

    // Set up serial communication at a baud rate of 9600 bps then wait up to
    // 5 seconds for a port to open.
    // Serial communication is not required for this example to run, however the
    // example will appear to do nothing without serial output.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Check whether the digital input has risen, fallen, or is unchanged
    // since the last time we checked.
    risen = InputConnector.InputRisen();
    fallen = InputConnector.InputFallen();

    Serial.print("DI-6 Transitions: ");

    if (risen && fallen) {
        Serial.println("RISEN and FALLEN");
    }
    else if (risen) {
        Serial.println("RISEN");
    }
    else if (fallen) {
        Serial.println("FALLEN");
    }
    else {
        Serial.println("NO CHANGE");
    }

    // Wait a couple seconds then repeat...
    delay(2000);
}
