#define SS_PIN   10
#define RST_PIN  3

#define SPI_SCK  6
#define SPI_MOSI 7
#define SPI_MISO 2

#define DF_RX 20
#define DF_TX 21

#include <SPI.h>
#include <MFRC522.h>
#include <DFRobotDFPlayerMini.h>

MFRC522 rfid(SS_PIN, RST_PIN);
DFRobotDFPlayerMini dfplayer;

HardwareSerial dfSerial(1);

// Replace this UID
byte allowedUID[4] = {0xDE, 0xAD, 0xBE, 0xEF};

bool playing = false;

void setup() {
  Serial.begin(115200);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SS_PIN);

  rfid.PCD_Init();

  dfSerial.begin(9600, SERIAL_8N1, DF_RX, DF_TX);

  if (!dfplayer.begin(dfSerial)) {
    Serial.println("DFPlayer not found");
    while (true);
  }

  dfplayer.volume(20); // 0–30
  dfplayer.stop();

  Serial.println("Ready. Scan RFID.");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  if (checkUID()) {
    Serial.println("Card accepted");

    if (!playing) {
      dfplayer.play(1);
      playing = true;
    } else {
      dfplayer.stop();
      playing = false;
    }
  } else {
    Serial.println("Unknown card");
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);
}

bool checkUID() {
  if (rfid.uid.size != 4) return false;

  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedUID[i]) {
      return false;
    }
  }
  return true;
}
