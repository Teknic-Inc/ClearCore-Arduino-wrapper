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
#include "Arduino.h"
#include "DigitalInOutHBridge.h"
#include "SysConnectors.h"
#include "SysManager.h"
#include "WVariant.h"
#include "variant.h"
#include <Common.h>
#include <sam.h>

namespace ClearCore {
extern SysManager SysMgr;
}

// Board resource controller
extern const uint32_t SystemCoreClock;
uint32_t toneMaxFrequency = SystemCoreClock / 2;

void noTone(uint8_t outputPin) {
    ClearCore::Connector *myConnector =
        ClearCore::SysMgr.ConnectorByIndex(static_cast<ClearCorePins>(outputPin));
    // Only playing tones on the H-Bridge connectors
    if (myConnector->Type() != ClearCore::Connector::H_BRIDGE_TYPE) {
        return;
    }

    if (static_cast<ClearCore::DigitalInOutHBridge *>(myConnector)->ToneActiveState()
            != ClearCore::DigitalInOutHBridge::ToneState::TONE_TIMED) {
        // Stop the tone on the specified pin
        static_cast<ClearCore::DigitalInOutHBridge *>(myConnector)->ToneStop();
    }
}

void tone(
    unsigned char outputPin, unsigned int frequency, unsigned long duration) {

    ClearCore::Connector *myConnector =
        ClearCore::SysMgr.ConnectorByIndex(static_cast<ClearCorePins>(outputPin));
    // Only playing tones on the H-Bridge connectors
    if (myConnector->Type() != ClearCore::Connector::H_BRIDGE_TYPE) {
        return;
    }

    // Put the pin in tone mode
    static_cast<ClearCore::DigitalInOutHBridge *>(myConnector)
        ->Mode(ClearCore::DigitalInOutHBridge::OUTPUT_TONE);

    static_cast<ClearCore::DigitalInOutHBridge *>(myConnector)
        ->ToneTimed(frequency, duration);
}

void toneVolume(unsigned char outputPin, float volume) {

    ClearCore::Connector *myConnector =
        ClearCore::SysMgr.ConnectorByIndex(static_cast<ClearCorePins>(outputPin));
    // Only playing tones on the H-Bridge connectors
    if (myConnector->Type() != ClearCore::Connector::H_BRIDGE_TYPE) {
        return;
    }

    // Set the tone amplitude for this pin
    static_cast<ClearCore::DigitalInOutHBridge *>(myConnector)
        ->ToneAmplitude(static_cast<int16_t>(volume * INT16_MAX));
}