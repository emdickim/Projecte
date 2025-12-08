#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"
bool WifiConnected = false;

void SetupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connectant WiFi");
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    WifiConnected = true;
    Serial.println("\nWiFi connectat! IP: " + WiFi.localIP().toString());
  } else {
    WifiConnected = false;
    Serial.println("\nERROR WiFi");
  }
}

void CheckWifi() {
  if (WiFi.status() != WL_CONNECTED && WifiConnected) {
    WifiConnected = false;
    Serial.println("WiFi perdut!");
  }
  if (WiFi.status() == WL_CONNECTED && !WifiConnected) {
    WifiConnected = true;
    Serial.println("WiFi reconectat!");
  }
}
/*#include <WiFi.h>
bool WifiConnected = false;
const char *ssid = "lolxd";
const char *password = "12345678";

void SetupWifi() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\nConnectant a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connectat!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  WifiConnected = true;
}

void CheckWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ Connexió WiFi perduda. Reconnectant...");
    WifiConnected = false;
    WiFi.disconnect();
    WiFi.reconnect();

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Reconnectat a WiFi!");
      WifiConnected = true;
    } else {
      Serial.println("\n❌ No s'ha pogut reconnectar a WiFi.");
    }
  } else {
    WifiConnected = true;
  }
}*/