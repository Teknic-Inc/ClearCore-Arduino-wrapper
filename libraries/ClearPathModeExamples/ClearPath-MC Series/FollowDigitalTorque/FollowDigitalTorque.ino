/*
 * Title: FollowDigitalTorque
 *
 * Objective:
 *    This example demonstrates control of the ClearPath-MC operational mode
 *    Follow Digital Torque Command, Unipolar PWM Command.
 *
 * Description:
 *    This example enables and then commands a ClearPath motor to output various
 *    torques. in both the clockwise and counterclockwise directions.
 *    During operation, various move statuses are written to the USB serial
 *    port.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Follow Digital Torque Command, Unipolar PWM Command mode  (In MSP
 *    select Mode>>Torque>>Follow Digital Torque Command, then with
 *    "Unipolar PWM Command" selected hit the OK button).
 * 3. The ClearPath motor must be set to use the HLFB mode "ASG-Torque"
 *    through the MSP software (select Advanced>>High Level Feedback [Mode]...
 *    then choose "All Systems Go (ASG) - Torque" from the dropdown and hit
 *    the OK button).
 * 4. The ClearPath must have a defined Max Torque Command configured through
 *    the MSP software. This value must match the "maxTorque" variable defined
 *    below.
 * 5. Ensure the Input A filter in MSP is set to 20ms (In MSP select
 *    Advanced>>Input A, B Filtering... then in the Settings box fill in the
 *    textbox labeled "Input A Filter Time Constant (msec)" then hit the OK
 *    button).
 * 6. Set your Max Speed and Over-Speed Timeout in MSP according to your
 *    mechanical system. Note you may notice any of the following if this max
 *    speed is reached: motor shutdown, speed limit cycling at the max speed,
 *    HLFB not signaling ASG/move done.
 * 7. Ensure the "Invert PWM Input" box is unchecked in MSP.
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

// The INPUT_A_FILTER must match the Input A filter setting in
// MSP (Advanced >> Input A, B Filtering...)
#define INPUT_A_FILTER 20

// Defines the motor's connector as ConnectorM0
#define motor ConnectorM0

// Select the baud rate to match the target device.
#define baudRate 9600

// Defines the limit of the torque command, as a percent of the motor's peak
// torque rating (must match the value used in MSP).
double maxTorque = 100;

// Declares our user-defined helper function, which is used to command torque.
// The definition/implementation of this function is at the bottom of the sketch.
bool CommandTorque(int commandedTorque);

void setup() {
    // Put your setup code here, it will only run once:

    // Sets all motor connectors to the correct mode for Follow Digital
    // Torque mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_A_DIRECT_B_PWM);

    // Sets up serial communication and waits up to 5 seconds for a port to open
    // Serial communication is not required for this example to run
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Enables the motor
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

    // Output 15% of the motor's peak torque in the positive (CCW) direction.
    CommandTorque(15);    // See below for the detailed function definition.
    // Wait 2000ms.
    delay(2000);

    CommandTorque(-75); // Output 75% peak torque in the negative (CW) direction.
    delay(2000);

    CommandTorque(5); // Output 5% peak torque in the positive (CCW) direction.
    delay(2000);

    CommandTorque(-35); // Output 35% peak torque in the negative (CW) direction.
    delay(2000);

    CommandTorque(10); // Output 10% peak torque in the positive (CCW) direction.
    delay(2000);
}

/*------------------------------------------------------------------------------
 * CommandTorque
 *
 *    Command the motor to move using a torque of commandedTorque
 *    Prints the move status to the USB serial port
 *    Returns when HLFB asserts (indicating the motor has reached the commanded
 *    torque)
 *
 * Parameters:
 *    int commandedTorque  - The torque to command
 *
 * Returns: True/False depending on whether the torque was successfully
 * commanded.
 */
bool CommandTorque(int commandedTorque) {
    if (abs(commandedTorque) > abs(maxTorque)) {
        Serial.println("Move rejected, invalid torque requested");
        return false;
    }
	
    Serial.print("Commanding torque: ");
    Serial.println(commandedTorque);

    // Find the scaling factor of our torque range mapped to the PWM duty cycle
    // range (255 is the max duty cycle).
    double scaleFactor = 255 / maxTorque;

    // Scale the torque command to our duty cycle range.
    int dutyRequest = abs(commandedTorque) * scaleFactor;

    // Set input A to match the direction of torque.
    if (commandedTorque < 0) {
        motor.MotorInAState(true);
    }
    else {
        motor.MotorInAState(false);
    }

    // Ensures this delay is at least 2ms longer than the Input A filter
    // setting in MSP
    delay(2 + INPUT_A_FILTER);

    // Command the move
    motor.MotorInBDuty(dutyRequest);

    // Waits for HLFB to assert (signaling the move has successfully completed)
    Serial.println("Moving... Waiting for HLFB");
    // Allow some time for HLFB to transition
    delay(1);
    while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
        continue;
    }

    Serial.println("Move Done");
    return true;
}
//------------------------------------------------------------------------------