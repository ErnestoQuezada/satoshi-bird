#include "Input.h"
#include "Config.h"
#include <Arduino.h>

void Input::begin() {
    pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
}

void Input::update() {
    flapPressed = false;
    bool reading = digitalRead(Config::BUTTON_PIN);

    if (reading == LOW && lastButtonState == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
            flapPressed = true;
            lastDebounceTime = millis();
        }
    }

    lastButtonState = reading;
}

bool Input::isFlapPressed() const {
    return flapPressed;
}
