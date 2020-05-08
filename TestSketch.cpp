#include "Arduino.h"
#include "ClearCore.h"

bool outputState;

void setup() {
    // Put your setup code here, it will run once:

    // Configure pins IO-0 through IO-5 as digital outputs. These are the only
    // pins that support digital output mode.
    pinMode(IO0, OUTPUT);
    pinMode(IO1, OUTPUT);
    pinMode(IO2, OUTPUT);
    pinMode(IO3, OUTPUT);
    pinMode(IO4, OUTPUT);
    pinMode(IO5, OUTPUT);
    
    // The connectors are all set up; start the loop with turning them all on.
    outputState = true;
}

void loop() {
    // Put your main code here, it will run repeatedly:
    
    // Toggle the digital output state.
    if (outputState) {
        digitalWrite(IO0, true);
        digitalWrite(IO1, true);
        digitalWrite(IO2, true);
        digitalWrite(IO3, true);
        digitalWrite(IO4, true);
        digitalWrite(IO5, true);
    }
    else {
        digitalWrite(IO0, false);
        digitalWrite(IO1, false);
        digitalWrite(IO2, false);
        digitalWrite(IO3, false);
        digitalWrite(IO4, false);
        digitalWrite(IO5, false);
    }

    // Toggle the state to write in the next loop.
    outputState = !outputState;

    // Wait a second, then repeat.
    delay(1000);
}