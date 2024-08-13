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
        volatile const MotorDriver::AlertRegMotor &alertReg = motor->AlertReg();
        
        Serial.print("Motor status register for motor M");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(statusReg.reg, 2); // prints the status register in binary
      
      Serial.print("AtTargetPosition:  ");
      Serial.println(statusReg.bit.AtTargetPosition);
    
      Serial.print("StepsActive:       ");
      Serial.println(statusReg.bit.StepsActive);
    
      Serial.print("AtTargetVelocity:  ");
      Serial.println(statusReg.bit.AtTargetVelocity);
    
      Serial.print("MoveDirection:     ");
      Serial.println(statusReg.bit.MoveDirection);
    
      Serial.print("MotorInFault:      ");
      Serial.println(statusReg.bit.MotorInFault);
    
      Serial.print("Enabled:           ");
      Serial.println(statusReg.bit.Enabled);
    
      Serial.print("PositionalMove:    ");
      Serial.println(statusReg.bit.PositionalMove);
    
      Serial.print("HLFB State:    ");
      switch (statusReg.bit.HlfbState) {
        case 0:
          Serial.println("HLFB_DEASSERTED");
          break;
        case 1:
          Serial.println("HLFB_ASSERTED");
          break;
        case 2:
          Serial.println("HLFB_HAS_MEASUREMENT");
          break;
        case 3:
          Serial.println("HLFB_UNKNOWN");
          break;
        default:
          // something has gone wrong if this is printed
          Serial.println("???");
      }
    
      Serial.print("AlertsPresent:     ");
      Serial.println(statusReg.bit.AlertsPresent);
    
      Serial.print("Ready state:   ");
      switch (statusReg.bit.ReadyState) {
        case MotorDriver::MOTOR_DISABLED:
          Serial.println("Disabled");
          break;
        case MotorDriver::MOTOR_ENABLING:
          Serial.println("Enabling");
          break;
        case MotorDriver::MOTOR_FAULTED:
          Serial.println("Faulted");
          break;
        case MotorDriver::MOTOR_READY:
          Serial.println("Ready");
          break;
        case MotorDriver::MOTOR_MOVING:
          Serial.println("Moving");
          break;
        default:
          // something has gone wrong if this is printed
          Serial.println("???");
      }
      
      Serial.print("Triggering:        ");
      Serial.println(statusReg.bit.Triggering);
    
      Serial.print("InPositiveLimit:   ");
      Serial.println(statusReg.bit.InPositiveLimit);
    
      Serial.print("InNegativeLimit:   ");
      Serial.println(statusReg.bit.InNegativeLimit);
    
      Serial.print("InEStopSensor:     ");
      Serial.println(statusReg.bit.InEStopSensor); 
    
      Serial.println("--------------------------------");
      
        
      if (statusReg.bit.AlertsPresent){
        Serial.print("Alert register:  ");
        Serial.println(alertReg.reg, 2); // prints the alert register in binary
    
        Serial.print("MotionCanceledInAlert:         ");
        Serial.println(alertReg.bit.MotionCanceledInAlert);
    
        Serial.print("MotionCanceledPositiveLimit:   ");
        Serial.println(alertReg.bit.MotionCanceledPositiveLimit);
    
        Serial.print("MotionCanceledNegativeLimit:   ");
        Serial.println(alertReg.bit.MotionCanceledNegativeLimit);
    
        Serial.print("MotionCanceledSensorEStop:     ");
        Serial.println(alertReg.bit.MotionCanceledSensorEStop);
    
        Serial.print("MotionCanceledMotorDisabled:   ");
        Serial.println(alertReg.bit.MotionCanceledMotorDisabled);
    
        Serial.print("MotorFaulted:                  ");
        Serial.println(alertReg.bit.MotorFaulted);
    
        Serial.println("--------------------------------");
      }
    }
    // Wait a few seconds then repeat...
    delay(5000);
}

//------------------------------------------------------------------------------
