/*
 * SPI Master library for Teknic ClearCore.
 * Copyright (c) 2015 Arduino LLC
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>
#include "SerialBase.h"

// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 1

#define SPI_MODE0 0x00
#define SPI_MODE1 0x01
#define SPI_MODE2 0x02
#define SPI_MODE3 0x03

#define MAX_SPI 10000000
#define SPI_MIN_CLOCK_DIVIDER 1

class SPISettings {
public:
    SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode)
        : m_clockFreq(clock),
          m_bitOrder(bitOrder),
          m_dataMode(dataMode) {
    }

    // Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to
    // MSB first.
    SPISettings()
        : m_clockFreq(10000000),
          m_bitOrder(BitOrder::MSBFIRST),
          m_dataMode(SPI_MODE0) {
    }

private:
    uint32_t m_clockFreq;
    BitOrder m_bitOrder;
    uint8_t m_dataMode;

    friend class SPIClass;
};

class SPIClass {
public:
    SPIClass(ClearCore::SerialBase &thePort, bool isCom);

    byte transfer(uint8_t data);
    uint16_t transfer16(uint16_t data);
    void transfer(void *buf, size_t count);
    void transfer(const void *txbuf, void *rxbuf, size_t count,
                  bool block = true);
    void waitForTransfer(void);

    // Transaction Functions
    void usingInterrupt(int interruptNumber);
    void notUsingInterrupt(int interruptNumber);
    void beginTransaction(SPISettings settings);
    void endTransaction(void);

    // SPI Configuration methods
    void attachInterrupt();
    void detachInterrupt();

    void begin();
    void end();

    void setBitOrder(BitOrder order);
    void setDataMode(uint8_t uc_mode);
    void setClockDivider(uint8_t uc_div);

private:
    void config();

    ClearCore::SerialBase *m_serial;
    bool m_isCom;
    SPISettings m_settings;

};

extern SPIClass SPI;
extern SPIClass SPI1;
extern SPIClass SPI2;

// For compatibility with sketches designed for AVR @ 16 MHz
// New programs should use SPI.beginTransaction to set the SPI clock
#define SPI_CLOCK_DIV2   (MAX_SPI * 2 / 8000000)
#define SPI_CLOCK_DIV4   (MAX_SPI * 2 / 4000000)
#define SPI_CLOCK_DIV8   (MAX_SPI * 2 / 2000000)
#define SPI_CLOCK_DIV16  (MAX_SPI * 2 / 1000000)
#define SPI_CLOCK_DIV32  (MAX_SPI * 2 / 500000)
#define SPI_CLOCK_DIV64  (MAX_SPI * 2 / 250000)
#define SPI_CLOCK_DIV128 (MAX_SPI * 2 / 125000)

#endif
