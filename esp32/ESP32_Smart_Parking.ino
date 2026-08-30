#include <WiFi.h>
#include <FirebaseESP32.h>

#define WIFI_SSID "[]"
#define WIFI_PASSWORD "[]"
#define FIREBASE_AUTH "KEY"
#define FIREBASE_HOST "DB NAME" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

#define RXD2 16
#define TXD2 17
int rembalance = 0;
int S1 = 0, S2 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0, rembal = 0;
int index1, index2, index3, index4, index5, index6, index7;
String S1String = "", S2String = "", S3String = "", S4String = "", S5String = "", S6String = "", rembalString = "";
String str = "";

void setup() {
  Serial.begin(9600); // Starts the serial communication
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  /////////// Wifi ///////////////
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(3000);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Set up Firebase config
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serialparse();
  upload();
}

void Serialparse() {
  while (Serial2.available()) {
    str = Serial2.readStringUntil('\n');
    Serial.println(str);
    index1 = str.indexOf(',');
    index2 = str.indexOf('!');
    index3 = str.indexOf('#');
    index4 = str.indexOf('$');
    index5 = str.indexOf('%');
    index6 = str.indexOf('^');
    index7 = str.indexOf('\n');
    S1String = str.substring(0, index1);
    S2String = str.substring(index1 + 1, index2);
    S3String = str.substring(index2 + 1, index3);
    S4String = str.substring(index3 + 1, index4);
    S5String = str.substring(index4 + 1, index5);
    S6String = str.substring(index5 + 1, index6);
    rembalString = str.substring(index6 + 1, index7);

    S1 = S1String.toInt();
    S2 = S2String.toInt();
    S3 = S3String.toInt();
    S4 = S4String.toInt();
    S5 = S5String.toInt();
    S6 = S6String.toInt();
    rembal = rembalString.toInt();

    Serial.println(S1);
    Serial.println(S2);
    Serial.println(S3);
    Serial.println(S4);
    Serial.println(S5);
    Serial.println(S6);
    Serial.println(rembal);
  }
}

void upload() {
  Firebase.setString(fbdo, "Slot1", String(S1));
  Firebase.setString(fbdo, "Slot2", String(S2));
  Firebase.setString(fbdo, "Slot3", String(S3));
  Firebase.setString(fbdo, "Slot4", String(S4));
  Firebase.setString(fbdo, "Slot5", String(S5));
  Firebase.setString(fbdo, "Slot6", String(S6));
  Firebase.setString(fbdo, "Remaining Balance", String(rembal));
}
