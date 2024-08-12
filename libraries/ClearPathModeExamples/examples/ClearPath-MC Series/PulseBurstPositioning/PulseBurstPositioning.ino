/*
 * Title: PulseBurstPositioning
 *
 * Objective:
 *    This example demonstrates control of the ClearPath-MCPV operational mode
 *    Pulse Burst Positioning.
 *
 * Description:
 *    This example enables a ClearPath motor and executes a repeating pattern of
 *    positional move commands. During operation, various move statuses are
 *    written to the USB serial port.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Pulse Burst Positioning mode (In MSP select Mode>>Position>>Pulse
 *    Burst Positioning, then hit the OK button).
 * 3. The ClearPath motor must be set to use the HLFB mode "ASG-Position
 *    w/Measured Torque" with a PWM carrier frequency of 482 Hz through the MSP
 *    software (select Advanced>>High Level Feedback [Mode]... then choose
 *    "ASG-Position w/Measured Torque" from the dropdown, make sure that 482 Hz
 *    is selected in the "PWM Carrier Frequency" dropdown, and hit the OK
 *    button).
 * 4. Ensure the Trigger Pulse Time in MSP is set to 20ms. To configure, click
 *    the "Setup..." button found under the "Trigger Pulse" label on the MSP's
 *    main window, fill in the text box, and hit the OK button. Setting this to 
 *    20ms allows trigger pulses to be as long as 60ms, which will accommodate 
 *    our 25ms pulses used later.
 *
 * ** Note: Homing is optional, and not required in this operational mode or in
 *    this example. This example makes its first move in the positive direction,
 *    assuming any homing move occurs in the negative direction.
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
 *
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"

// Defines the motor's connector as ConnectorM0
#define motor ConnectorM0

// The TRIGGER_PULSE_TIME is set to 25ms to ensure it is within the
// Trigger Pulse Range defined in the MSP software (Default is 20ms, which 
// allows a range of pulses up to 60ms)
#define TRIGGER_PULSE_TIME 25

// Select the baud rate to match the target device.
#define baudRate 9600

// This example has built-in functionality to automatically clear motor alerts, 
//	including motor shutdowns. Any uncleared alert will cancel and disallow motion.
// WARNING: enabling automatic alert handling will clear alerts immediately when 
//	encountered and return a motor to a state in which motion is allowed. Before 
//	enabling this functionality, be sure to understand this behavior and ensure 
//	your system will not enter an unsafe state. 
// To enable automatic alert handling, #define HANDLE_ALERTS (1)
// To disable automatic alert handling, #define HANDLE_ALERTS (0)
#define HANDLE_ALERTS (0)

// Declares user-defined helper functions.
// The definition/implementations of these functions are at the bottom of the sketch.
bool MoveDistance(int pulseNum);
void PrintAlerts();
void HandleAlerts();

void setup() {
    // Put your setup code here, it will run once:

    // To command for Pulse Burst Positioning, use the step and direction
    // interface with the acceleration and velocity limits set to their
    // maximum values. The ClearPath will then take the pulses and enforce
    // the motion profile constraints.

    // Sets all motor connectors into step and direction mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);

    // Set the motor's HLFB mode to bipolar PWM
    motor.HlfbMode(MotorDriver::HLFB_MODE_HAS_BIPOLAR_PWM);
    // Set the HFLB carrier frequency to 482 Hz
    motor.HlfbCarrier(MotorDriver::HLFB_CARRIER_482_HZ);

    // Sets the maximum velocity for each move
    motor.VelMax(INT32_MAX);

    // Set the maximum acceleration for each move
    motor.AccelMax(INT32_MAX);

    // Sets up serial communication and waits up to 5 seconds for a port to open.
    // Serial communication is not required for this example to run.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Enables the motor; homing will begin automatically if homing is enabled
    // in MSP.
    motor.EnableRequest(true);
    Serial.println("Motor Enabled");

    // Waits for HLFB to assert (waits for homing to complete if applicable)
    Serial.println("Waiting for HLFB...");
    while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED &&
			!motor.StatusReg().bit.AlertsPresent) {
        continue;
    }
	// Check if motor alert occurred during enabling
	// Clear alert if configured to do so 
    if (motor.StatusReg().bit.AlertsPresent) {
		Serial.println("Motor alert detected.");		
		PrintAlerts();
		if(HANDLE_ALERTS){
			HandleAlerts();
		} else {
			Serial.println("Enable automatic alert handling by setting HANDLE_ALERTS to 1.");
		}
		Serial.println("Enabling may not have completed as expected. Proceed with caution.");		
 		Serial.println();
	} else {
		Serial.println("Motor Ready");	
	}
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Move 6400 counts (positive direction) then wait 1000ms
    MoveDistance(6400);
    delay(1000);
    // Move 19200 counts farther positive, then wait 1000ms
    MoveDistance(19200);
    delay(1000);

    // Generates a trigger pulse on the enable line so the next move uses the
    // Alt Speed Limit.
    motor.EnableTriggerPulse(1, TRIGGER_PULSE_TIME, true);

    // Move back 12800 counts (negative direction), then wait 1000ms
    MoveDistance(-12800);
    delay(1000);
    // Move back 6400 counts (negative direction), then wait 1000ms
    MoveDistance(-6400);
    delay(1000);
    // Move back to the start (negative 6400 pulses), then wait 1000ms
    MoveDistance(-6400);
    delay(1000);
}

/*------------------------------------------------------------------------------
 * MoveDistance
 *
 *    Command "distance" number of step pulses away from the current position
 *    Prints the move status to the USB serial port
 *    Returns when HLFB asserts (indicating the motor has reached the commanded
 *    position)
 *
 * Parameters:
 *    int distance  - The distance, in step pulses, to move
 *
 * Returns: True/False depending on whether the move was successfully triggered.
 */
bool MoveDistance(int distance) {
    // Check if a motor alert is currently preventing motion
	// Clear alert if configured to do so 
    if (motor.StatusReg().bit.AlertsPresent) {
		Serial.println("Motor alert detected.");		
		PrintAlerts();
		if(HANDLE_ALERTS){
			HandleAlerts();
		} else {
			Serial.println("Enable automatic alert handling by setting HANDLE_ALERTS to 1.");
		}
		Serial.println("Move canceled.");		
		Serial.println();
        return false;
    }

    Serial.print("Commanding ");
    Serial.print(distance);
    Serial.println(" pulses");

    // Command the move of incremental distance
    motor.Move(distance);

    // Add a short delay to allow HLFB to update
    delay(2);

    // Waits for HLFB to assert (signaling the move has successfully completed)
    Serial.println("Moving.. Waiting for HLFB");
    while ( (!motor.StepsComplete() || motor.HlfbState() != MotorDriver::HLFB_ASSERTED) &&
			!motor.StatusReg().bit.AlertsPresent) {
        continue;
    }
	// Check if motor alert occurred during move
	// Clear alert if configured to do so 
    if (motor.StatusReg().bit.AlertsPresent) {
		Serial.println("Motor alert detected.");		
		PrintAlerts();
		if(HANDLE_ALERTS){
			HandleAlerts();
		} else {
			Serial.println("Enable automatic fault handling by setting HANDLE_ALERTS to 1.");
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
 * PrintAlerts
 *
 *    Prints active alerts.
 *
 * Parameters:
 *    requires "motor" to be defined as a ClearCore motor connector
 *
 * Returns: 
 *    none
 */
 void PrintAlerts(){
	// report status of alerts
 	Serial.println("Alerts present: ");
	if(motor.AlertReg().bit.MotionCanceledInAlert){
		Serial.println("    MotionCanceledInAlert "); }
	if(motor.AlertReg().bit.MotionCanceledPositiveLimit){
		Serial.println("    MotionCanceledPositiveLimit "); }
	if(motor.AlertReg().bit.MotionCanceledNegativeLimit){
		Serial.println("    MotionCanceledNegativeLimit "); }
	if(motor.AlertReg().bit.MotionCanceledSensorEStop){
		Serial.println("    MotionCanceledSensorEStop "); }
	if(motor.AlertReg().bit.MotionCanceledMotorDisabled){
		Serial.println("    MotionCanceledMotorDisabled "); }
	if(motor.AlertReg().bit.MotorFaulted){
		Serial.println("    MotorFaulted ");
	}
 }
//------------------------------------------------------------------------------


/*------------------------------------------------------------------------------
 * HandleAlerts
 *
 *    Clears alerts, including motor faults. 
 *    Faults are cleared by cycling enable to the motor.
 *    Alerts are cleared by clearing the ClearCore alert register directly.
 *
 * Parameters:
 *    requires "motor" to be defined as a ClearCore motor connector
 *
 * Returns: 
 *    none
 */
 void HandleAlerts(){
	if(motor.AlertReg().bit.MotorFaulted){
		// if a motor fault is present, clear it by cycling enable
		Serial.println("Faults present. Cycling enable signal to motor to clear faults.");
		motor.EnableRequest(false);
		Delay_ms(3*TRIGGER_PULSE_TIME);
		motor.EnableRequest(true);
	}
	// clear alerts
	Serial.println("Clearing alerts.");
	motor.ClearAlerts();
 }
//------------------------------------------------------------------------------

 
