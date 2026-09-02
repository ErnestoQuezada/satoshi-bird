#include "Renderer.h"
#include "Config.h"
#include <qrcode.h>

void Renderer::begin() {
    tft.init();
    tft.setRotation(1); // Landscape
    tft.fillScreen(Config::COLOR_BG);
    sprite.createSprite(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
}

void Renderer::clearScreen(uint32_t color) {
    tft.fillScreen(color);
}

void Renderer::drawBootScreen() {
    tft.fillScreen(Config::COLOR_BG);
    tft.setTextColor(Config::COLOR_TEXT, Config::COLOR_BG);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Bitcoin Switch", Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2 - 20, 4);
    tft.drawString("Initializing...", Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2 + 20, 2);
}

void Renderer::drawWifiConnecting() {
    tft.fillScreen(Config::COLOR_BG);
    tft.setTextColor(Config::COLOR_TEXT, Config::COLOR_BG);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Connecting to WiFi...", Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2, 4);
}

void Renderer::drawBlinkConnecting() {
    tft.fillScreen(Config::COLOR_BG);
    tft.setTextColor(Config::COLOR_TEXT, Config::COLOR_BG);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Connecting to Blink...", Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2, 4);
}

void Renderer::drawWaitingScreen(bool wifiConnected, bool blinkConnected) {
    tft.fillScreen(Config::COLOR_BG);
    
    // Draw QR code
    int qrSize = 130;
    int qrX = (Config::SCREEN_WIDTH / 2) - qrSize - 10;
    int qrY = (Config::SCREEN_HEIGHT - qrSize) / 2;
    drawQRCode(Config::INVOICE_STRING, qrX, qrY, qrSize);

    // Draw Status Text
    tft.setTextColor(Config::COLOR_TEXT, Config::COLOR_BG);
    tft.setTextDatum(TL_DATUM);
    int textX = Config::SCREEN_WIDTH / 2 + 10;
    int textY = 30;
    
    tft.drawString("Waiting for", textX, textY, 4);
    tft.drawString("Lightning Payment", textX, textY + 25, 2);

    textY += 60;
    
    // Indicators
    tft.drawString("WiFi:", textX, textY, 2);
    tft.setTextColor(wifiConnected ? TFT_GREEN : TFT_RED, Config::COLOR_BG);
    tft.drawString(wifiConnected ? "Connected" : "Disconnected", textX + 40, textY, 2);
    
    textY += 20;
    tft.setTextColor(Config::COLOR_TEXT, Config::COLOR_BG);
    tft.drawString("Blink:", textX, textY, 2);
    tft.setTextColor(blinkConnected ? TFT_GREEN : TFT_RED, Config::COLOR_BG);
    tft.drawString(blinkConnected ? "Ready" : "Waiting", textX + 45, textY, 2);
}

void Renderer::drawQRCode(const char* text, int x, int y, int size) {
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, text);
    
    int scale = size / qrcode.size;
    int offsetX = (size - qrcode.size * scale) / 2;
    int offsetY = (size - qrcode.size * scale) / 2;
    
    tft.fillRect(x, y, size, size, Config::COLOR_QR_BG);
    
    for (uint8_t qy = 0; qy < qrcode.size; qy++) {
        for (uint8_t qx = 0; qx < qrcode.size; qx++) {
            if (qrcode_getModule(&qrcode, qx, qy)) {
                tft.fillRect(x + offsetX + qx * scale, y + offsetY + qy * scale, scale, scale, Config::COLOR_QR_FG);
            }
        }
    }
}

void Renderer::drawGame(const Bird& bird, const std::vector<Pipe>& pipes, int score) {
    sprite.fillSprite(Config::COLOR_BG);

    // Draw Bird (Circle for simplicity)
    sprite.fillCircle(Config::BIRD_X_POS, (int)bird.getY(), Config::BIRD_RADIUS, Config::COLOR_BIRD);

    // Draw Pipes
    for (const auto& pipe : pipes) {
        int px = (int)pipe.getX();
        int gapY = pipe.getGapY();
        
        // Top pipe
        sprite.fillRect(px, 0, Config::PIPE_WIDTH, gapY - Config::PIPE_GAP / 2, Config::COLOR_PIPE);
        // Bottom pipe
        sprite.fillRect(px, gapY + Config::PIPE_GAP / 2, Config::PIPE_WIDTH, Config::SCREEN_HEIGHT - (gapY + Config::PIPE_GAP / 2), Config::COLOR_PIPE);
    }

    // Draw Score
    sprite.setTextColor(Config::COLOR_TEXT);
    sprite.setTextDatum(TC_DATUM);
    sprite.drawNumber(score, Config::SCREEN_WIDTH / 2, 10, 4);
}

void Renderer::drawCountdown(int secondsLeft) {
    sprite.setTextColor(TFT_RED);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString(String(secondsLeft), Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2, 6);
}

void Renderer::pushSprite() {
    sprite.pushSprite(0, 0);
}

void Renderer::drawGameOver(int score, int highScore) {
    tft.setTextColor(TFT_RED);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("GAME OVER", Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2 - 20, 4);
    
    tft.setTextColor(Config::COLOR_TEXT);
    tft.drawString("Score: " + String(score), Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2 + 10, 2);
    tft.drawString("High Score: " + String(highScore), Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2 + 30, 2);
}
