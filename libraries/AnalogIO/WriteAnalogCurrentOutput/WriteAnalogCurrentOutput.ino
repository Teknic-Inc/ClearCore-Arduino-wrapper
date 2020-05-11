/*
 * Title: WriteAnalogCurrentOutput
 *
 * Objective:
 *    This example demonstrates how to write analog current values to an analog
 *    current output connector.
 *
 * Description:
 *    This example configures pin IO-0 as an analog current output. It outputs
 *    a repeating analog signal, starting at 0mA, increasing to 20mA, and
 *    decreasing back to 0mA.
 *
 * Requirements:
 * ** Connect a device to IO-0 which takes in analog current.
 *
 * Links:
 * ** web link to doxygen (all Examples)
 * ** web link to ClearCore Manual (all Examples)  <<FUTURE links to Getting started webpage/ ClearCore videos>>
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */


void setup() {
    // Put your setup code here, it will run once:

    // Set up connector IO-0 to be an output. We will specify that it outputs
    // current in our analogWrite functions later (as opposed to a digital
    // signal or PWM).
    // Note that only connector IO-0 is capable of analog current output.
    pinMode(IO0, OUTPUT);
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Ramp the current output of IO-0 up to 20mA.
    for (uint16_t value = 0; value < 2047; value++) {
        // ClearCore's analog current output has 11-bit resolution, so we write
        // values of 0 to 2047 (corresponding to 0-20mA).
        analogWrite(IO0, value, CURRENT);
        delay(2);
    }

    // Ramp the current output of IO-0 back down.
    for (uint16_t value = 0; value < 2047; value++) {
        analogWrite(IO0, 2047 - value, CURRENT);
        delay(2);
    }
}