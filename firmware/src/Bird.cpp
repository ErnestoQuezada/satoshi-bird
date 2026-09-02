#include "Bird.h"
#include "Config.h"

Bird::Bird() {
    reset();
}

void Bird::reset() {
    y = Config::SCREEN_HEIGHT * 0.35f;
    velocity = 0;
}

void Bird::update() {
    velocity += Config::GRAVITY;
    if (velocity > Config::MAX_FALL_SPEED) {
        velocity = Config::MAX_FALL_SPEED;
    }
    y += velocity;
}

void Bird::flap() {
    velocity = Config::FLAP_FORCE;
}
