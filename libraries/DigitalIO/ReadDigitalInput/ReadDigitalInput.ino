/*
 * Title: ReadDigitalInput
 *
 * Objective:
 *    This example demonstrates how to read the state of a ClearCore digital
 *    input.
 *
 * Description:
 *    This example repeatedly reads the state of a defined digital input. During
 *    operation, the state of the input is printed to the USB serial port.
 *
 * Requirements:
 * ** A digital input device, such as a switch or sensor, connected to DI-6
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

// Specify which input pin to read from.
// IO-0 through A-12 are all available as digital inputs.
#define inputPin DI6

// The current state of the input pin
PinStatus state;

// Select the baud rate to match the target serial device.
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

    // Read the state of the input connector.
    state = digitalRead(inputPin);

    // Display the state of the input connector.
    Serial.print("DI-6 Input state: ");
    if (state) {
        Serial.println("ON");
    }
    else {
        Serial.println("OFF");
    }

    // Wait a second then repeat...
    delay(1000);
}
