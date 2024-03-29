/*
 * Title: ManualVelocity
 *
 * Objective:
 *    This example demonstrates control of the ClearPath-MC operational mode
 *    Manual Velocity Control.
 *
 * Description:
 *    This example enables a ClearPath motor and executes a repeating pattern of
 *    bidirectional velocity moves. During operation, various move statuses are
 *    written to the USB serial port.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Manual Velocity Control mode (In MSP select Mode>>Velocity>>Manual
 *    Velocity Control, then hit the OK button).
 * 3. In the MSP software:
 *    * Define a Max Clockwise and Counter-Clockwise (CW/CCW) Velocity (On the
 *      main MSP window fill in the textboxes labeled "Max CW Velocity (RPM)"
 *      and "Max CCW Velocity (RPM)"). Any velocity commanded outside of this
 *      range will be rejected.
 *    * Set the Velocity Resolution to 2 (On the main MSP window check the
 *      textbox labeled "Velocity Resolution (RPM per knob count)" 2 is
 *      default). This means the commanded velocity will always be a multiple
 *      of 2. For finer resolution, lower this value and change
 *      velocityResolution in the sketch below to match.
 *    * Set Knob Direction to As-Wired, and check the Has Detents box (On the
 *      main MSP window check the dropdown labeled "Knob Direction" and the
 *      checkbox directly below it labeled "Has Detents").
 *    * On the main MSP window set the dropdown labeled "On Enable..." to be
 *      "Zero Velocity".
 *    * Set the HLFB mode to "ASG-Velocity w/Measured Torque" with a PWM carrier
 *      frequency of 482 Hz through the MSP software (select Advanced>>High
 *      Level Feedback [Mode]... then choose "ASG-Velocity w/Measured Torque" 
 *      from the dropdown, make sure that 482 Hz is selected in the "PWM Carrier
 *      Frequency" dropdown, and hit the OK button).
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

// This is the variable used to keep track of the current commanded velocity
double commandedVelocity = 0;

// A reference to the maximum clockwise and counter-clockwise velocities set in
// the MSP software. These must match the values in MSP
int maxVelocityCW = 1000;
int maxVelocityCCW = 1000;

// Each velocity commanded will be a multiple of this value, which must match
// the Velocity Resolution value in MSP. Use a lower value here (and in MSP) to
// command velocity with a finer resolution
double velocityResolution = 2.0;

// Declares user-defined helper functions.
// The definition/implementations of these functions are at the bottom of the sketch.
bool MoveAtVelocity(double velocity);
void HandleMotorFaults();

void setup() {
    // Put your setup code here, it will run once:

    // Set all motor connectors to the correct mode for Manual Velocity
    // mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_A_DIRECT_B_DIRECT);

    // Set the motor's HLFB mode to bipolar PWM
    motor.HlfbMode(MotorDriver::HLFB_MODE_HAS_BIPOLAR_PWM);
    // Set the HFLB carrier frequency to 482 Hz
    motor.HlfbCarrier(MotorDriver::HLFB_CARRIER_482_HZ);

    // Enforces the state of the motor's A and B inputs before enabling
    // the motor.
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
    // Put your main code here, it will run repeatedly:

    // Spin at 500 RPM in the CCW direction.
    MoveAtVelocity(500);    // See below for the detailed function definition.

    // Hold the velocity for 5 seconds before issuing a new command.
    delay(5000);

    // Spin at 100 RPM in the CW direction.
    MoveAtVelocity(-100);
    delay(5000);

    // Spin at 750 RPM in the CW direction.
    MoveAtVelocity(-750);
    delay(5000);

    // Spin at 1000 RPM in the CCW direction.
    MoveAtVelocity(1000);
    delay(5000);

    // Command 0 RPM, the motor shaft is stationary.
    MoveAtVelocity(0);
    delay(5000);
}

/*------------------------------------------------------------------------------
 * MoveAtVelocity
 *
 *    Triggers a quadrature output commanding the desired velocity.
 *    Prints the velocity and move status to the USB serial port.
 *    Returns when HLFB asserts (indicating move has successfully completed).
 *
 * Parameters:
 *    double velocity  - The velocity in RPM to command
 *
 * Returns: True/False depending on whether a new velocity was reached
 */
bool MoveAtVelocity(double velocity) {
    // If the same velocity is commanded there's nothing to do.
    if (velocity == commandedVelocity) {
        return false;
    }

    // Check to see if the requested velocity exceeds the valid range.
    if (velocity > maxVelocityCCW || velocity < -maxVelocityCW) {
        Serial.print("An invalid velocity of ");
        Serial.print(velocity);
        Serial.println(" RPM has been requested.");
        return false;
    }

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

    Serial.print("Commanding ");
    Serial.print(velocity);
    Serial.println(" RPM");

    // Determine which order the quadrature must be sent by determining if the
    // new velocity is greater or less than the previously commanded velocity
    // If greater, Input A begins the quadrature. If less, Input B begins the
    // quadrature.
    int32_t currentVelocityRounded = round(commandedVelocity / velocityResolution);
    int32_t targetVelocityRounded = round(velocity / velocityResolution);
    int32_t velocityDifference = labs(targetVelocityRounded - currentVelocityRounded);
    for (int32_t i = 0; i < velocityDifference; i++) {
        if (velocity > commandedVelocity) {
            // Toggle Input A to begin the quadrature signal.
            motor.MotorInAState(true);
            // Command a 5 microsecond delay to ensure proper signal timing.
            delayMicroseconds(5);
            motor.MotorInBState(true);
            delayMicroseconds(5);
            motor.MotorInAState(false);
            delayMicroseconds(5);
            motor.MotorInBState(false);
            delayMicroseconds(5);
        }
        else {
            motor.MotorInBState(true);
            delayMicroseconds(5);
            motor.MotorInAState(true);
            delayMicroseconds(5);
            motor.MotorInBState(false);
            delayMicroseconds(5);
            motor.MotorInAState(false);
            delayMicroseconds(5);
        }
    }

    // Keeps track of the new commanded velocity
    commandedVelocity = velocity;

    // Waits for HLFB to assert (signaling the motor has successfully reached
    // its target velocity).
    Serial.println("Ramping Speed... Waiting for HLFB");
    // Wait for some time so HLFB has time to transition.
    delay(1);
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
