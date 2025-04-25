#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 3
#define GREEN_LED 6
#define RED_LED 7

MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;

byte authorizedUID[4] = {0x91, 0x61, 0x31, 0x03}; // Replace with your card UID

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Locked

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH); // Red ON by default (locked)

  Serial.println("Place your RFID card near the reader...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("Card UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (isAuthorized(rfid.uid.uidByte)) {
    Serial.println("Access Granted");

    digitalWrite(RED_LED, LOW);     // Turn OFF red LED
    digitalWrite(GREEN_LED, HIGH);  // Turn ON green LED

    myServo.write(90); // Unlock
    delay(3000);
    myServo.write(0);  // Lock again

    digitalWrite(GREEN_LED, LOW);   // Turn off green LED
    digitalWrite(RED_LED, HIGH);    // Turn red LED back ON (locked)
  } else {
    Serial.println("Access Denied");

    // Red LED already ON, optionally blink it if you want:
    digitalWrite(RED_LED, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
    delay(200);
  }

  rfid.PICC_HaltA();
  delay(500);
}

bool isAuthorized(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != authorizedUID[i]) return false;
  }
  return true;
}



