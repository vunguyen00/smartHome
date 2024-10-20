#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Firebase credentials
#define FIREBASE_HOST "esp8266-9c680-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "lj98sgCzt1ufAQkiq6WO6fPHRReSep1zzweeTN0p"

// WiFi credentials
const char* ssid = "TP_Link-36AA";
const char* password = "29120574";

// Firebase variables
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

// Pin configuration
const int ledPin1 = D1; // Pin for LED 1
const int ledPin2 = D2; // Pin for LED 2
const int ledPin3 = D3; // Pin for LED 3
const int doorPin = D5; // Pin for the door

// NTP setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Time interval for getting Firebase data
unsigned long previousMillis = 0;
const long interval = 2000; // Interval to read from Firebase

void setup() {
  Serial.begin(9600);
  pinMode(ledPin1, OUTPUT); // Set LED pins as output
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(doorPin, OUTPUT); // Set door pin as output
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Configure Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // NTP setup
  timeClient.begin();
  timeClient.setTimeOffset(25200); // Adjust for timezone (UTC +7)
}

void loop() {
  // Update NTP client to get the current time
  timeClient.update();

  // Get the current time in a formatted string
  String formattedTime = timeClient.getFormattedTime();
  
  // Check if it's time to get the LED status from Firebase
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read LED statuses from Firebase
    if (Firebase.get(firebaseData, "/led/led1/status")) {
      String ledStatus1 = firebaseData.stringData();
      digitalWrite(ledPin1, ledStatus1 == "on" ? HIGH : LOW);
      Serial.print("LED 1 status: "); Serial.println(ledStatus1);
    } else {
      Serial.println("Failed to get LED 1 status from Firebase");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.get(firebaseData, "/led/led2/status")) {
      String ledStatus2 = firebaseData.stringData();
      digitalWrite(ledPin2, ledStatus2 == "on" ? HIGH : LOW);
      Serial.print("LED 2 status: "); Serial.println(ledStatus2);
    } else {
      Serial.println("Failed to get LED 2 status from Firebase");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.get(firebaseData, "/led/led3/status")) {
      String ledStatus3 = firebaseData.stringData();
      digitalWrite(ledPin3, ledStatus3 == "on" ? HIGH : LOW);
      Serial.print("LED 3 status: "); Serial.println(ledStatus3);
    } else {
      Serial.println("Failed to get LED 3 status from Firebase");
      Serial.println(firebaseData.errorReason());
    }

    // Control door status from Firebase
    if (Firebase.get(firebaseData, "/door/status")) {
      String doorStatus = firebaseData.stringData();
      digitalWrite(doorPin, doorStatus == "open" ? HIGH : LOW);
      Serial.print("Door status: "); Serial.println(doorStatus);
    } else {
      Serial.println("Failed to get door status from Firebase");
      Serial.println(firebaseData.errorReason());
    }

    // Print current time
    Serial.print("Current Time: "); Serial.println(formattedTime);
  }

  delay(1000); 
}