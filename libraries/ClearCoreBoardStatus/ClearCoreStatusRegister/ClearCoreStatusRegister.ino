/*
 * Title: ClearCoreStatusRegister
 *
 * Objective:
 *    This example demonstrates how to read and display bits in the ClearCore
 *    Status Register.
 *
 * Description:
 *    This example gets a snapshot of the ClearCore's real-time status register
 *    and prints the state of the status register bits to the USB serial port.
 *
 * Requirements:
 * ** None
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

void setup() {
    // Put your setup code here, it will run once:

    // Set up serial communication at a baud rate of 9600 bps then wait up to
    // 5 seconds for a port to open.
    // Serial communication is not required for this example to run, however the
    // example will appear to do nothing without serial output.
    Serial.begin(9600);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Get a copy of the real-time status register.
    StatusManager::StatusRegister statusReg = StatusMgr.StatusRT();

    Serial.println("Status Register:");

    Serial.print("Vsupply over-voltage:\t\t");
    if (statusReg.bit.VSupplyOverVoltage) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("Vsupply under-voltage:\t\t");
    if (statusReg.bit.VSupplyUnderVoltage) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("H-Bridge output overloaded:\t");
    if (statusReg.bit.HBridgeOverloaded) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("H-Bridge resetting:\t\t");
    if (statusReg.bit.HBridgeReset) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    // This status bit denotes the state of the 5 volt supply for off-board
    // items
    Serial.print("Offboard 5V overloaded:\t\t");
    if (statusReg.bit.Overloaded5V) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("Output overloaded:\t\t");
    if (statusReg.bit.OutputOverloaded) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("CCIO-8 output overloaded:\t");
    if (statusReg.bit.CcioOverloaded) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("CCIO-8 link broken:\t\t");
    if (statusReg.bit.CcioLinkBroken) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("ADC in timeout:\t\t\t");
    if (statusReg.bit.AdcTimeout) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("Ethernet disconnect:\t\t");
    if (statusReg.bit.EthernetDisconnect) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("Ethernet remote fault:\t\t");
    if (statusReg.bit.EthernetRemoteFault) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.print("SD card error:\t\t\t");
    if (statusReg.bit.SdCardError) {
        Serial.println('1');
    }
    else {
        Serial.println('0');
    }

    Serial.println("------------------------");

    // Wait a couple seconds then repeat...
    delay(2000);
}