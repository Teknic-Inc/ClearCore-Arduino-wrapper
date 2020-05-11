#include "Arduino.h"
#include "ClearCore.h"

bool outputState = false;

void setup() {
    // Put your setup code here, it will run once:
}

void loop() {
    // Put your main code here, it will run repeatedly:
    
    // Toggle and write the digital output state.
    outputState = !outputState;
    digitalWrite(LED_BUILTIN, outputState);

    // Wait a second, then repeat.
    delay(1000);
}