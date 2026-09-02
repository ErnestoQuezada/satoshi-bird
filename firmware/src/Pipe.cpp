#include "Pipe.h"
#include "Config.h"
#include <Arduino.h>

Pipe::Pipe(float startX) {
    reset(startX);
}

void Pipe::update() {
    x -= Config::PIPE_SPEED;
}

bool Pipe::isOffScreen() const {
    return x + Config::PIPE_WIDTH < 0;
}

void Pipe::reset(float newX) {
    x = newX;
    // Generate a random gap Y position
    // Ensure the gap doesn't go off the top or bottom of the screen
    int minGapY = Config::PIPE_GAP / 2 + 10;
    int maxGapY = Config::SCREEN_HEIGHT - Config::PIPE_GAP / 2 - 10;
    gapY = random(minGapY, maxGapY);
    passed = false;
}
