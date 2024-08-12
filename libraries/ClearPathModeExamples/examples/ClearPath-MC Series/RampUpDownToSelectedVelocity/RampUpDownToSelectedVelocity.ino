/*
 * Title: RampUpDownToSelectedVelocity
 *
 * Objective:
 *    This example demonstrates control of the ClearPath-MC operational mode
 *    Ramp Up/Down To Selected Velocity.
 *
 * Description:
 *    This example enables and then moves a ClearPath motor between
 *    preprogrammed velocity selections as defined in the MSP software. During
 *    operation, various move statuses are written to the USB serial port.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Ramp Up/Down to Selected Velocity mode (In MSP
 *    select Mode>>Velocity>>Ramp Up/Down to Selected Velocity, then hit the OK
 *    button).
 * 3. The ClearPath motor must be set to use the HLFB mode "ASG-Velocity
 *    w/Measured Torque" with a PWM carrier frequency of 482 Hz through the MSP
 *    software (select Advanced>>High Level Feedback [Mode]... then choose
 *    "ASG-Velocity w/Measured Torque" from the dropdown, make sure that 482 Hz
 *    is selected in the "PWM Carrier Frequency" dropdown, and hit the OK
 *    button).
 * 4. The ClearPath must have defined Velocity Selections through the MSP
 *    software (On the main MSP window check the "Velocity Selection Setup
 *    (RPM)" box and fill in the four text boxes labeled "A off B off", "A on B
 *    off", "A off B on", and "A on B on").
 * 5. Ensure the Input A & B filters in MSP are both set to 20ms (In MSP
 *    select Advanced>>Input A, B Filtering... then in the Settings box fill in
 *    the textboxes labeled "Input A Filter Time Constant (msec)" and "Input B
 *    Filter Time Constant (msec)" then hit the OK button).
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

// The INPUT_A_B_FILTER must match the Input A, B filter setting in
// MSP (Advanced >> Input A, B Filtering...)
#define INPUT_A_B_FILTER 20

// Defines the motor's connector as ConnectorM0
#define motor ConnectorM0

// Select the baud rate to match the target device.
#define baudRate 9600

// This example has built-in functionality to automatically clear motor faults. 
//	Any uncleared fault will cancel and disallow motion.
// WARNING: enabling automatic fault handling will clear faults immediately when 
//	encountered and return a motor to a state in which motion is allowed. Before 
//	enabling this functionality, be sure to understand this behavior and ensure 
//	your system will not enter an unsafe state. 
// To enable automatic fault handling, #define HANDLE_MOTOR_FAULTS (1)
// To disable automatic fault handling, #define HANDLE_MOTOR_FAULTS (0)
#define HANDLE_MOTOR_FAULTS (0)

// Declares user-defined helper functions.
// The definition/implementations of these functions are at the bottom of the sketch.
bool RampToVelocitySelection(int velocityIndex);
void HandleMotorFaults();

void setup() {
    // Put your setup code here, it will only run once:

    // Sets all motor connectors to the correct mode for Ramp Up/Down to
    // Selected Velocity mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_A_DIRECT_B_DIRECT);

    // Set the motor's HLFB mode to bipolar PWM
    motor.HlfbMode(MotorDriver::HLFB_MODE_HAS_BIPOLAR_PWM);
    // Set the HFLB carrier frequency to 482 Hz
    motor.HlfbCarrier(MotorDriver::HLFB_CARRIER_482_HZ);

    // Enforces the state of the motor's A and B inputs before enabling the motor
    motor.MotorInAState(false);
    motor.MotorInBState(false);

    // Sets up serial communication and waits up to 5 seconds for a port to open.
    // Serial communication is not required for this example to run.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Enables the motor
    motor.EnableRequest(true);
    Serial.println("Motor Enabled");

    // Waits for HLFB to assert
    Serial.println("Waiting for HLFB...");
    while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED &&
			!motor.StatusReg().bit.MotorInFault) {
        continue;
    }
	// Check if a motor faulted during enabling
	// Clear fault if configured to do so 
    if (motor.StatusReg().bit.MotorInFault) {
		Serial.println("Motor fault detected.");		
		if(HANDLE_MOTOR_FAULTS){
			HandleMotorFaults();
		} else {
			Serial.println("Enable automatic fault handling by setting HANDLE_MOTOR_FAULTS to 1.");
		}
		Serial.println("Enabling may not have completed as expected. Proceed with caution.");		
 		Serial.println();
	} else {
		Serial.println("Motor Ready");	
	}
}


void loop() {
    // Move to Velocity 1 defined in MSP (Inputs A off, B off).
    // See below for the detailed function definition.
    RampToVelocitySelection(1);

    // Wait 1000ms.
    delay(1000);
    RampToVelocitySelection(2); // Inputs A on, B off
    delay(1000);
    RampToVelocitySelection(3); // Inputs A off, B on
    delay(1000);
    RampToVelocitySelection(4); // Inputs A on, B on
    delay(1000);

    // Alternatively, if you'd like to control the ClearPath motor's inputs
    // directly using ClearCore inputs consider doing something like this:
    /*
    // Sets ClearPath's InA to DI6's state.
     motor.MotorInAState(digitalRead(DI6));

    // Sets ClearPath's InB to DI7's state.
     motor.MotorInBState(digitalRead(DI7));
    */
}

/*------------------------------------------------------------------------------
 * RampToVelocitySelection
 *
 *    Move to Velocity Selection number velocityIndex (defined in MSP)
 *    Prints the move status to the USB serial port
 *    Returns when HLFB asserts (indicating the motor has reached the target
 *    velocity)
 *
 * Parameters:
 *    int velocityIndex  - The velocity number to command (defined in MSP)
 *
 * Returns: True/False depending on whether the velocity selection was
 * successfully commanded.
 */
bool RampToVelocitySelection(int velocityIndex) {
    // Check if a motor fault is currently preventing motion
	// Clear fault if configured to do so 
    if (motor.StatusReg().bit.MotorInFault) {
		if(HANDLE_MOTOR_FAULTS){
			Serial.println("Motor fault detected. Move canceled.");
			HandleMotorFaults();
		} else {
			Serial.println("Motor fault detected. Move canceled. Enable automatic fault handling by setting HANDLE_MOTOR_FAULTS to 1.");
		}
        return false;
    }

    Serial.print("Moving to Velocity Selection: ");
    Serial.print(velocityIndex);

    switch (velocityIndex) {
        case 1:
            // Sets Input A and B for velocity 1
            motor.MotorInAState(false);
            motor.MotorInBState(false);
            Serial.println(" (Inputs A Off/B Off)");
            break;
        case 2:
            // Sets Input A and B for velocity 2
            motor.MotorInAState(true);
            motor.MotorInBState(false);
            Serial.println(" (Inputs A On/B Off)");
            break;
        case 3:
            // Sets Input A and B for velocity 3
            motor.MotorInAState(false);
            motor.MotorInBState(true);
            Serial.println(" (Inputs A Off/B On)");
            break;
        case 4:
            // Sets Input A and B for velocity 4
            motor.MotorInAState(true);
            motor.MotorInBState(true);
            Serial.println(" (Inputs A On/B On)");
            break;
        default:
            // If this case is reached then an incorrect velocityIndex was
            // entered
            return false;
    }

    // Ensures this delay is at least 20ms longer than the Input A, B filter
    // setting in MSP
    delay(20 + INPUT_A_B_FILTER);

    // Waits for HLFB to assert (signaling the move has successfully reached its
    // target velocity)
    Serial.println("Moving.. Waiting for HLFB");
    while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED &&
			!motor.StatusReg().bit.MotorInFault) {
        continue;
    }
	// Check if a motor faulted during move
	// Clear fault if configured to do so 
    if (motor.StatusReg().bit.MotorInFault) {
		Serial.println("Motor fault detected.");		
		if(HANDLE_MOTOR_FAULTS){
			HandleMotorFaults();
		} else {
			Serial.println("Enable automatic fault handling by setting HANDLE_MOTOR_FAULTS to 1.");
		}
		Serial.println("Motion may not have completed as expected. Proceed with caution.");
		Serial.println();
		return false;
    } else {
		Serial.println("Move Done");
		return true;
	}
}
//------------------------------------------------------------------------------
 
/*------------------------------------------------------------------------------
 * HandleMotorFaults
 *
 *    Clears motor faults by cycling enable to the motor.
 *    Assumes motor is in fault 
 *      (this function is called when motor.StatusReg.MotorInFault == true)
 *
 * Parameters:
 *    requires "motor" to be defined as a ClearCore motor connector
 *
 * Returns: 
 *    none
 */
 void HandleMotorFaults(){
 	Serial.println("Handling fault: clearing faults by cycling enable signal to motor.");
	motor.EnableRequest(false);
	Delay_ms(10);
	motor.EnableRequest(true);
	Delay_ms(100);
}
//------------------------------------------------------------------------------
