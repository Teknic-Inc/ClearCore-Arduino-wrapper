/*
 * Title: MoveVelocity
 *
 * Objective:
 *    This example demonstrates control of a ClearPath motor in Step and
 *    Direction mode, making velocity moves.
 *
 * Description:
 *    This example enables a ClearPath then commands a series of repeating
 *    velocity moves to the motor.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Step and Direction mode (In MSP select Mode>>Step and Direction).
 * 3. The ClearPath motor must be set to use the HLFB mode "ASG-Position"
 *    through the MSP software (select Advanced>>High Level Feedback [Mode]...
 *    then choose "All Systems Go (ASG) - Position" from the dropdown and hit
 *    the OK button).
 * 4. Set the Input Format in MSP for "Step + Direction".
 *
 * ** Note: Set the Input Resolution in MSP the same as your motor's Positioning
 *    Resolution spec if you'd like the pulse frequency sent by ClearCore to 
 *    command the same frequency in motor encoder counts/sec, a 1:1 ratio.
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

// Specifies which motor to move.
// Options are: ConnectorM0, ConnectorM1, ConnectorM2, or ConnectorM3.
#define motor ConnectorM0

// Select the baud rate to match the target serial device
#define baudRate 9600

// Define the velocity and acceleration limits to be used for each move
int accelerationLimit = 100000; // pulses per sec^2

// Declares our user-defined helper function, which is used to command moves to
// the motor. The definition/implementation of this function is at the  bottom
// of the example
void MoveAtVelocity(int32_t velocity);

void setup() {
    // Put your setup code here, it will only run once:

    // Sets the input clocking rate. This normal rate is ideal for ClearPath
    // step and direction applications.
    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);

    // Sets all motor connectors into step and direction mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);

    // Set the maximum acceleration for each move
    motor.AccelMax(accelerationLimit);

    // Sets up serial communication and waits up to 5 seconds for a port to open.
    // Serial communication is not required for this example to run.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Enables the motor; homing will begin automatically if enabled
    motor.EnableRequest(true);
    Serial.println("Motor Enabled");

    // Waits for HLFB to assert (waits for homing to complete if applicable)
    Serial.println("Waiting for HLFB...");
    while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
        continue;
    }
    Serial.println("Motor Ready");
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Move at 10,000 counts/sec, then wait 2000ms
    MoveAtVelocity(10000);
    delay(2000);
    // Move at -5,000 counts/sec, then wait 2000ms
    MoveAtVelocity(-5000);
    delay(2000);
    // Move at 7,000 counts/sec, then wait 2000ms
    MoveAtVelocity(7000);
    delay(2000);
    // Move at -10,000 counts/sec, then wait 2000ms
    MoveAtVelocity(-10000);
    delay(2000);
    // Command a 0 counts/sec velocity to stop motion, then wait 2000ms
    MoveAtVelocity(0);
    delay(2000);
}

/*------------------------------------------------------------------------------
 * MoveAtVelocity
 *
 *    Command the motor to move at the specified "velocity", in pulses/second.
 *    Prints the move status to the USB serial port
 *
 * Parameters:
 *    int velocity  - The velocity, in step pulses/sec, to command
 *
 * Returns: None
 */
void MoveAtVelocity(int velocity) {
    Serial.print("Moving at velocity: ");
    Serial.println(velocity);

    // Command the velocity move
    motor.MoveVelocity(velocity);
}
//------------------------------------------------------------------------------
