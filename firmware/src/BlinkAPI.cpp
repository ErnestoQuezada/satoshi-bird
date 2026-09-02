#include "BlinkAPI.h"
#include "Config.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

extern const char *blinkApiKey; // We'll keep this in main.cpp
extern const char *BLINK_API_URL;

WiFiClientSecure secureClient;

void BlinkAPI::begin() {
    secureClient.setInsecure();
    initialized = false;
}

bool BlinkAPI::checkNewPayment() {
    if (millis() - last_poll_time > Config::POLLING_INTERVAL) {
        last_poll_time = millis();
        
        Serial.print(".");
        String current_tx_id = getLatestReceiveTransaction();
        
        if (current_tx_id != "") {
            if (!initialized) {
                last_tx_id = current_tx_id;
                initialized = true;
                Serial.println("\nBlink API initialized. Baseline tx: " + last_tx_id);
                return false; // Just initialized, not a new payment
            } else {
                if (current_tx_id != last_tx_id) {
                    last_tx_id = current_tx_id;
                    return true; // New payment detected!
                }
            }
        }
    }
    return false;
}

String BlinkAPI::getLatestReceiveTransaction() {
    HTTPClient http;

    if (!http.begin(secureClient, BLINK_API_URL)) {
        Serial.println("[Error] Failed to start HTTPClient");
        return "";
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-API-KEY", blinkApiKey);

    const char *query = "{\"query\":\"query listTx { me { defaultAccount { transactions(first: 5) { edges { node { id direction status settlementAmount createdAt } } } } } }\"}";

    int httpCode = http.POST(query);
    String tx_id = "";

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            JsonArray edges = doc["data"]["me"]["defaultAccount"]["transactions"]["edges"];
            for (JsonObject edge : edges) {
                JsonObject node = edge["node"];
                const char *direction = node["direction"];
                const char *status = node["status"];

                if (strcmp(direction, "RECEIVE") == 0 && strcmp(status, "SUCCESS") == 0) {
                    tx_id = node["id"].as<String>();
                    break; 
                }
            }
        } else {
            Serial.print("[Error] deserializeJson() failed: ");
            Serial.println(error.c_str());
        }
    } else {
        Serial.printf("[Error] HTTP Error: %d\n", httpCode);
    }

    http.end();
    return tx_id;
}
