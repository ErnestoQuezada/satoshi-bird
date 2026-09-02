#include "StateMachine.h"

void StateMachine::setState(AppState newState) {
    currentState = newState;
}

AppState StateMachine::getState() const {
    return currentState;
}
