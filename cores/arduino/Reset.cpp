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
#include "Reset.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NVM_MEMORY ((volatile uint16_t *)0x000000)

#if (ARDUINO_SAMD_VARIANT_COMPLIANCE >= 10610)

extern const uint32_t __text_start__;
#define APP_START ((volatile uint32_t)(&__text_start__) + 4)

#else
#define APP_START 0x00002004
#endif

static inline bool nvmReady(void) {
    return NVMCTRL->STATUS.bit.READY;
}

__attribute__((long_call, section(".ramfunc")))
static void banzai() {
    // Disable all interrupts
    __disable_irq();

    // Get a copy of the NVM "user page"
    uint32_t userspace = *((uint32_t *)NVMCTRL_FUSES_BOOTPROT_ADDR);
    uint32_t bootsize = NVMCTRL_BLOCK_SIZE *
                        (15 - ((userspace & NVMCTRL_FUSES_BOOTPROT_Msk)
                               >> NVMCTRL_FUSES_BOOTPROT_Pos));
    // Avoid erasing the application if APP_START is < than the minimum
    // bootloader sizea
    // This could happen if without_bootloader linker script was chosen
    if ((APP_START - 4) < bootsize) {
        goto reset;
    }

    // Erase application
    while (!nvmReady()) {
        continue;
    }
    NVMCTRL->STATUS.reg |= NVMCTRL_STATUS_MASK;
    // Set address to starting address
    NVMCTRL->ADDR.reg = (APP_START - 4);
    NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_EB | NVMCTRL_CTRLB_CMDEX_KEY;
    while (!nvmReady()) {
        continue;
    }
reset:
    // Reset the device
    NVIC_SystemReset() ;

    while (true) {
        continue;
    }
}

static int ticks = -1;

void initiateReset(int _ticks) {
    ticks = _ticks;
}

void cancelReset() {
    ticks = -1;
}

void tickReset() {
    if (ticks == -1) {
        return;
    }
    ticks--;
    if (ticks == 0) {
        banzai();
    }
}

#ifdef __cplusplus
}
#endif
