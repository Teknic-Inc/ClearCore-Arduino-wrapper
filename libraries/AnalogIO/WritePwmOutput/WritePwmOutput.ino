/*
 * Title: WritePwmOutput
 *
 * Objective:
 *    This example demonstrates how to write a digital PWM signal to a ClearCore
 *    digital output.
 *
 * Description:
 *    This example sets the defined pin as an output then writes a series of
 *    PWM signals with varying duty cycles to the output.
 *
 * Requirements:
 * ** Connect a device that takes in a PWM signal to IO-1
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 *
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

// Specify which output pin to write digital PWM to.
// PWM-capable pins: IO-0 through IO-5.
// Note: IO-4 and IO-5 are capable of bi-directional and higher-current PWM
//       output using an H-Bridge. See the WriteHBridgeOutput example.
#define outputPin IO1

void setup() {
    // Put your setup code here, it will run once:

    // Set up the output pin to be an output
    pinMode(outputPin, OUTPUT);
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Write some digital PWM signals to the output connector.
    // Note: analogWrite() is the Arduino wrapper function used to write PWM,
    // despite the fact that it is not a "true" analog signal
    // Valid values range from 0 (0% duty cycle / always off)
    // to 255 (100% duty cycle / always on).

    // Output a low duty cycle for 1 second.
    analogWrite(outputPin, 10);
    delay(1000);

    // Output a medium duty cycle for 1 second.
    analogWrite(outputPin, 120);
    delay(1000);

    // Output a high duty cycle for 1 second.
    analogWrite(outputPin, 230);
    delay(1000);
}
