/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

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

#ifndef __CLEARCORE_VARIANT_H__
#define __CLEARCORE_VARIANT_H__

// The definitions here needs a SAMD core >=1.6.10
#define ARDUINO_SAMD_VARIANT_COMPLIANCE 10610
// Core clock frequency
#define VARIANT_MCK 120000000       // __SYSTEM_CLOCK


#include "WVariant.h"
#include "Common.h"

#ifdef __cplusplus
#include "SERCOM.h"
#include "Uart.h"
#endif

enum _ADC_CHANNELS {
    SDRVR3_IMON = PIN_PC00,
    SDRVR2_IMON = PIN_PC01,
    VSUPP_MON = PIN_PC02,
    V5VOB_MON = PIN_PB04,
    AIN09 = PIN_PB07,
    AIN10 = PIN_PB06,
    AIN11 = PIN_PB05,
    AIN12 = PIN_PC03
};


#endif //__CLEARCORE_VARIANT_H__

