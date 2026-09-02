#pragma once

#include <Arduino.h>

class BlinkAPI {
public:
    static BlinkAPI& getInstance() {
        static BlinkAPI instance;
        return instance;
    }

    void begin();
    
    // Non-blocking poll. Returns true if a new payment was detected since last call.
    bool checkNewPayment();
    
    bool isInitialized() const { return initialized; }

private:
    BlinkAPI() {}
    
    String getLatestReceiveTransaction();
    
    String last_tx_id = "";
    unsigned long last_poll_time = 0;
    bool initialized = false;
};
