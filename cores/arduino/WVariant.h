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

#pragma once

#include <stdint.h>
#include "sam.h"
#include "Common.h"
#include "pins_arduino.h"
#include "SysConnectors.h"
#include "SysManager.h"



#ifdef __cplusplus
extern "C" {
#endif

#define LED_BUILTIN CLEARCORE_PIN_LED

/**
  PIN Modes
**/
typedef enum {
    CLEARCORE_MODE_INPUT = INPUT, // Match standard name
    CLEARCORE_MODE_OUTPUT = OUTPUT,
    CLEARCORE_MODE_ANALOG
} ClearCorePinModes;

// Parallel version of pinMode to setup ClearCore "connectors" using the
// connector index.
void pinModeClearCore(ClearCorePins thePin, PinMode ulMode);

// Mapping of ClearCore pins to interrupts
pin_size_t digitalPinToInterrupt(pin_size_t pin);

#ifdef __cplusplus
} // extern "C"
#endif