/*
 * Title: MovePositionAbsolute
 *
 * Objective:
 *    This example demonstrates control of a ClearPath motor in Step and
 *    Direction mode using hardstop homing and absolute position moves.
 *
 * Description:
 *    This example enables a ClearPath, homes to a hardstop, then commands 
 *    a series of repeating absolute position moves to the motor.
 *
 * Requirements:
 * 1. A ClearPath-SD motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Step and Direction mode (In MSP select Mode>>Step and Direction).
 * 3. The ClearPath motor must be set to use the HLFB mode "ASG-Position"
 *    through the MSP software (select Advanced>>High Level Feedback [Mode]...
 *    then choose "All Systems Go (ASG) - Position" from the dropdown and hit
 *    the OK button).
 * 4. Set the Input Format in MSP for "Step + Direction".
 * 5. Configure ClearPath to use automatic hardstop homing. "Enable" homing 
 *    in the homing field, and click "Setup..." and in that window:
 *      - Use "Normal" homing
 *      - Select "ClearPath Homes Automatically"
 *      - Set to home "Upon every Enable"
 *      - Set the appropriate homing direction (see note below on move direction)
 *      - Enter your desired "Physical Home Clearance"
 *      - It is recommended to use a "Home Offset Move Distance" of 0 so MSP and 
 *      ClearCore both have the same 0 position.
 *      
 * ** This example makes moves to positive absolute positions,
 *    assuming any homing move occurs in the negative direction.
 *    Change the target positions to negative if homing is positive.
 *
 * ** Note: Homing is optional, and not required in this operational mode.
 *    However, homing is typically recommended when making
 *    absolute position moves to ensure an absolute position on the output
 *    mechanics corresponds to a consistent absolute position for ClearCore
 *    and ClearPath.
 *
 * ** Note: Set the Input Resolution in MSP the same as your motor's Positioning
 *    Resolution spec if you'd like the pulses sent by ClearCore to command a
 *    move of the same number of Encoder Counts, a 1:1 ratio.
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 * ** ClearPath Manual (DC Power): https://www.teknic.com/files/downloads/clearpath_user_manual.pdf
 * ** ClearPath Manual (AC Power): https://www.teknic.com/files/downloads/ac_clearpath-mc-sd_manual.pdf
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
int velocityLimit = 10000; // pulses per sec
int accelerationLimit = 100000; // pulses per sec^2

// Declares our user-defined helper function, which is used to command moves to
// the motor. The definition/implementation of this function is at the  bottom
// of the example
void MoveAbsolutePosition(int32_t position);

void setup() {
    // Put your setup code here, it will only run once:

    // Sets the input clocking rate. This normal rate is ideal for ClearPath
    // step and direction applications.
    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);

    // Sets all motor connectors into step and direction mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);

    // Sets the maximum velocity for each move
    motor.VelMax(velocityLimit);

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

    // Waits for HLFB to assert (waits for homing to complete)
    Serial.println("Homing. Waiting for HLFB...");
    while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
        continue;
    }
    Serial.println("Motor Ready");
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // This example makes moves to positive absolute positions,
    // assuming any homing move occurs in the negative direction.
    // Change the target positions to negative if homing is positive.

    // Move to +6400 counts then wait 2000ms
    MoveAbsolutePosition(6400);
    delay(2000);
    // Move to +12800 counts, then wait 2000ms
    MoveAbsolutePosition(12800);
    delay(2000);
    // Move back to "software home", then wait 2000ms
    MoveAbsolutePosition(0);
    delay(2000);
}

/*------------------------------------------------------------------------------
 * MoveAbsolutePosition
 *
 *    Command step pulses to move the motor's current position to the absolute
 *    position specified by "position"
 *    Prints the move status to the USB serial port
 *    Returns when HLFB asserts (indicating the motor has reached the commanded
 *    position)
 *
 * Parameters:
 *    int position  - The absolute position, in step pulses, to move to
 *
 * Returns: None
 */
void MoveAbsolutePosition(int position) {
    Serial.print("Moving to absolute position: ");
    Serial.println(position);

    // Command the move of absolute distance
    motor.Move(position, MotorDriver::MOVE_TARGET_ABSOLUTE);

    // Waits for HLFB to assert (signaling the move has successfully completed)
    Serial.println("Moving.. Waiting for HLFB");
    while (!motor.StepsComplete() || motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
        continue;
    }

    Serial.println("Move Done");
}
//------------------------------------------------------------------------------
