/*
 * Title: InputInterrupts
 *
 * Objective:
 *    This example demonstrates how to configure a digital interrupt on a
 *    ClearCore input.
 *
 * Description:
 *    This example sets up and attaches a callback function to be triggered by
 *    a digital interrupt. Interrupts are useful when a function needs to be
 *    called asynchronously from the rest of the code's execution.
 *
 *    This example's interrupt blinks the on-board user LED when the interrupt
 *    pin goes from on to off (aka "falling"). You may notice multiple blinks
 *    depending on how much bounce the input device has.
 *
 *    The interrupt callback function's ability to run is turned on and off
 *    periodically by this example. The callback function can only run when
 *    interrupts are turned "on", regardless of the interrupt pin state. If the
 *    interrupt pin is triggered while interrupts are "off", the callback will
 *    execute when interrupts are next turned on. This on/off state is printed
 *    to the USB serial port.
 *
 * Requirements:
 * ** A digital signal source, such as a switch or sensor, connected to DI-6 to
 *    trigger the interrupt
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

// Pins that support digital interrupts are:
// DI-6, DI-7, DI-8, A-9, A-10, A-11, A-12
#define interruptPin DI6

// Declare the signature for our interrupt service routine (ISR). The function
// is defined below
void MyCallback();

void setup() {
    // Put your setup code here, it will run once:

    // Set up the interrupt pin in digital input mode.
    pinMode(interruptPin, INPUT);

    // Set an ISR to be called when the state of the interrupt pin goes from
    // true to false.
    attachInterrupt(digitalPinToInterrupt(interruptPin), MyCallback, FALLING);

    // Set up serial communication and wait up to 5 seconds for a port to open
    // Serial communication is not required for this example to run
    Serial.begin(9600);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Enable digital interrupts.
    interrupts();

    Serial.println("Interrupts are turned on.");

    // Test that the ISR is triggered when the state of the interrupt pin
    // transitions from true to false by toggling your switch.

    // Wait while the interrupt is triggered.
    delay(5000);

    // Disable digital interrupts.
    noInterrupts();

    Serial.println("Interrupts are turned off.");

    // Test that the ISR does not get triggered when the state of the interrupt
    // pin transitions from true to false by toggling your switch.
    delay(5000);
}

// The function to be triggered on an interrupt.
// This function blinks the user-controlled LED once.
/*------------------------------------------------------------------------------
 * MyCallback
 *
 *    Flashes the ClearCore's built-in LED (next to the USB port) on and off.
 *
 * Parameters:
 *    None
 *
 * Returns: None
 */
void MyCallback() {
    digitalWrite(LED_BUILTIN, true);
    delay(100);
    digitalWrite(LED_BUILTIN, false);
    delay(100);
}
//------------------------------------------------------------------------------
