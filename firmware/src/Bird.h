#pragma once

class Bird {
public:
    Bird();
    void reset();
    void update();
    void flap();
    
    float getY() const { return y; }
    float getVelocity() const { return velocity; }

private:
    float y;
    float velocity;
};
