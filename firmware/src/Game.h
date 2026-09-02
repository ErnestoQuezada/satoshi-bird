#pragma once

#include "Bird.h"
#include "Pipe.h"
#include <vector>
#include <Preferences.h>

class Game {
public:
    static Game& getInstance() {
        static Game instance;
        return instance;
    }

    void begin();
    void start();
    void update();
    
    int getScore() const { return score; }
    int getHighScore() const { return highScore; }

private:
    Game() {}
    
    Bird bird;
    std::vector<Pipe> pipes;
    
    int score = 0;
    int highScore = 0;
    unsigned long countdownStartTime = 0;
    Preferences preferences;
    
    void checkCollisions();
    bool checkRectCollision(float r1x, float r1y, float r1w, float r1h, 
                            float r2x, float r2y, float r2w, float r2h);
};
