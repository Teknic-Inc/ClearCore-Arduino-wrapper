/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include <sam.h>
#include <ArduinoAPI.h>

#define clockCyclesPerMicrosecond() (SystemCoreClock / 1000000L)
#define clockCyclesToMicroseconds(a) (((a) * 1000L) / (SystemCoreClock / 1000L))
#define microsecondsToClockCycles(a) ((a) * (SystemCoreClock / 1000000L))

#include "WVariant.h"
#include "delay.h"

typedef enum {
    AR_INTERNAL,
    AR_INTVCC0 = 2,     // 1/2 VDDANA
    AR_INTVCC1 = 3,     // VDDANA
    AR_EXTERNAL_A = 4,
    AR_EXTERNAL_B = 5,
    AR_EXTERNAL_C = 6,
} eAnalogReference ;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
void init(void);
void delay(unsigned long ms);
void setup(void);
void loop(void);
#ifdef __cplusplus
} // extern "C"
#endif

#endif // Arduino_h
