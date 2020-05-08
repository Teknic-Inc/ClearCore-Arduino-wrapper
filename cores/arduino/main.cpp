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

#define ARDUINO_MAIN
#include "SysManager.h"
#include "SerialDriver.h"
#include "SerialUsb.h"
#include "SysTiming.h"
#include "sam.h"
#include <Arduino.h>

// Board resource controller
namespace ClearCore {
extern SysManager SysMgr;
extern SerialUsb ConnectorUsb;
extern SerialDriver ConnectorCOM0;
extern SerialDriver ConnectorCOM1;
}

/*
 * \brief Main entry point of Arduino application
 */
int main(void) {
    // - - - - - - - - - - - - - - - - - - - - - -
    // Perform C++ initializations before sketch
    // - - - - - - - - - - - - - - - - - - - - - -
    delay(1);
    // Start sketch "setup"
    setup();

    for (;;) {
        // sketch "loop"
        loop();
        if (serialEventRun) {
            serialEventRun();
        }
    }

    return 0;
}

/* Default Arduino systick handler */
extern "C" int sysTickHook(void);
extern "C" void SysTick_DefaultHandler(void);

extern "C" void SysTick_Handler(void) __attribute__((weak));
extern "C" void SysTick_Handler(void) {
    ClearCore::SysMgr.SysTickUpdate();
    if (sysTickHook()) {
        return;
    }
    SysTick_DefaultHandler();
}