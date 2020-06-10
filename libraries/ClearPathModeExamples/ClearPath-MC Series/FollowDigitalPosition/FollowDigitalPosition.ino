/*
 * Title: FollowDigitalPosition
 *
 * Objective:
 *    This example demonstrates control of the ClearPath-MCPV operational mode
 *    Follow Digital Position Command, Unipolar PWM Command.
 *
 * Description:
 *    This example enables and then moves a ClearPath motor between various
 *    repeating positions within a range defined in the MSP software. During
 *    operation, various move statuses are written to the USB serial port.
 *    The resolution for PWM outputs is 8-bit, meaning only 256 discrete
 *    positions can be commanded. The motor's actual commanded position may
 *    differ from what you input below because of this.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Follow Digital Position Command, Unipolar PWM Command mode (In MSP
 *    select Mode>>Position>>Follow Digital Position Command, then with
 *    "Unipolar PWM Command" selected hit the OK button).
 * 3. The ClearPath motor must be set to use the HLFB mode "ASG-Position"
 *    through the MSP software (select Advanced>>High Level Feedback [Mode]...
 *    then choose "All Systems Go (ASG) - Position" from the dropdown and hit
 *    the OK button).
 * 3. The ClearPath must have defined positions for 0% and 100% PWM (On the
 *    main MSP window check the "Position Range Setup (cnts)" box and fill in
 *    the two text boxes labeled "Posn at 0% PWM" and "Posn at 100% PWM").
 *    Change the "PositionZeroPWM" and "PositionMaxPWM" variables in the sketch
 *    below to match.
 * 4. Homing must be configured in the MSP software for your mechanical
 *    system (e.g. homing direction, torque limit, etc.). This example does
 *    not use the ClearPath's Input A as a homing sensor, although that may
 *    be configured in this mode through MSP.
 * 5. (Optional) An input source, such as a switch, connected to DI-6 to control
 *    the Command Lock or Home Sensor (configured in MSP).
 *
 * Links:
 * ** web link to doxygen (all Examples)
 * ** web link to ClearCore Manual (all Examples)  <<FUTURE links to Getting started webpage/ ClearCore videos>>
 * ** web link to ClearPath Operational mode video (Only ClearPath Examples)
 * ** web link to ClearPath manual (Only ClearPath Examples)
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"

// Defines the motor's connector as ConnectorM0
#define motor ConnectorM0

// Defines the command lock sensor connector
#define LockSensor DI6

// Select the baud rate to match the target device.
#define baudRate 9600

// Defines the bounds for our position range. These positions cannot actually be
// commanded, only values inside this range.
double positionZeroPWM = 0;
double positionMaxPWM = 10000;

// Declares our user-defined functions, which are used to pass the Command Lock
// sensor state and send position commands to the motor. The implementations of
// these functions are at the bottom of the sketch.
void LockSensorCallback();
bool CommandPosition(int commandedPosition);

void setup() {
    // Put your setup code here, it will only run once:

    // Sets all motor connectors to the correct mode for Follow Digital
    // Position mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_A_DIRECT_B_PWM);

    // This section attaches the interrupt callback to the locking sensor pin,
    // set to trigger on any change of sensor state
    pinMode(LockSensor, INPUT);
    attachInterrupt(digitalPinToInterrupt(LockSensor), LockSensorCallback, CHANGE);
    // Set input A to match the initial state of the sensor
    motor.MotorInAState(digitalRead(LockSensor));

    // Sets up serial communication and waits up to 5 seconds for a port to open
    // Serial communication is not required for this example to run
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Enables the motor; homing will begin automatically
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

    // Move to position +5000.
    CommandPosition(5000);    // See below for the detailed function definition.
    // Wait 2000ms.
    delay(2000);

    CommandPosition(4000);
    delay(2000);

    CommandPosition(1500);
    delay(2000);

    CommandPosition(9000);
    delay(2000);

    CommandPosition(1000);
    delay(2000);
}

/*------------------------------------------------------------------------------
 * CommandPosition
 *
 *    Move to position number commandedPosition (counts in MSP)
 *    Prints the move status to the USB serial port
 *    Returns when HLFB asserts (indicating the motor has reached the commanded
 *    position)
 *
 * Parameters:
 *    int commandedPosition  - The position, in counts, to command
 *
 * Returns: True/False depending on whether the position was successfully
 * commanded.
 */
bool CommandPosition(int commandedPosition) {
    if (abs(commandedPosition) > abs(positionMaxPWM) || abs(commandedPosition) < abs(positionZeroPWM)) {
        Serial.println("Move rejected, invalid position requested");
        return false;
    }
    Serial.print("Moving to position: ");
    Serial.println(commandedPosition);

    // Find the scaling factor of our position range mapped to the PWM duty
    // cycle range (255 is the max duty cycle).
    double scaleFactor = 255 / abs(positionMaxPWM - positionZeroPWM);

    // Scale the position command to our duty cycle range.
    int dutyRequest = abs(commandedPosition - positionZeroPWM) * scaleFactor;

    // Command the move.
    motor.MotorInBDuty(dutyRequest);

    // Waits for HLFB to assert (signaling the move has successfully completed)
    Serial.println("Moving... Waiting for HLFB");

    // Delay to give HLFB time to change according to the new command
    delay(2);

    while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
        continue;
    }

    Serial.println("Move Done");
    return true;
}
//------------------------------------------------------------------------------

/*------------------------------------------------------------------------------
 * LockSensorCallback
 *
 *    Reads the state of the locking sensor and passes the state to the motor.
 *
 * Parameters:
 *    None
 *
 * Returns: None
 */
void LockSensorCallback() {
    // A 1 ms delay is required in order to pass the correct filtered sensor
    // state.
    delay(1);
    motor.MotorInAState(digitalRead(LockSensor));
}
//------------------------------------------------------------------------------