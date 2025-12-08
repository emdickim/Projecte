#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "secrets.h"

bool AWSConnected = false;

LiquidCrystal_I2C lcd(0x27, 20, 4);  // Cambia a 0x3F si tu pantalla lo necesita

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

#define THINGNAME "ESP32"
#define AWS_SUB_TOPIC "iticbcn/espnode01/sub"
#define AWS_PUB_TOPIC "iticbcn/espnode01/pub"

void mostrarLCD(String l1="", String l2="", String l3="", String l4="") {
  lcd.clear();
  if(l1!="") { lcd.setCursor(0,0); lcd.print(l1.substring(0,20)); }
  if(l2!="") { lcd.setCursor(0,1); lcd.print(l2.substring(0,20)); }
  if(l3!="") { lcd.setCursor(0,2); lcd.print(l3.substring(0,20)); }
  if(l4!="") { lcd.setCursor(0,3); lcd.print(l4.substring(0,20)); }
}

void messageHandler(String &topic, String &payload) {
  Serial.println("AWS → " + payload);

  if (payload.indexOf("ok") != -1 || payload.indexOf("entrada") != -1 || payload.indexOf("sortida") != -1) {
    mostrarLCD("ITIC Barcelona", "ACCES PERMES", "Benvingut :-)", "");
  } else {
    mostrarLCD("ITIC Barcelona", "ACCES DENEGAT", "Targeta no valida", "");
  }
}

void SetupAWS() {
  Wire.begin(21, 22);           // SDA = 21, SCL = 22
  lcd.init();
  lcd.backlight();
  mostrarLCD("ITIC Barcelona", "Connectant AWS...", "", "");

  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  client.onMessage(messageHandler);

  if (client.connect(THINGNAME)) {
    AWSConnected = true;
    client.subscribe(AWS_SUB_TOPIC);
    mostrarLCD("ITIC Barcelona", "AWS Connectat!", "Escaneja el TAG", "");
  } else {
    AWSConnected = false;
    mostrarLCD("ITIC Barcelona", "ERROR AWS", "Reintentant...", "");
  }
}

void CheckAWS() {
  if (!client.connected()) {
    AWSConnected = false;
    mostrarLCD("ITIC Barcelona", "AWS perdut", "Reconnectant...", "");
    delay(5000);
    SetupAWS();
  } else {
    client.loop();
  }
}

void PublicaTag(String tagID) {
  if (!AWSConnected) return;

  StaticJsonDocument<200> doc;
  doc["tag"] = tagID;

  char buffer[256];
  serializeJson(doc, buffer);

  client.publish(AWS_PUB_TOPIC, buffer);
  mostrarLCD("ITIC Barcelona", "Tag enviada", tagID, "Esperant resposta...");
}