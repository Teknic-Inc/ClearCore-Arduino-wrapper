/*
    Copyright (c) 2015 Arduino LLC.  All right reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Arduino.h>
#include <sam.h>
#include "Reset.h" // for tickReset()
#include "SysTiming.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Tick Counter united by ms */
static volatile uint32_t _ulTickCount = 0;

unsigned long millis(void) {
    return Milliseconds();
}

unsigned long micros(void) {
    return Microseconds();
}

void delay(unsigned long ms) {
    Delay_ms(ms);
}

void delayMicroseconds(unsigned int usec) {
    Delay_us(usec);
}

void SysTick_DefaultHandler(void) {
    // Increment tick count each ms
    _ulTickCount++;
    // See if USB Serial or others have caused a reset to bootloader cycle
    tickReset();
}

#ifdef __cplusplus
}
#endif
