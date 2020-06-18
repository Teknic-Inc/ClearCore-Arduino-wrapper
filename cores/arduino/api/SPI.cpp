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

#include "SPI.h"
#include "ISerial.h"
#include "SerialBase.h"
#include "SerialDriver.h"
#include "SdCardDriver.h"

using ClearCore::Connector;
using ClearCore::SerialBase;
using ClearCore::SerialDriver;
using ClearCore::ISerial;

namespace ClearCore {
extern SdCardDriver SdCard;
extern SerialDriver ConnectorCOM0;
extern SerialDriver ConnectorCOM1;
}

SPIClass::SPIClass(SerialBase &thePort, bool isCom)
    : m_serial(&thePort),
      m_isCom(isCom),
      m_settings() {
}

void SPIClass::begin() {
    if (m_isCom) {
        ISerial *asISerial = dynamic_cast<ISerial *>(m_serial);
        SerialDriver *serialDriver = static_cast<SerialDriver *>(asISerial);
        serialDriver->Mode(Connector::ConnectorModes::SPI);
    }
    else {
        m_serial->PortMode(SerialBase::PortModes::SPI);
    }
    m_serial->SpiSsMode(SerialBase::CtrlLineModes::LINE_OFF);
    config();
    m_serial->PortOpen();
}

void SPIClass::config() {
    m_serial->Speed(m_settings.m_clockFreq);
    switch (m_settings.m_dataMode) {
        case SPI_MODE0:
            m_serial->SpiClock(SerialBase::SpiClockPolarities::SCK_LOW,
                               SerialBase::SpiClockPhases::LEAD_SAMPLE);
            break;
        case SPI_MODE1:
            m_serial->SpiClock(SerialBase::SpiClockPolarities::SCK_LOW,
                               SerialBase::SpiClockPhases::LEAD_CHANGE);
            break;
        case SPI_MODE2:
            m_serial->SpiClock(SerialBase::SpiClockPolarities::SCK_HIGH,
                               SerialBase::SpiClockPhases::LEAD_SAMPLE);
            break;
        case SPI_MODE3:
            m_serial->SpiClock(SerialBase::SpiClockPolarities::SCK_HIGH,
                               SerialBase::SpiClockPhases::LEAD_CHANGE);
            break;
        default:
            break;
    }
}

void SPIClass::end() {
    m_serial->PortClose();
}

void SPIClass::usingInterrupt(int interruptNumber) {
    (void)interruptNumber;
}

void SPIClass::notUsingInterrupt(int interruptNumber) {
    (void)interruptNumber;
}

void SPIClass::beginTransaction(SPISettings settings) {
    m_settings.m_bitOrder = settings.m_bitOrder;
    m_settings.m_dataMode = settings.m_dataMode;
    m_settings.m_clockFreq = settings.m_clockFreq;
    config();
    m_serial->SpiSsMode(SerialBase::CtrlLineModes::LINE_ON);
}

void SPIClass::endTransaction(void) {
    m_serial->SpiSsMode(SerialBase::CtrlLineModes::LINE_OFF);
}

void SPIClass::setBitOrder(BitOrder order) {
    m_settings.m_bitOrder = order;
    config();
}

void SPIClass::setDataMode(uint8_t mode) {
    m_settings.m_dataMode = mode;
    config();
}

void SPIClass::setClockDivider(uint8_t div) {
    m_settings.m_clockFreq = MAX_SPI / div;
    config();
}

byte SPIClass::transfer(uint8_t data) {
    return m_serial->SpiTransferData(data);
}

uint16_t SPIClass::transfer16(uint16_t data) {
    union {
        uint16_t val;
        struct {
            uint8_t lsb;
            uint8_t msb;
        };
    } t;

    t.val = data;

    if (m_settings.m_bitOrder == LSBFIRST) {
        t.lsb = transfer(t.lsb);
        t.msb = transfer(t.msb);
    }
    else {
        t.msb = transfer(t.msb);
        t.lsb = transfer(t.lsb);
    }

    return t.val;
}

void SPIClass::transfer(void *buf, size_t count) {
    m_serial->SpiTransferData((uint8_t *)buf, (uint8_t *)buf, count);
}

void SPIClass::transfer(const void *txbuf, void *rxbuf, size_t count,
                        bool block) {
    if (!block &&
            m_serial->SpiTransferDataAsync((uint8_t *)txbuf, (uint8_t *)rxbuf, count)) {
        return;
    }
    m_serial->SpiTransferData((uint8_t *)txbuf, (uint8_t *)rxbuf, count);
}

// Waits for a prior in-background DMA transfer to complete.
void SPIClass::waitForTransfer(void) {
    m_serial->SpiAsyncWaitComplete();
}

void SPIClass::attachInterrupt() {
    // Should be enableInterrupt()
}

void SPIClass::detachInterrupt() {
    // Should be disableInterrupt()
}


SPIClass SPI(ClearCore::ConnectorCOM0, true);
SPIClass SPI1(ClearCore::ConnectorCOM1, true);
SPIClass SPI2(ClearCore::SdCard, false);

