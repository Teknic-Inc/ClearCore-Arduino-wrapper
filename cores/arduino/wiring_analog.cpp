/*
    Copyright (c) 2014 Arduino LLC.  All right reserved.

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

#include <ArduinoAPI.h>
#include "Arduino.h"

void analogWrite(pin_size_t ulPin, uint32_t ulVal, AnalogOutMode mode,
                 AnalogOutputUnits units) {
    analogWriteAPI(ulPin, (int)ulVal, mode, units);
}

int analogRead(pin_size_t ulPin, AnalogInputUnits units) {
    return analogReadAPI(ulPin, units);
}