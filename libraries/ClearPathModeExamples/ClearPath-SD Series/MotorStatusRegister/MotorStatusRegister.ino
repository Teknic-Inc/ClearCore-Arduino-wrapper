/*
 * Title: MotorStatusRegister
 *
 * Objective:
 *    This example demonstrates how to read and display bits in the ClearCore's
 *    MotorDriver status register.
 *
 * Description:
 *    This example gets a snapshot of the status register for each MotorDriver
 *    connector with an attached motor. Then, the state of the status register
 *    bits is printed to the USB serial port.
 *
 * Requirements:
 * ** A ClearPath motor must be connected to Connector M-0.
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

// Select the baud rate to match the target device.
#define baudRate 9600

// The containers for our motor objects. If only some of the motor connectors
// are being used, remove the unused entries from the following arrays and
// reduce motorConnectorCount.
MotorDriver *motorConnectors[] = {&ConnectorM0, &ConnectorM1,
                                  &ConnectorM2, &ConnectorM3
                                 };
char motorConnectorNames[][4] = { "M-0", "M-1", "M-2", "M-3" };
uint8_t motorConnectorCount = 4;

// Hold a string representation of each motor's ready state.
char *readyStateStr;

// Declare our helper function so that it may be used before it is defined.
char *ReadyStateString(MotorDriver::MotorReadyStates readyState);

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

    // Get a copy of the motor status register for each motor connector.
    for (uint8_t i = 0; i < motorConnectorCount; i++) {
        MotorDriver *motor = motorConnectors[i];
        volatile const MotorDriver::StatusRegMotor &statusReg = motor->StatusReg();

        Serial.print("Motor Status Register for ");
        Serial.print(motorConnectorNames[i]);
        Serial.println(":");

        Serial.print("Enabled:\t\t");
        if (statusReg.bit.Enabled) {
            Serial.println('1');
        }
        else {
            Serial.println('0');
        }

        Serial.print("Move direction:\t\t");
        if (statusReg.bit.MoveDirection) {
            Serial.println('+');
        }
        else {
            Serial.println('-');
        }

        Serial.print("Steps active:\t\t");
        if (statusReg.bit.StepsActive) {
            Serial.println('1');
        }
        else {
            Serial.println('0');
        }

        Serial.print("At velocity target:\t");
        if (statusReg.bit.AtTargetVelocity) {
            Serial.println('1');
        }
        else {
            Serial.println('0');
        }

        Serial.print("Ready state:\t\t");
        readyStateStr = ReadyStateString(statusReg.bit.ReadyState);
        Serial.println(readyStateStr);

        Serial.println("--------------------------------");
    }

    // Wait a few seconds then repeat...
    delay(5000);
}

/*------------------------------------------------------------------------------
 * ReadyStateString
 *
 *    Converts the state of a motor status register bit into a user-readable
 *    format so it may be printed to a serial port.
 *
 * Parameters:
 *    MotorReadyStates readyState  - The current state of the ReadyState bit
 *
 * Returns: Text describing the state of the status bit.
 */
char *ReadyStateString(MotorDriver::MotorReadyStates readyState) {
    switch (readyState) {
        case MotorDriver::MOTOR_DISABLED:
            return (char *)"Disabled";
        case MotorDriver::MOTOR_ENABLING:
            return (char *)"Enabling";
        case MotorDriver::MOTOR_FAULTED:
            return (char *)"Faulted";
        case MotorDriver::MOTOR_READY:
            return (char *)"Ready";
        case MotorDriver::MOTOR_MOVING:
            return (char *)"Moving";
        default:
            // Something has gone wrong if this is printed
            return (char *)"???";
    }
}
//------------------------------------------------------------------------------