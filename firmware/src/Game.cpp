#include "Game.h"
#include "Config.h"
#include "Input.h"
#include "Renderer.h"
#include "StateMachine.h"

void Game::begin() {
    preferences.begin("flappy", false);
    highScore = preferences.getInt("highscore", 0);
}

void Game::start() {
    bird.reset();
    pipes.clear();
    
    // Create initial pipes
    float firstPipeX = Config::SCREEN_WIDTH + (3000.0f / Config::FRAME_TIME_MS) * Config::PIPE_SPEED;
    
    Pipe firstPipe(firstPipeX);
    firstPipe.setGapY(Config::SCREEN_HEIGHT / 2);
    pipes.push_back(firstPipe);
    pipes.push_back(Pipe(firstPipeX + Config::PIPE_SPACING));
    
    score = 0;
    countdownStartTime = millis();
    StateMachine::getInstance().setState(AppState::GAME_COUNTDOWN);
}

void Game::update() {
    Input::getInstance().update();
    
    if (StateMachine::getInstance().getState() == AppState::GAME_COUNTDOWN) {
        unsigned long elapsed = millis() - countdownStartTime;
        if (elapsed < 3000) {
            Renderer::getInstance().drawGame(bird, pipes, score);
            int secondsLeft = 3 - (elapsed / 1000);
            Renderer::getInstance().drawCountdown(secondsLeft);
            Renderer::getInstance().pushSprite();
            return;
        } else {
            StateMachine::getInstance().setState(AppState::GAME_RUNNING);
        }
    }
    
    if (Input::getInstance().isFlapPressed()) {
        bird.flap();
    }
    
    bird.update();
    
    for (auto& pipe : pipes) {
        pipe.update();
        
        // Check scoring
        if (!pipe.passed && (pipe.getX() + Config::PIPE_WIDTH < Config::BIRD_X_POS - Config::BIRD_RADIUS)) {
            pipe.passed = true;
            score++;
        }
        
        if (pipe.isOffScreen()) {
            pipe.reset(pipe.getX() + Config::PIPE_SPACING * pipes.size());
        }
    }
    
    checkCollisions();
    
    Renderer::getInstance().drawGame(bird, pipes, score);
    Renderer::getInstance().pushSprite();
}

void Game::checkCollisions() {
    // Floor/Ceiling collision
    if (bird.getY() + Config::BIRD_RADIUS >= Config::SCREEN_HEIGHT || 
        bird.getY() - Config::BIRD_RADIUS <= 0) {
        StateMachine::getInstance().setState(AppState::GAME_OVER);
    }
    
    // Pipe collision (using simple AABB rects instead of circle-rect for simplicity/speed)
    float bx = Config::BIRD_X_POS - Config::BIRD_RADIUS;
    float by = bird.getY() - Config::BIRD_RADIUS;
    float bw = Config::BIRD_RADIUS * 2;
    float bh = Config::BIRD_RADIUS * 2;
    
    for (const auto& pipe : pipes) {
        float px = pipe.getX();
        float pw = Config::PIPE_WIDTH;
        float gapY = pipe.getGapY();
        float gapHalf = Config::PIPE_GAP / 2;
        
        // Top pipe rect
        if (checkRectCollision(bx, by, bw, bh, px, 0, pw, gapY - gapHalf)) {
            StateMachine::getInstance().setState(AppState::GAME_OVER);
        }
        
        // Bottom pipe rect
        if (checkRectCollision(bx, by, bw, bh, px, gapY + gapHalf, pw, Config::SCREEN_HEIGHT - (gapY + gapHalf))) {
            StateMachine::getInstance().setState(AppState::GAME_OVER);
        }
    }
    
    if (StateMachine::getInstance().getState() == AppState::GAME_OVER) {
        if (score > highScore) {
            highScore = score;
            preferences.putInt("highscore", highScore);
        }
    }
}

bool Game::checkRectCollision(float r1x, float r1y, float r1w, float r1h, 
                              float r2x, float r2y, float r2w, float r2h) {
    return r1x < r2x + r2w && r1x + r1w > r2x &&
           r1y < r2y + r2h && r1h + r1y > r2y;
}
