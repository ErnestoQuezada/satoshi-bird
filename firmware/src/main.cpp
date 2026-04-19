#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// Blink API Configuration
// IMPORTANT: Do NOT commit your real API key to version control.
// Use a placeholder or a separate config file for production.
const char *BLINK_API_KEY = "YOUR_BLINK_API_KEY_HERE";
const char *BLINK_API_URL = "https://api.blink.sv/graphql";

String last_tx_id = "";
unsigned long last_poll_time = 0;
const unsigned long POLLING_INTERVAL = 3000; // Poll every 3 seconds
bool is_initialized = false;

WiFiClientSecure secureClient;

/**
 * Fetches the latest successful receive transaction from the Blink API.
 * Returns the transaction ID as a String.
 */
String getLatestReceiveTransaction() {
  HTTPClient http;

  if (!http.begin(secureClient, BLINK_API_URL)) {
    Serial.println("[Error] Failed to start HTTPClient");
    return "";
  }

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-KEY", BLINK_API_KEY);

  // GraphQL query to fetch the last 5 transactions
  const char *query = "{\"query\":\"query listTx { me { defaultAccount { "
                      "transactions(first: 5) { edges { node { id direction "
                      "status settlementAmount createdAt } } } } } }\"}";

  int httpCode = http.POST(query);
  String tx_id = "";

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Parse JSON response
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      JsonArray edges =
          doc["data"]["me"]["defaultAccount"]["transactions"]["edges"];
      for (JsonObject edge : edges) {
        JsonObject node = edge["node"];
        const char *direction = node["direction"];
        const char *status = node["status"];

        // Find the most recent successful receive
        if (strcmp(direction, "RECEIVE") == 0 &&
            strcmp(status, "SUCCESS") == 0) {
          tx_id = node["id"].as<String>();
          break; // Edges are ordered by newest first
        }
      }
    } else {
      Serial.print("[Error] deserializeJson() failed: ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.printf("[Error] HTTP Error updating transactions: %d\n", httpCode);
  }

  http.end();
  return tx_id;
}

/**
 * Action to perform when a payment is detected.
 * In this case, activating a GPIO for a set duration.
 */
void triggerAction() {
  Serial.println("----------------------------------------");
  Serial.println("⚡️⚡️⚡️ Lightning Payment Received! ⚡️⚡️⚡️");
  Serial.println("Activating relay for 5 seconds...");
  Serial.println("----------------------------------------");

  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Action completed. Awaiting next payment...");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Delay to let Serial monitor catch up
  delay(2000);

  Serial.println("\n\n=== Bitcoin Switch - Blink Controller ===");

  // Initialize WiFiManager
  WiFiManager wm;

  Serial.println("Connecting to WiFi...");
  // Tries to connect to known networks. If it fails, hosts "Bitcoin-Switch" AP.
  bool res = wm.autoConnect("Bitcoin-Switch", "bitcoin123");

  if (!res) {
    Serial.println("Failed to connect to WiFi! Restarting...");
    delay(3000);
    ESP.restart();
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup secure client to bypass certificate checks for simplicity
  secureClient.setInsecure();

  Serial.println("Initializing transaction state via Blink API...");
}

void loop() {
  if (millis() - last_poll_time > POLLING_INTERVAL) {
    last_poll_time = millis();

    Serial.print("."); // Loading indicator for polling

    String current_tx_id = getLatestReceiveTransaction();

    if (current_tx_id != "") {
      // First run: just set the baseline
      if (!is_initialized) {
        last_tx_id = current_tx_id;
        is_initialized = true;
        Serial.println(
            "\nReady! Listening for Lightning payments to your Blink account.");
        Serial.print("Base transaction ID: ");
        Serial.println(last_tx_id);
      } else {
        // If a new ID appears, it's a new payment!
        if (current_tx_id != last_tx_id) {
          Serial.println();
          last_tx_id = current_tx_id;
          triggerAction();
        }
      }
    }
  }
}
