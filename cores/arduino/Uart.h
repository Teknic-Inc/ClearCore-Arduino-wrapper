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

#include "SerialDriver.h"
#include "SerialUsb.h"
#include "SysConnectors.h"
#include "SysManager.h"
#include "HardwareSerial.h"
#include "SERCOM.h"

class Uart : public HardwareSerial {
public:
    Uart(ClearCorePins connectorPin, bool isUsbCon = false);
    void begin(unsigned long baudRate);
    void begin(unsigned long baudrate, uint16_t config);
    void end();
    int available();
    int availableForWrite();
    int peek();
    int read();
    void flush();
    void flushInput();
    size_t write(const uint8_t data);
    void ttl(bool newState);
    bool ttl();
    using Print::write; // pull in write(str) and write(buf, size) from Print

    operator bool();

private:

    ClearCore::ISerial *m_serial;
    ClearCore::SerialDriver *m_serialConnector;

    uint8_t extractNbStopBit(uint16_t config);
    uint8_t extractCharSize(uint16_t config);
    ClearCore::ISerial::Parities extractParity(uint16_t config);
};


extern Uart Serial;
extern Uart Serial0;
extern Uart Serial1;