/*
    Copyright (c) 2019 Arduino LLC.  All right reserved.

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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void yield(void);

typedef enum {
    LOW = 0,
    HIGH = 1,
    CHANGE = 2,
    FALLING = 3,
    RISING = 4,
} PinStatus;

typedef enum {
    INPUT = 0x0,
    OUTPUT = 0x1,
    INPUT_PULLUP = 0x2,
} PinMode;

typedef enum {
    PWM,
    CURRENT
} AnalogOutMode;

typedef enum {
    ADC_RAW,
    MILLIVOLTS
} AnalogInputUnits;

typedef enum {
    DAC_RAW,
    MICROAMPS
} AnalogOutputUnits;

typedef enum {
    LSBFIRST = 0,
    MSBFIRST = 1,
} BitOrder;

#define PI         3.1415926535897932384626433832795
#define HALF_PI    1.5707963267948966192313216916398
#define TWO_PI     6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER      2.718281828459045235360287471352

#define SERIAL     0x0
#define DISPLAY    0x1

typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrParam)(void *);

// interrupts() / noInterrupts() must be defined by the core

#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) \
    (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#ifndef bit
#define bit(b) (1UL << (b))
#endif

typedef bool     boolean;
typedef uint8_t  byte;
typedef uint16_t word;
typedef uint16_t _iq15_s;

void init(void);
void initVariant(void);

int atexit(void (*func)()) __attribute__((weak));
int main() __attribute__((weak));

#ifdef EXTENDED_PIN_MODE
// Platforms who want to declare more than 256 pins need to define
// EXTENDED_PIN_MODE globally
typedef uint32_t pin_size_t;
#else
typedef uint8_t pin_size_t;
#endif

// Define sketch exposed functions
// New API definitions for these functions break Arduino's included examples.
// Created "overloaded" versions to facilitate this

// Digital I/O
PinStatus digitalRead(pin_size_t pinNumber);
void digitalWrite(pin_size_t ulPin, uint32_t ulVal);
void pinMode(pin_size_t ulPin, uint32_t ulMode);

PinStatus digitalReadClearCore(pin_size_t pinNumber);
void digitalWriteClearCore(pin_size_t pinNumber, PinStatus status);
void pinModeAPI(pin_size_t pinNumber, PinMode ulMode);

// Analog I/O
int analogReadAPI(pin_size_t pinNumber, AnalogInputUnits units);
void analogWriteAPI(pin_size_t pinNumber, int value, AnalogOutMode mode,
                    AnalogOutputUnits units);

// Zero, Due & MKR Family
void analogReadResolution(int res);

// Advanced I/O
void noTone(uint8_t _pin);
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);
pin_size_t shiftIn(pin_size_t dataPin, pin_size_t clockPin, BitOrder bitOrder);
void shiftOut(pin_size_t dataPin, pin_size_t clockPin, BitOrder bitOrder,
              uint8_t val);
void tone(uint8_t _pin, unsigned int frequency, unsigned long duration);
void toneVolume(unsigned char outputPin, float volume);
void OutputPulsesStop(pin_size_t pin);

// Time
void delay(unsigned long);
void delayMicroseconds(unsigned int usec);
unsigned long micros(void);
unsigned long millis(void);

// Math
#ifndef constrain
#define constrain(amt, low, high) \
    ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

#ifndef min
#define min(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif

#ifndef max
#define max(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#endif

#ifndef sq
#define sq(x) ((x) * (x))
#endif

// Trigonometry
#ifndef radians
#define radians(deg) ((deg) * DEG_TO_RAD)
#endif

#ifndef degrees
#define degrees(rad) ((rad) * RAD_TO_DEG)
#endif

// External Interrupts
void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback,
                     PinStatus mode);
void attachInterruptParam(pin_size_t interruptNumber, voidFuncPtrParam callback,
                          PinStatus mode, void *param);
void detachInterrupt(pin_size_t interruptNumber);

// Interrupts
void interrupts();
void noInterrupts();

// Test Sketch
void setup(void);
void loop(void);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

/* C++ prototypes */
int analogRead(pin_size_t pinNumber, AnalogInputUnits units = ADC_RAW);
void analogWrite(pin_size_t pinNumber, uint32_t value,
                 AnalogOutMode mode = PWM, AnalogOutputUnits units = DAC_RAW);

uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);

#define word(...) makeWord(__VA_ARGS__)

unsigned long pulseIn(uint8_t pin, uint8_t state,
                      unsigned long timeout = 1000000L); // timeout in microsec
unsigned long pulseInLong(uint8_t pin, uint8_t state,
                          unsigned long timeout = 1000000L);

void OutputPulsesStart(pin_size_t pin, uint32_t onTime, uint32_t offTime,
                       uint16_t pulseCnt = 0, bool blockUntilDone = false);
void OutputPulsesStop(pin_size_t pin, bool stopImmediately);

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration = 0);
// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned long);
long map(long, long, long, long, long);

#endif // __cplusplus