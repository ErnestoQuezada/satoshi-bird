#pragma once

class Input {
public:
    static Input& getInstance() {
        static Input instance;
        return instance;
    }

    void begin();
    void update();
    bool isFlapPressed() const;

private:
    Input() {}
    bool flapPressed = false;
    bool lastButtonState = true; // Pull-up means true is unpressed
    unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50;
};
