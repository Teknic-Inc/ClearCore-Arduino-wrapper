/*
 * Title: StepAndDirection
 *
 * Objective:
 *    This example demonstrates control of a third party Step and
 *    Direction motor using a ClearCore motor connector.
 *    This example is NOT intended to be used with ClearPath servos.
 *    There are other examples created specifically for ClearPath.
 *
 * Description:
 *    This example enables a motor then commands a series of repeating
 *    moves to the motor.
 *
 * Requirements:
 * 1. A motor capable of step and direction must be connected to Connector M-0.
 * 2. The motor may optionally be connected to the MotorDriver's HLFB line if 
 *    the motor has a "servo on" type feedback feature.
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
// of the example.
void MoveDistance(int distance);

void setup() {
    // Put your setup code here, it will only run once:

    // Sets the input clocking rate.
    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);

    // Sets all motor connectors into step and direction mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);

    // These lines may be uncommented to invert the output signals of the 
    // Enable, Direction, and HLFB lines. Some motors may have input polarities 
    // that are inverted from the ClearCore's polarity.
    //motor.PolarityInvertSDEnable(true);
    //motor.PolarityInvertSDDirection(true);
    //motor.PolarityInvertSDHlfb(true);

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

    // Enables the motor.
    motor.EnableRequest(true);

    // Waits for HLFB to assert. Uncomment these lines if your motor has a 
    // "servo on" feature and it is wired to the HLFB line on the connector.
    //Serial.println("Waiting for HLFB...");
    //while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
    //    continue;
    //}
    Serial.println("Motor Ready");
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Move 6400 counts (positive direction), then wait 2000ms
    MoveDistance(6400);
    delay(2000);
    // Move 19200 counts farther positive, then wait 2000ms
    MoveDistance(19200);
    delay(2000);
    // Move back 12800 counts (negative direction), then wait 2000ms
    MoveDistance(-12800);
    delay(2000);
    // Move back 6400 counts (negative direction), then wait 2000ms
    MoveDistance(-6400);
    delay(2000);
    // Move back to the start (negative 6400 pulses), then wait 2000ms
    MoveDistance(-6400);
    delay(2000);
}

/*------------------------------------------------------------------------------
 * MoveDistance
 *
 *    Command "distance" number of step pulses away from the current position
 *    Prints the move status to the USB serial port
 *    Returns when step pulses have completed
 *
 * Parameters:
 *    int distance  - The distance, in step pulses, to move
 *
 * Returns: None
 */
void MoveDistance(int distance) {
    Serial.print("Moving distance: ");
    Serial.println(distance);

    // Command the move of incremental distance
    motor.Move(distance);

    // Waits for all step pulses to output
    Serial.println("Moving... Waiting for the step output to finish...");
    while (!motor.StepsComplete()) {
        continue;
    }

    Serial.println("Steps Complete");
}
//------------------------------------------------------------------------------