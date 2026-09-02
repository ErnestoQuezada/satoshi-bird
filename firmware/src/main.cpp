#include "BlinkAPI.h"
#include "Config.h"
#include "Game.h"
#include "Input.h"
#include "Renderer.h"
#include "StateMachine.h"
#include <Arduino.h>
#include <WiFiManager.h>


// Fallbacks for environment variables (set in .env file)
#ifndef BLINK_API_KEY
#define BLINK_API_KEY "YOUR_API_KEY_HERE"
#endif

#ifndef WIFI_AP_NAME
#define WIFI_AP_NAME "Bitcoin-Switch"
#endif

#ifndef WIFI_AP_PASSWORD
#define WIFI_AP_PASSWORD "bitcoin123"
#endif

// Blink API Configuration
const char *blinkApiKey = BLINK_API_KEY;
const char *BLINK_API_URL = "https://api.blink.sv/graphql";

// Global instances handled by singletons, but we need some state variables
unsigned long gameOverTime = 0;
unsigned long lastFrameTime = 0;
bool wifiConnected = false;

void setup() {
  Serial.begin(115200);

  // T-Display S3 specific power and backlight
  pinMode(15, OUTPUT); // Power on display
  digitalWrite(15, HIGH);

  pinMode(38, OUTPUT); // Backlight
  digitalWrite(38, HIGH);

  delay(2000);

  Serial.println(
      "\n\n=== Bitcoin Switch - Blink Controller (T-Display S3) ===");

  Renderer::getInstance().begin();
  Input::getInstance().begin();
  Game::getInstance().begin();
  BlinkAPI::getInstance().begin();

  StateMachine::getInstance().setState(AppState::BOOT);
}

void loop() {
  static AppState lastState = AppState::BOOT;
  AppState currentState = StateMachine::getInstance().getState();
  
  bool stateChanged = (currentState != lastState);
  lastState = currentState;

  switch (currentState) {
  case AppState::BOOT: {
    Renderer::getInstance().drawBootScreen();
    delay(1000);
    StateMachine::getInstance().setState(AppState::CONNECT_WIFI);
    break;
  }

  case AppState::CONNECT_WIFI: {
    Renderer::getInstance().drawWifiConnecting();

    WiFiManager wm;
    bool res = wm.autoConnect(WIFI_AP_NAME, WIFI_AP_PASSWORD);

    if (!res) {
      Serial.println("Failed to connect to WiFi! Restarting...");
      delay(3000);
      ESP.restart();
    }

    wifiConnected = true;
    Serial.println("WiFi connected!");
    StateMachine::getInstance().setState(AppState::CONNECT_BLINK);
    break;
  }

  case AppState::CONNECT_BLINK: {
    Renderer::getInstance().drawBlinkConnecting();

    // Poll once to establish baseline
    BlinkAPI::getInstance().checkNewPayment();
    if (BlinkAPI::getInstance().isInitialized()) {
      StateMachine::getInstance().setState(AppState::WAIT_FOR_PAYMENT);
    }
    break;
  }

  case AppState::WAIT_FOR_PAYMENT: {
    if (stateChanged) {
      Renderer::getInstance().drawWaitingScreen(
          wifiConnected, BlinkAPI::getInstance().isInitialized());
    }

    if (BlinkAPI::getInstance().checkNewPayment()) {
      StateMachine::getInstance().setState(AppState::GAME_START);
    }
    break;
  }

  case AppState::GAME_START: {
    Renderer::getInstance().clearScreen();
    Game::getInstance().start();
    // State is updated inside Game::start() to GAME_RUNNING
    break;
  }

  case AppState::GAME_COUNTDOWN:
  case AppState::GAME_RUNNING: {
    unsigned long currentMillis = millis();
    if (currentMillis - lastFrameTime >= Config::FRAME_TIME_MS) {
      lastFrameTime = currentMillis;
      Game::getInstance().update();
    }
    break;
  }

  case AppState::GAME_OVER: {
    Renderer::getInstance().drawGameOver(Game::getInstance().getScore(),
                                         Game::getInstance().getHighScore());

    if (gameOverTime == 0) {
      gameOverTime = millis();
    }

    if (millis() - gameOverTime > Config::GAME_OVER_DELAY) {
      gameOverTime = 0;
      StateMachine::getInstance().setState(AppState::WAIT_FOR_PAYMENT);
    }
    break;
  }
  }
}
