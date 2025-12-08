#include <Arduino.h>
#include <WiFi.h>
extern bool WifiConnected;
extern void SetupWifi();
extern void SetupRFID();
extern void CheckWifi();
extern bool CheckRFID(String &tag);

extern void SetupAWS();
extern void CheckAWS();
extern void PublicaTag(String tagID);
extern bool AWSConnected;
extern bool WifiConnected;        // <--- AFEGEIX AQUESTA

void setup() {
  Serial.begin(115200);
  delay(500);
  SetupWifi();
  SetupRFID();
  SetupAWS();
}

void loop() {
  CheckWifi();

  if (WiFi.status() == WL_CONNECTED) {
    CheckAWS();

    if (AWSConnected) {
      String tag;
      if (CheckRFID(tag)) {
        PublicaTag(tag);
      }
    }
  }
  delay(50);
}