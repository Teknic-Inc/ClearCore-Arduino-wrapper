/*
 * Title: WriteCCIODigitalOutputPulses
 *
 * Objective:
 *    This example demonstrates how to initialize a CCIO-8 Expansion Board and
 *    write digital pulses to its outputs.
 *
 * Description:
 *    This example sets up COM-0 to control a CCIO-8 Expansion Board then
 *    writes a series of digital pulses to the defined connector.
 *
 * Requirements:
 * ** A CCIO-8 Expansion Board powered and connected to COM-0
 * ** An output such as an LED connected to defined connector (CCIO-0)
 *      Note: You can leave the I/O point disconnected and still see the
 *      built-in I/O LED toggle with the connector state.
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

// Specify which ClearCore serial COM port is connected to the COM IN port
// of the CCIO-8 board: ConnectorCOM0 or ConnectorCOM1.
#define CcioPort ConnectorCOM0

// Specify which output pin to write digital output pulses.
// All connectors on the ClearCore I/O Expansion Board are capable of
// pulsing the output.
#define OutputPin CLEARCORE_PIN_CCIOA0

void setup() {
    // Put your setup code here, it will run once:

    // Set up the CCIO-8 COM port.
    CcioPort.Mode(Connector::CCIO);
    CcioPort.PortOpen();

    // Set up the output connector in output mode.
    pinMode(OutputPin, OUTPUT);
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Generate a 100ms on/100ms off pulse that runs until
    // the stop function is called.
    OutputPulsesStart(OutputPin, 100, 100);
    delay(1000);

    // Stop any further pulses on the pin. The second argument controls how
    // output pulses are stopped. If true, the output pulses will be stopped
    // immediately (this is the default behavior); if false, the active pulse
    // cycle will be completed first.
    OutputPulsesStop(OutputPin, true);

    // Generate a 250ms on/50ms off pulse that continues until
    // 20 on/off cycles are complete or until the stop function
    // is called.
    OutputPulsesStart(OutputPin, 250, 50, 20);
    delay(6000);

    // Pulses should be complete, but call stop to be safe.
    OutputPulsesStop(OutputPin, true);

    // Generate a 300ms on/500ms off pulse that runs for 5
    // complete cycles. With the final true argument, the sketch will pause here
    // until all the pulse cycles are complete.
    OutputPulsesStart(OutputPin, 300, 500, 5, true);
    delay(3000);
}