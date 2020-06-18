/*
 * Title: SpiComPortInitialization
 *
 * Objective:
 *    This example demonstrates how to configure a COM port for use with an
 *    SPI device.
 *
 * Description:
 *    This example will explain the basic configuration settings of an SPI
 *    device then perform a brief transaction with the SPI device connected to
 *    COM-0.
 *
 * Requirements:
 * ** An SPI device connected to COM-0.
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
#include <SPI.h>

// Select the baud rate to match the target device.
#define baudRate 80000

// Select the clock polarity to match the target device. The clock polarity
// setting indicates whether the device expects a low signal when idle or a
// high signal when idle. It also indicates whether the leading or trailing
// edge of the clock cycle are rising or falling edges.
// Selecting SCK_LOW indicates that SCK is low when idle and the leading edge
// of the clock cycle is a rising edge while the trailing edge is a falling
// edge.
// Selecting SCK_HIGH indicates that SCK is high when idle and the leading edge
// of the clock cycle is a falling edge while the trailing edge is a rising
// edge.
// The default value for a COM connector's clock polarity is SCK_LOW.
#define clockPolarity SerialDriver::SCK_LOW

// Select the clock phase setting to match the target device. The clock phase
// setting indicates whether data is sampled or changed on the leading or
// trailing edge in the clock cycle.
// Selecting LEAD_SAMPLE indicates that data is sampled on the leading edge and
// changed on the trailing edge.
// Selecting LEAD_CHANGE indicates that data is sampled on the trailing edge
// and changed on the leading edge.
// The default value for a COM connector's clock phase is LEAD_CHANGE.
#define clockPhase SerialDriver::LEAD_CHANGE

void setup() {
    // Put your setup code here, it will run once:

    // With the user settings, decide which SPI mode is needed
    int spiMode;
    if (clockPolarity == SerialDriver::SCK_LOW) {
        if (clockPhase == SerialDriver::LEAD_SAMPLE) {
            spiMode = SPI_MODE0;
        }
        else {
            spiMode = SPI_MODE1;
        }
    }
    else {
        if (clockPhase == SerialDriver::LEAD_SAMPLE) {
            spiMode = SPI_MODE2;
        }
        else {
            spiMode = SPI_MODE3;
        }
    }

    // Create the settings object to be passed when beginning SPI communications
    SPISettings spiConfig(baudRate, MSBFIRST, spiMode);

    // Open the SPI port on ConnectorCOM0
    SPI.begin();

    // Output some arbitrary sample data to the SPI device. This data is not
    // required for set up, just to demonstrate the transfer process.
    SPI.beginTransaction(spiConfig);
    SPI.transfer('a');
    SPI.transfer(98);
    SPI.transfer(0x63);
    SPI.endTransaction();
}

void loop() {
    // This example doesn't have any ongoing tasks to perform.
}