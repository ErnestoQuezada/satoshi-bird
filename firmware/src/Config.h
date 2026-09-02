#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h> // For color constants

namespace Config {

    // Hardware
    constexpr uint8_t BUTTON_PIN = 14; // T-Display S3 right button

    // Network & API
    constexpr unsigned long POLLING_INTERVAL = 3000;
    
    // We will generate the invoice QR from a predefined string for now, or you can dynamically set it
    // In a real app, this should be fetched from Blink.
    constexpr char INVOICE_STRING[] = "lightning:acreonte@blink.sv";

    // Display
    constexpr int SCREEN_WIDTH = 320; // Landscape
    constexpr int SCREEN_HEIGHT = 170;

    // Game Physics
    constexpr float GRAVITY = 0.35f;
    constexpr float FLAP_FORCE = -4.5f;
    constexpr float MAX_FALL_SPEED = 8.0f;
    constexpr float PIPE_SPEED = 2.0f;
    
    // Game Layout
    constexpr int BIRD_X_POS = 50;
    constexpr int BIRD_RADIUS = 8;
    constexpr int PIPE_WIDTH = 40;
    constexpr int PIPE_GAP = 90;
    constexpr int PIPE_SPACING = 160;

    // Game Timings
    constexpr unsigned long GAME_OVER_DELAY = 3000;
    constexpr int TARGET_FPS = 30;
    constexpr unsigned long FRAME_TIME_MS = 1000 / TARGET_FPS;

    // Colors
    constexpr uint32_t COLOR_BG = TFT_NAVY;
    constexpr uint32_t COLOR_BIRD = TFT_YELLOW;
    constexpr uint32_t COLOR_PIPE = TFT_GREEN;
    constexpr uint32_t COLOR_TEXT = TFT_WHITE;
    constexpr uint32_t COLOR_QR_BG = TFT_WHITE;
    constexpr uint32_t COLOR_QR_FG = TFT_BLACK;

} // namespace Config
