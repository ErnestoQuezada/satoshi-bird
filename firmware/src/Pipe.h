#pragma once

class Pipe {
public:
    Pipe(float startX);
    void update();
    bool isOffScreen() const;
    void reset(float newX);

    float getX() const { return x; }
    int getGapY() const { return gapY; }
    void setGapY(int newGapY) { gapY = newGapY; }
    bool passed;

private:
    float x;
    int gapY;
};
