#pragma once

enum class AppState {
    BOOT,
    CONNECT_WIFI,
    CONNECT_BLINK,
    WAIT_FOR_PAYMENT,
    GAME_START,
    GAME_COUNTDOWN,
    GAME_RUNNING,
    GAME_OVER
};

class StateMachine {
public:
    static StateMachine& getInstance() {
        static StateMachine instance;
        return instance;
    }

    void setState(AppState newState);
    AppState getState() const;

private:
    StateMachine() : currentState(AppState::BOOT) {}
    AppState currentState;
};
