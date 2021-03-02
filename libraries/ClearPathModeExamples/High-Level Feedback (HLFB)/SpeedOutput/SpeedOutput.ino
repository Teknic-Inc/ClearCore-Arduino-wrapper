/*
 * Title: SpeedOutput
 *
 * Objective:
 *    This example demonstrates how to configure and read-in the High-Level
 *    Feedback mode "Speed Output" of a ClearPath motor.
 *
 *    This HLFB mode is available in ClearPath-MC series servos, in every
 *    ClearPath-MC operational mode except Ramp Up/Down to Selected Velocity.
 *
 * Description:
 *    This example reads the state of an attached ClearPath motor's HLFB output
 *    when configured for "Speed Output". During operation, the state of HLFB
 *    and calculated measured speed are written to the USB serial port.
 *
 *    This example does not enable the motor or command motion. Use the
 *    "Override Inputs" feature in MSP to command motion and see changes in the
 *    HLFB measurement.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2.  The connected ClearPath motor must be configured through the MSP software
 *    for an operational mode compatible with Speed Output HLFB mode (see above)
 * 3. The connected ClearPath motor must have its HLFB mode set to "Speed Output"
 *    (select Advanced>>High Level Feedback [Mode]... then choose "Speed Output"
 *    from the dropdown and hit the OK button).
 *    Select a 482 Hz PWM Carrier Frequency in this menu.
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 * ** ClearPath Manual (DC Power): https://www.teknic.com/files/downloads/clearpath_user_manual.pdf
 * ** ClearPath Manual (AC Power): https://www.teknic.com/files/downloads/ac_clearpath-mc-sd_manual.pdf
 *
 *
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"

// Specify which motor to move.
// Options are: ConnectorM0, ConnectorM1, ConnectorM2, or ConnectorM3.
#define motor ConnectorM0

// Select the baud rate to match the target serial device
#define baudRate 9600

void setup() {
    // Put your setup code here, it will run once:

    // Put the motor connector into the correct HLFB mode to read the Speed
    // Output PWM signal and convert it to percent of Max Speed.
    motor.HlfbMode(MotorDriver::HLFB_MODE_HAS_PWM);

    // Set up serial communication at a baud rate of baudRate (9600 bps) then
    // wait up to 5 seconds for a port to open.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Use the MSP application to enable and move the motor. The duty cycle of
    // the HFLB output will be refreshed and displayed every 0.5 seconds.

    // Check the state of the HLFB.
    MotorDriver::HlfbStates hlfbState = motor.HlfbState();

    // Print the HLFB state.
    if (hlfbState == MotorDriver::HLFB_HAS_MEASUREMENT) {
        // Get the measured speed as a percent of Max Speed.
        float hlfbPercent = motor.HlfbPercent();

        Serial.print("Speed output: ");

        if (hlfbPercent == MotorDriver::HLFB_DUTY_UNKNOWN) {
            Serial.println("UNKNOWN");
        }
        else {
            char hlfbPercentStr[10];
            // Convert the floating point duty cycle into a string representation.
            snprintf(hlfbPercentStr, sizeof(hlfbPercentStr), "%.0f%%", hlfbPercent);
            Serial.print(hlfbPercentStr);
            Serial.println(" of maximum speed");
        }
    }
    else if (hlfbState == MotorDriver::HLFB_DEASSERTED) {
        Serial.println("Disabled or Shutdown");
    }

    // Wait 0.5 secs before reading the HLFB again.
    delay(500);
}
