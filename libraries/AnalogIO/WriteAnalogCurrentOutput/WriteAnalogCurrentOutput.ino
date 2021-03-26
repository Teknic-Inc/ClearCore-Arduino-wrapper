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
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 *
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

    // Ramp the current output of IO-0 up to 20 mA. If using an operating
    // range of 4-20 mA, change the lower bounds of the loops below to 410
    // instead of 0.
    for (uint16_t value = 0; value < 2047; value++) {
        // ClearCore's analog current output has 11-bit resolution, so we write
        // values of 0 to 2047 (corresponding to 0-20 mA) or 410 to 2047
        // (corresponding to 4-20 mA).
        analogWrite(IO0, value, CURRENT);
        delay(2);
    }

    // Ramp the current output of IO-0 back down.
    for (uint16_t value = 2047; value > 0; value--) {
        analogWrite(IO0, value, CURRENT);
        delay(2);
    }
}
