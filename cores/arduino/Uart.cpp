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

#include "Uart.h"
#include "Arduino.h"
#include "SerialDriver.h"
#include "SysConnectors.h"
#include "SysManager.h"
#include "Common.h"

// Board resource controller
namespace ClearCore {
extern SysManager SysMgr;
}

Uart::Uart(ClearCorePins connectorPin, bool isUsbCon) {
    m_serial = nullptr;
    m_serialConnector = nullptr;

    ClearCore::Connector *con =
        ClearCore::SysMgr.ConnectorByIndex(connectorPin);
    if (isUsbCon) {
        ClearCore::SerialUsb *serialUsb =
            static_cast<ClearCore::SerialUsb *>(con);
        m_serial = static_cast<ClearCore::ISerial *>(serialUsb);
    }
    else {
        m_serialConnector = static_cast<ClearCore::SerialDriver *>(con);
        m_serial = static_cast<ClearCore::ISerial *>(m_serialConnector);
    }
}

void Uart::begin(unsigned long baudrate) {
    if (m_serial) {
        m_serial->PortClose();
        m_serial->Speed(baudrate);
        m_serial->PortOpen();
    }
}

void Uart::begin(unsigned long baudrate, uint16_t config) {
    if (m_serial) {
        m_serial->PortClose();
        m_serial->Speed(baudrate);
        m_serial->CharSize(extractCharSize(config));
        m_serial->Parity(extractParity(config));
        m_serial->PortOpen();
        m_serial->StopBits(extractNbStopBit(config));
    }
}

void Uart::end() {
    m_serial->PortClose();
}

void Uart::flush() {
    m_serial->WaitForTransmitIdle();
    m_serial->Flush();
}

void Uart::flushInput() {
    m_serial->FlushInput();
}

int Uart::available() {
    return m_serial ? m_serial->AvailableForRead() : 0;
}

int Uart::availableForWrite() {
    return m_serial ? m_serial->AvailableForWrite() : 0;
}

int Uart::peek() {
    return m_serial ? m_serial->CharPeek() : -1;
}

int Uart::read() {
    return m_serial ? m_serial->CharGet() : -1;
}

size_t Uart::write(const uint8_t data) {
    if (m_serial) {
        m_serial->SendChar(data);
        // The serial connector returns nothing, but return that one byte
        // was written to conform to the arduino standard for write()
        return 1;
    }

    return 0;
}

uint8_t Uart::extractNbStopBit(uint16_t config) {
    switch (config & SERIAL_STOP_BIT_MASK) {
        case SERIAL_STOP_BIT_1:
        default:
            return 1;

        case SERIAL_STOP_BIT_2:
            return 2;
    }
}

uint8_t Uart::extractCharSize(uint16_t config) {
    switch (config & SERIAL_DATA_MASK) {
        case SERIAL_DATA_5:
            return 5;

        case SERIAL_DATA_6:
            return 6;

        case SERIAL_DATA_7:
            return 7;

        case SERIAL_DATA_8:
        default:
            return 8;
    }
}

ClearCore::ISerial::Parities Uart::extractParity(uint16_t config) {
    switch (config & SERIAL_PARITY_MASK) {
        case SERIAL_PARITY_NONE:
        default:
            return ClearCore::ISerial::Parities::PARITY_N;

        case SERIAL_PARITY_EVEN:
            return ClearCore::ISerial::Parities::PARITY_E;

        case SERIAL_PARITY_ODD:
            return ClearCore::ISerial::Parities::PARITY_O;
    }
}

// Get or set TTL on the Serial port
void Uart::ttl(bool newState) {
    if (m_serialConnector) {
        if (newState) {
            m_serialConnector->Mode(ClearCore::SerialDriver::TTL);
        }
        else {
            m_serialConnector->Mode(ClearCore::SerialDriver::RS232);
        }
    }
}
bool Uart::ttl() {
    if (m_serialConnector) {
        return (m_serialConnector->Mode() == ClearCore::SerialDriver::TTL);
    }
    return false;
}

Uart::operator bool() {
    if (m_serial) {
        return (bool) *m_serial;
    }

    return false;
}

// Serial Constants for Arduino users
Uart Serial(CLEARCORE_PIN_USB, true);
Uart Serial0(CLEARCORE_PIN_COM0);
Uart Serial1(CLEARCORE_PIN_COM1);

// Weak definitions for the serial event runner and handlers
// These can be overridden by the user's functions if needed
void serialEventRun(void) {
    if (serialEvent && Serial.available()) {
        serialEvent();
    }
    if (serialEvent0 && Serial0.available()) {
        serialEvent0();
    }
    if (serialEvent1 && Serial1.available()) {
        serialEvent1();
    }
}
