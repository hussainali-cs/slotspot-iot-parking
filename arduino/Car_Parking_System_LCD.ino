#include <Servo.h>  // includes the servo library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10
LiquidCrystal_I2C lcd(0x27, 20, 4);

Servo myservo;
byte readCard[4];
String MasterTag1 = "441BB71";  // REPLACE this Tag ID with your Tag ID!!!
String MasterTag2 = "A3FCE71A";  // REPLACE this Tag ID with your Tag ID!!!
String MasterTag3 = "99AA2474";  // REPLACE this Tag ID with your Tag ID!!!
String tagID = "";
boolean scan = false;

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define ir_enter A0
#define ir_back A1

#define ir_car1 2
#define ir_car2 3
#define ir_car3 4
#define ir_car4 5
#define ir_car5 6
#define ir_car6 7

int rembal = 100;
int balance;
int S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0;
int flag1 = 0, flag2 = 0;
int slot = 6;
unsigned long barrierTimer = 0; // Timer for the barrier
unsigned long entryTime = 0; // Timer for entry
const unsigned long barrierDelay = 3000; // 3 seconds delay for the barrier
const unsigned long exitDelay = 3000; // 3 seconds delay after entry

void setup() {
  Serial.begin(9600);
  SPI.begin();         // SPI bus
  mfrc522.PCD_Init();  // MFRC522
  pinMode(ir_car1, INPUT);
  pinMode(ir_car2, INPUT);
  pinMode(ir_car3, INPUT);
  pinMode(ir_car4, INPUT);
  pinMode(ir_car5, INPUT);
  pinMode(ir_car6, INPUT);

  pinMode(ir_enter, INPUT);
  pinMode(ir_back, INPUT);

  myservo.attach(8);
  myservo.write(90); // Start with the barrier closed

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("      IoT Based   ");
  lcd.setCursor(0, 1);
  lcd.print("    Car  Parking     ");
  lcd.setCursor(0, 2);
  lcd.print("       System     ");
  delay(2000);
  lcd.clear();
}

void loop() {
  Read_Sensor();  // Read the sensor data in each loop iteration

  // Calculate available slots based on the IR sensor data
  int totalFilledSlots = S1 + S2 + S3 + S4 + S5 + S6;
  slot = 6 - totalFilledSlots;  // Assuming there are 6 total slots

  // RFID Logic
  while (getID()) {
    if (tagID == MasterTag1 || tagID == MasterTag2 || tagID == MasterTag3) {
      scan = true;
      rembal = rembal - 20;
      if (rembal < 0) {
        scan = false;
        rembal = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Sorry Low Balance");
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Access Granted!");
        lcd.setCursor(0, 1);
        lcd.print(" Opening Barrier...");
      }
      Serial.println(rembal);
      Serial.println(scan);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Access Denied!");
      lcd.setCursor(0, 1);
      lcd.print(" Barrier won't open.");
    }
    Serial.print(" ID : ");
    Serial.println(tagID);
    delay(2000);
  }

  // Update the LCD with available slots and their status
  lcd.setCursor(0, 0);
  lcd.print("Available Slots: ");
  lcd.print(slot);
  lcd.print(" ");

  // Display slot statuses
  lcd.setCursor(0, 1);
  lcd.print("S1:"); lcd.print(S1 == 1 ? "Fill " : "Empty");
  lcd.setCursor(10, 1);
  lcd.print("S2:"); lcd.print(S2 == 1 ? "Fill " : "Empty");
  
  lcd.setCursor(0, 2);
  lcd.print("S3:"); lcd.print(S3 == 1 ? "Fill " : "Empty");
  lcd.setCursor(10, 2);
  lcd.print("S4:"); lcd.print(S4 == 1 ? "Fill " : "Empty");

  lcd.setCursor(0, 3);
  lcd.print("S5:"); lcd.print(S5 == 1 ? "Fill " : "Empty");
  lcd.setCursor(10, 3);
  lcd.print("S6:"); lcd.print(S6 == 1 ? "Fill " : "Empty");

  // Entry Logic
  if (digitalRead(ir_enter) == 0 && flag1 == 0 && scan == true) {
    if (slot > 0) {
      flag1 = 1;
      scan = false;
      // Open the barrier
      for (int i = 90; i < 180; i++) {
        myservo.write(i);
        delay(5);
      }
      delay(1000);  // Keep the barrier open for a moment
      slot = slot - 1;  // Decrease available slots
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Welcome! Entering...");
      delay(1000);

      // Set entry time
      entryTime = millis(); // Start the entry timer
      barrierTimer = millis(); // Start the barrier timer
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Sorry Parking Full ");
      delay(1500);
    }
  }

  // Exit Logic
  if (digitalRead(ir_back) == 0 && flag2 == 0) {
    // Check if 3 seconds have passed since entry
    if (millis() - entryTime >= exitDelay) {
      flag2 = 1;
      // Open the barrier for exiting
      for (int i = 90; i < 180; i++) {
        myservo.write(i);
        delay(5);
      }
      slot = slot + 1;  // Increase available slots
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Thank you! Exiting...");
      delay(1000);

      // Set barrier timer
      barrierTimer = millis(); // Start the timer
    }
  }

  // Check if the barrier timer has elapsed
  if ((flag1 == 1 || flag2 == 1) && (millis() - barrierTimer >= barrierDelay)) {
    // Close the barrier
    for (int i = 180; i > 90; i--) {
      myservo.write(i);
      delay(5);
    }

    // Reset flags
    flag1 = 0;
    flag2 = 0;
  }

  // Send data to ESP32
  serialsend();
}

// Read new tag if available
boolean getID() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {  // If a new PICC placed to RFID reader continue
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  // Since a PICC placed get Serial and continue
    return false;
  }
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));  // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  return true;
}

void serialsend() {
  Serial.println(String(S1) + "," + String(S2) + "!" + String(S3) + "#" + String(S4) + "$" + String(S5) + "%" + String(S6) + "^" + String(rembal));  
}

void Read_Sensor() {
  S1 = digitalRead(ir_car1) == 0 ? 1 : 0;  // Detect if car is in slot 1
  S2 = digitalRead(ir_car2) == 0 ? 1 : 0;  // Detect if car is in slot 2
  S3 = digitalRead(ir_car3) == 0 ? 1 : 0;  // Detect if car is in slot 3
  S4 = digitalRead(ir_car4) == 0 ? 1 : 0;  // Detect if car is in slot 4
  S5 = digitalRead(ir_car5) == 0 ? 1 : 0;  // Detect if car is in slot 5
  S6 = digitalRead(ir_car6) == 0 ? 1 : 0;  // Detect if car is in slot 6
}