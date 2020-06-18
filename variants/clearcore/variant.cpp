/*
  Copyright 2020, Teknic, Inc.

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


#include "variant.h"
#include <component/eic.h>
#include <delay.h>
#include <sam.h>
#include "AdcManager.h"
#include "DigitalInOutAnalogOut.h"
#include "DigitalInAnalogIn.h"
#include "DigitalInOut.h"
#include "DigitalIn.h"
#include "DigitalInOutHBridge.h"
#include "InputManager.h"
#include "LedDriver.h"
#include "MotorDriver.h"
#include "SysConnectors.h"
#include "SysManager.h"
#include "SysUtils.h"
#include "CcioBoardManager.h"
#include "ShiftRegister.h"
#include "MotorManager.h"

#define ADC ADC1

// Board resource controller
namespace ClearCore {
extern SysManager SysMgr;
extern AdcManager &AdcMgr;
extern InputManager &InputMgr;
}

static void *OutputPulsesSetup(pin_size_t pin);

// analogReadResolution(), mapResolution() from wiring_analog.c

void analogReadResolution(int res) {
    ClearCore::AdcMgr.AdcResolution(res);
}

static inline uint32_t mapResolution(uint32_t value,
    uint32_t from, uint32_t to) {
    if (from == to) {
        return value;
    }
    if (from > to) {
        return value >> (from - to);
    }
    return value << (to - from);
}

PinStatus digitalReadClearCore(pin_size_t conNum) {
    // check if trying to read a motor input
    if (conNum > CLEARCORE_PIN_MAX && conNum <= M3_INB) {
        int motorPin = -1;
        switch (conNum) {
            case M0_INA:
            case M0_INB:
                motorPin = M0;
                break;
            case M1_INA:
            case M1_INB:
                motorPin = M1;
                break;
            case M2_INA:
            case M2_INB:
                motorPin = M2;
                break;
            case M3_INA:
            case M3_INB:
                motorPin = M3;
                break;
        }
        
        // Get a reference to the appropriate motor
        ClearCore::MotorDriver *motor =
            static_cast<ClearCore::MotorDriver*>(
                ClearCore::SysMgr.ConnectorByIndex(
                    static_cast<ClearCorePins>(motorPin)));
                
        if (conNum > CLEARCORE_PIN_MAX && conNum < M0_INB) {
            return motor->MotorInAState() ? LOW : HIGH;
        } 
        else {
            return motor->MotorInBState() ? LOW : HIGH;
        }
    }
    
    // Get a reference to the appropriate connector
    ClearCore::Connector *connector =
        ClearCore::SysMgr.ConnectorByIndex(
            static_cast<ClearCorePins>(conNum));

    if (!connector) {
        return LOW;
    }

    // If connector is in an input mode, make sure it is in digital input
    if (!connector->IsWritable()) {
        connector->Mode(ClearCore::Connector::INPUT_DIGITAL);
    }

    return (PinStatus)connector->State();
}

void digitalWriteClearCore(pin_size_t conNum, PinStatus ulVal) {
    // check if trying to write to a motor input
    if (conNum > CLEARCORE_PIN_MAX && conNum <= M3_INB) {
        int motorPin = -1;
        switch (conNum) {
            case M0_INA:
            case M0_INB:
                motorPin = M0;
                break;
            case M1_INA:
            case M1_INB:
                motorPin = M1;
                break;
            case M2_INA:
            case M2_INB:
                motorPin = M2;
                break;
            case M3_INA:
            case M3_INB:
                motorPin = M3;
                break;
        }
        
        // Get a reference to the appropriate motor
        ClearCore::MotorDriver *motor =
            static_cast<ClearCore::MotorDriver*>(
                ClearCore::SysMgr.ConnectorByIndex(
                    static_cast<ClearCorePins>(motorPin)));

        if (conNum > CLEARCORE_PIN_MAX && conNum < M0_INB) {
            motor->MotorInAState(ulVal);
        }
        else {
            motor->MotorInBState(ulVal);
        }
    } 
    else {
        // Get a reference to the appropriate connector
        ClearCore::Connector *connector =
            ClearCore::SysMgr.ConnectorByIndex(
                static_cast<ClearCorePins>(conNum));

        // If connector cannot be written, just return
        if (!connector || !connector->IsWritable()) {
            return;
        }

        connector->Mode(ClearCore::Connector::OUTPUT_DIGITAL);
        if (connector->Mode() == ClearCore::Connector::OUTPUT_DIGITAL) {
            connector->State(ulVal);
        }
    }    
}

/**
    This function replaces pinModeAPI in most cases as ClearCore uses
    a connector model in place of the pin model of the traditional Arduino
    implementations.
**/
void pinModeClearCore(ClearCorePins conNum, PinMode ulMode) {
    // Get a reference to the appropriate connector
    ClearCore::Connector *connector =
        ClearCore::SysMgr.ConnectorByIndex(
            static_cast<ClearCorePins>(conNum));

    if (!connector) {
        return;
    }

    switch (ulMode) {
        case OUTPUT:
            connector->Mode(ClearCore::Connector::OUTPUT_DIGITAL);
            break;
        case INPUT:
            connector->Mode(ClearCore::Connector::INPUT_DIGITAL);
            break;
        case INPUT_PULLUP:
            connector->Mode(ClearCore::Connector::INPUT_DIGITAL);
            break;
        default:
            break;
    }
}

int analogReadAPI(pin_size_t pinNumber, AnalogInputUnits units) {
    // Get a reference to the appropriate connector
    ClearCore::Connector *connector =
        ClearCore::SysMgr.ConnectorByIndex(
            static_cast<ClearCorePins>(pinNumber));
    
    if (!connector) {
        return 0;
    }
    
    // Ensure the connector is in analog input mode
    connector->Mode(ClearCore::Connector::INPUT_ANALOG);
    if (connector->Mode() != ClearCore::Connector::INPUT_ANALOG) {
        // Analog input not supported on this connector
        return 0;
    }
    // Assume State() function returns 15-bit result
    int adcRawValue = connector->State();

    // Convert result to millivolts if applicable.
    if (units == MILLIVOLTS) {
        int valueMax;

        switch (ClearCore::AdcMgr.AdcResolution()) {
            case 8:
                valueMax = 255;
                break;
            case 10:
                valueMax = 1023;
                break;
            case 12:
                valueMax = 4095;
                break;
            case 16:
                valueMax = 65535;
                break;
            default:
                // Shouldn't get here, assume 12-bit default
                valueMax = 4095;
            break;
        }
        
        return int(round(1000 * 9.9 * adcRawValue / valueMax));
    }
    else {
        return adcRawValue;
    }
}

void analogWriteAPI(pin_size_t conNum, int value, AnalogOutMode mode,
                    AnalogOutputUnits units) {
    // Get a reference to the appropriate connector
    ClearCore::Connector *connector =
        ClearCore::SysMgr.ConnectorByIndex(
            static_cast<ClearCorePins>(conNum));

    // If connector cannot be written, just return
    if (!connector || !connector->IsWritable()) {
        return;
    }

    if (mode == PWM && units == MICROAMPS) {
        return; // Invalid combination of mode/units.
    }

    // Convert to raw DAC value if specified in microamps
    if (units == MICROAMPS) {
        // First constrain the value to be in the 0-20 mA range.
        value = constrain(value, 0, 20000);
        value = int(round(value * 4095.0 / 20000.0));
    }

    switch (mode) {
        case PWM:
            switch (connector->Type()) {
                case ClearCore::Connector::ANALOG_OUT_DIGITAL_IN_OUT_TYPE:
                case ClearCore::Connector::DIGITAL_IN_OUT_TYPE:
                case ClearCore::Connector::H_BRIDGE_TYPE:
                    connector->Mode(ClearCore::Connector::OUTPUT_PWM);
                    connector->State(value);
                    break;
                default:
                    break;
            }
            break;
        case CURRENT:
            switch (connector->Type()) {
                case ClearCore::Connector::ANALOG_OUT_DIGITAL_IN_OUT_TYPE:
                    connector->Mode(ClearCore::Connector::OUTPUT_ANALOG);
                    connector->State(value);
                    break;
                default:
                    // unsupported connector type
                    break;
            }
            break;
    } // switch (mode)
}

/**
    This function is the implementation of the public facing pinMode. The pin 
    number in this case refers to a "connector" object in the global Connector
    connector array.
**/
void pinMode(pin_size_t pinNumber, uint32_t ulMode) {
    pinModeClearCore((ClearCorePins)pinNumber, (PinMode)ulMode);
}

pin_size_t digitalPinToInterrupt(pin_size_t pinNumber) {
    // assert pin number is within valid range
    ClearCorePins ccPin = static_cast<ClearCorePins>(pinNumber);

    if (ccPin < CLEARCORE_PIN_DI6 || ccPin > CLEARCORE_PIN_A12) {
        return 255;
    }
    
    return ClearCore::SysMgr.ConnectorByIndex(ccPin)->ExternalInterrupt();
}

void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback,
                     PinStatus mode) {
    if (interruptNumber >= EIC_NUMBER_OF_INTERRUPTS) {
        return;
    }

    // If attaching an interrupt to an analog input connector, make sure to 
    // set up the connector in digital input mode.
    ClearCore::Connector *analogInput;
    for (int32_t pin = CLEARCORE_PIN_A9; pin <= CLEARCORE_PIN_A12; pin++) {
        analogInput = ClearCore::SysMgr.ConnectorByIndex((ClearCorePins)pin);

        if (interruptNumber == analogInput->ExternalInterrupt()) {
            if (analogInput->Mode() != ClearCore::Connector::INPUT_DIGITAL) {
                analogInput->Mode(ClearCore::Connector::INPUT_DIGITAL);
            }
            break;
        }
    }

    ClearCore::InputMgr.InterruptHandlerSet(interruptNumber, callback,
        static_cast<ClearCore::InputManager::InterruptTrigger>(mode));
}

void detachInterrupt(pin_size_t interruptNumber) {
    if (interruptNumber >= EIC_NUMBER_OF_INTERRUPTS) {
        return;
    }

    ClearCore::InputMgr.InterruptHandlerSet(interruptNumber);
}

void interrupts() {
    ClearCore::InputMgr.InterruptsEnabled(true);
}

void noInterrupts() {
   ClearCore::InputMgr.InterruptsEnabled(false);
}

// implementation of pulseIn and pulseInLong from Common.h

/** 
    Measures the length (in microseconds) of a pulse on the pin.

    This function is essentially a wrapper for pulseInLong().

    \param <pin> {The pin to measure a pulse on}
    \param <state> {HIGH or LOW}
    \param <timeout> {The maximum amount of time to measure a pulse for in uS.}
    \return {The pulse width, or 0 if there was no pulse detected before 
    timeout.}
 **/
uint32_t pulseIn(pin_size_t pin, uint8_t state, unsigned long timeout) {
    return pulseInLong(pin, state, timeout);
}

/** 
    \brief Measures the length (in microseconds) of a pulse on the pin.

    \details Measures the duration of a pulse on a pin. The minimum pulse width
    duration depends on the digital input filtering selected on the given pin.
    The minimum pulse width is the digital filter sample length * 200 
    microseconds. If state is HIGH, this function will measure the width
    between a rising edge and its falling edge. If state is LOW, it will
    measure between a falling edge and its rising edge. This function will
    return after a complete pulse is measured, or the timeout period has
    elapsed regardless of whether or not any edges were detected.

    \param <pin> {The pin to measure a pulse on}
    \param <state> HIGH or LOW}
    \param <timeout> {The maximum amount of time to measure a pulse for in uS.}
    \return {The pulse width in uS, or 0 if there was no pulse detected before 
            timeout.}
 **/
uint32_t pulseInLong(pin_size_t pin, uint8_t state, unsigned long timeout) {

    PinStatus desiredState = (PinStatus) state;
    if (!(desiredState == HIGH || desiredState == LOW)) {
        // unsupported pin state
        return 0;
    }

    // Get a reference to the appropriate connector
    ClearCore::Connector *connector =
        ClearCore::SysMgr.ConnectorByIndex(
            static_cast<ClearCorePins>(pin));

    // If this is an invalid connector or in output mode, bail out
    if (!connector || connector->IsWritable()) {
        return 0;
    }

    // Put the connector in digital input mode
    connector->Mode(ClearCore::Connector::INPUT_DIGITAL);
    if (connector->Mode() != ClearCore::Connector::INPUT_DIGITAL) {
        // This connector does not support digital input, bail out
        return 0;
    }

    unsigned long startTime = micros();

    // twiddle your thumbs until the previous pulse ends
    while (connector->State() == desiredState) {
        if (micros() - startTime < timeout) {
            continue;
        }
        else {
            // out of time for the pulse request
            return 0;
        }
    }

    // wait for a new pulse to begin
    while (connector->State() != desiredState) {
        if (micros() - startTime < timeout) {
            continue;
        }
        else {
            // out of time for the pulse request
            return 0;
        }
    }

    // wait for the pulse to end
    unsigned long pulseBegin = micros();
    while (connector->State() == desiredState) {
        if (micros() - startTime < timeout) {
            continue;
        }
        else {
            // ran out of time before the pulse ended
            return 0;
        }
    }

    unsigned long currentTime = micros();
    if (currentTime - startTime < timeout) {
        return currentTime - pulseBegin;
    }
    else {
        return 0;
    }
}

/**
    \return Returns a pointer to the connector or CCIO-8 manager responsible for 
    the pin or null if pulse out operations aren't unsupported for the pin.
**/
static void *OutputPulsesSetup(pin_size_t pin) {
    ClearCore::Connector *connector =
        ClearCore::SysMgr.ConnectorByIndex(static_cast<ClearCorePins>(pin));

	if (!connector) {
		return NULL;
	}
    // If connector hasn't been initialized yet, just return
    if (connector->Mode() == ClearCore::Connector::INVALID_NONE) {
		return NULL;
	}
    switch (connector->Type()) {
        case ClearCore::Connector::ANALOG_OUT_DIGITAL_IN_OUT_TYPE:
        case ClearCore::Connector::H_BRIDGE_TYPE:
        case ClearCore::Connector::DIGITAL_IN_OUT_TYPE:
        case ClearCore::Connector::CCIO_DIGITAL_IN_OUT_TYPE:
            return connector;
        default:
            return nullptr; // unsupported connector type
    }
}

void OutputPulsesStart(pin_size_t pin, uint32_t onTime, uint32_t offTime,
                       uint16_t pulseCnt, bool blockUntilDone) {
    ClearCore::Connector *connector = 
        static_cast<ClearCore::Connector *>(OutputPulsesSetup(pin));

    if (!connector) {
        return;
    }
    // If connector hasn't been initialized yet, just return
    if (connector->Mode() == ClearCore::Connector::INVALID_NONE) {
		return;
	}

    if (connector->Type() == ClearCore::Connector::CCIO_DIGITAL_IN_OUT_TYPE) {
	    static_cast<ClearCore::CcioPin *>(connector)->
		    OutputPulsesStart(onTime, offTime, pulseCnt, blockUntilDone);
	}
	else {
        static_cast<ClearCore::DigitalInOut *>(connector)->
            OutputPulsesStart(onTime, offTime, pulseCnt, blockUntilDone);
    }
}

void OutputPulsesStop(pin_size_t pin) {
    OutputPulsesStop(pin, true);
}

void OutputPulsesStop(pin_size_t pin, bool stopImmediately) {
	ClearCore::Connector *connector = 
        static_cast<ClearCore::Connector *>(OutputPulsesSetup(pin));

	if (!connector) {
        return;
    }
    // If connector hasn't been initialized yet, just return
    if (connector->Mode() == ClearCore::Connector::INVALID_NONE) {
		return;
	}

	if (connector->Type() == ClearCore::Connector::CCIO_DIGITAL_IN_OUT_TYPE) {
	    static_cast<ClearCore::CcioPin *>(
            connector)->OutputPulsesStop(stopImmediately);
    }
    else {
	    static_cast<ClearCore::DigitalInOut *>(
            connector)->OutputPulsesStop(stopImmediately);
    }
}
