#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN  4
#define SS_PIN   5

MFRC522 rfid(SS_PIN, RST_PIN);

void SetupRFID() {
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Lector RFID preparat");
}

bool CheckRFID(String &tag) {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return false;
  }

  tag = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    tag += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    tag += String(rfid.uid.uidByte[i], HEX);
  }
  tag.toUpperCase();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  Serial.println("Tag llegida: " + tag);
  return true;
}