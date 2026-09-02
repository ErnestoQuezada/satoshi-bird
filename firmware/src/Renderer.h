#pragma once

#include <TFT_eSPI.h>
#include "Config.h"
#include "Bird.h"
#include "Pipe.h"
#include <vector>

class Renderer {
public:
    static Renderer& getInstance() {
        static Renderer instance;
        return instance;
    }

    void begin();
    void clearScreen(uint32_t color = Config::COLOR_BG);
    
    void drawBootScreen();
    void drawWifiConnecting();
    void drawBlinkConnecting();
    void drawWaitingScreen(bool wifiConnected, bool blinkConnected);
    
    void drawGame(const Bird& bird, const std::vector<Pipe>& pipes, int score);
    void drawCountdown(int secondsLeft);
    void pushSprite();
    void drawGameOver(int score, int highScore);

private:
    Renderer() : sprite(&tft) {}
    TFT_eSPI tft = TFT_eSPI();
    TFT_eSprite sprite;
    void drawQRCode(const char* text, int x, int y, int size);
};
