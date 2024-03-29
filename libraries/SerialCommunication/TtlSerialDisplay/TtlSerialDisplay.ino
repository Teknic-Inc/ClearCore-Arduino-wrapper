/*
 * Title: TtlSerialDisplay
 *
 * Objective:
 *    This example demonstrates how to write data to a TTL device.
 *
 * Description:
 *    This example will set up TTL communications on COM-0 then write various
 *    data to the device.
 *
 * Requirements:
 * ** A NHD-0420D3Z LCD display in TTL mode connected to COM-0.
 *    Datasheet: http://www.newhavendisplay.com/specs/NHD-0420D3Z-NSW-BBW-V3.pdf
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"

// Data bounds of the device
#define NUM_ROWS 4
#define NUM_COLUMNS 20

// Sample data to write to the display
const uint8_t line1[21] = "abcdefghijklmnopqrst";
const uint8_t line2[21] = "ABCDEFGHIJKLMNOPQRST";
const uint8_t line3[21] = "01234567890123456789";
const uint8_t line4[21] = "98765432109876543210";

// Declare our helper functions so that they may be used before they are defined
void SetBrightness(uint8_t level);
void SetCursor(uint8_t row, uint8_t column);

void setup() {
    // Put your setup code here, it will run once:

    Serial0.begin(57600);
    Serial0.ttl(true);

    // The COM port is now configured and ready to send commands and
    // data to the display

    // Set the display brightness level.
    // The maximum value for full brightness is 8.
    SetBrightness(4);

    // Set the cursor position to the top-left corner.
    SetCursor(0, 0);

    // Send the lines "out of order" (1, 3, 2, 4) to the display.
    // Without resetting the cursor position for each line, this is the order
    // in which lines must be sent to be displayed correctly.
    Serial0.print((char *)line1);
    Serial0.print((char *)line3);
    Serial0.print((char *)line2);
    Serial0.print((char *)line4);
}

void loop() {
    // This example doesn't have any ongoing tasks to perform.
}

/*------------------------------------------------------------------------------
 * SetBrightness
 *
 *    Sends a short group of data to control the brightness of the attached
 *    LCD screen. See the device's datasheet for a full set of commands and
 *    syntax.
 *
 * Parameters:
 *    uint8_t level  - The brightness level to be set
 *
 * Returns: None
 */
void SetBrightness(uint8_t level) {
    Serial0.write(0xfe);
    Serial0.write(0x53);
    Serial0.write(level);
}
//------------------------------------------------------------------------------

/*------------------------------------------------------------------------------
 * SetCursor
 *
 *    Sends a short group of data to control the position of the device's
 *    internal cursor that controls where characters are printed on the LCD
 *    screen. See the device's datasheet for a full set of commands and syntax.
 *
 * Parameters:
 *    uint8_t row  - The character row to move the cursor to.
 *    uint8_t column  - The character column to move the cursor to.
 *
 * Returns: None
 */
void SetCursor(uint8_t row, uint8_t column) {
    // Bounds-check the passed-in row and column
    if (row >= NUM_ROWS) {
        row = 0;
    }

    if (column >= NUM_COLUMNS) {
        column = 0;
    }

    uint8_t position = row * NUM_COLUMNS + column;
    Serial0.write(0xfe);
    Serial0.write(0x45);
    Serial0.write(position);
}
//------------------------------------------------------------------------------
