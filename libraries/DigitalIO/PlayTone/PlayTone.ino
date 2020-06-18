/*
 * Title: PlayTone
 *
 * Objective:
 *    This example demonstrates how to play a sequence of tones on the
 *    ClearCore's H-Bridge connectors using the musical frequency values defined
 *    in the pitches.h header file.
 *
 * Description:
 *    This example sets up an H-Bridge connector into output mode, sets the
 *    volume, and plays a melodic sequence of tones of equal duration.
 *
 * Requirements:
 * ** A speaker or other audio output device connected to IO-4
 *
 * Links:
 * ** web link to doxygen (all Examples)
 * ** web link to ClearCore Manual (all Examples)  <<FUTURE links to Getting started webpage/ ClearCore videos>>
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "pitches.h"

// Notes to be played in sequence as part of a melody.
// The maximum tone frequency is 1/4 of the tone interrupt rate, i.e. 5512 Hz.
// Any commanded frequency above 5512 Hz will get clipped to 5512 Hz.
// See the "pitches.h" file for the frequency definitions of these notes
const int melody[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5,
                      NOTE_CS4, NOTE_FS4, NOTE_AS4, NOTE_CS5,
                      NOTE_E4, NOTE_G4, NOTE_C5, NOTE_E5,
                      NOTE_FS4, NOTE_AS4, NOTE_CS5, NOTE_FS5
                     };
const uint32_t noteCount = sizeof(melody) / sizeof(melody[0]);

const uint32_t toneDuration = 200; // in milliseconds
const float toneAmplitude = 0.01;  // max volume is 1

// Tone output is supported on connectors IO-4 and IO-5 only
#define tonePin IO4

void setup() {
    // Put your setup code here, it will run once:

    // Set the tone connector into output mode.
    pinMode(tonePin, OUTPUT);

    // Set the volume of the tone connector to the value specified
    // by toneAmplitude.
    toneVolume(tonePin, toneAmplitude);
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Play the melody in order with equal note durations
    for (int note = 0; note < noteCount; note++) {
        tone(tonePin, melody[note]);
        delay(toneDuration);
    }

    // Stop the tone generation
    noTone(tonePin);

    // Wait a second, then repeat...
    delay(1000);
}