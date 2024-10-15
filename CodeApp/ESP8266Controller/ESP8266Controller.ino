#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "https://esp8266-9c680-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "lj98sgCzt1ufAQkiq6WO6fPHRReSep1zzweeTN0p"

// WiFi credentials
const char* ssid = "Xom Tro Vui Ve";
const char* password = "Hoang123";

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
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Configure Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
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
    String ledStatus1 = Firebase.getString(firebaseData, "/led/led1/status") ? firebaseData.stringData() : "";
    String ledStatus2 = Firebase.getString(firebaseData, "/led/led2/status") ? firebaseData.stringData() : "";
    String ledStatus3 = Firebase.getString(firebaseData, "/led/led3/status") ? firebaseData.stringData() : "";

    // Update LED states
    digitalWrite(ledPin1, ledStatus1 == "on" ? HIGH : LOW);
    digitalWrite(ledPin2, ledStatus2 == "on" ? HIGH : LOW);
    digitalWrite(ledPin3, ledStatus3 == "on" ? HIGH : LOW);

    // Debug output
    Serial.print("LED 1 status: "); Serial.println(ledStatus1);
    Serial.print("LED 2 status: "); Serial.println(ledStatus2);
    Serial.print("LED 3 status: "); Serial.println(ledStatus3);
    Serial.print("Current Time: "); Serial.println(formattedTime);
  }

  // Control door status from Firebase
  String doorStatus = Firebase.getString(firebaseData, "/door/status") ? firebaseData.stringData() : "";
  digitalWrite(doorPin, doorStatus == "open" ? HIGH : LOW);

  // Debug output
  Serial.print("Door status: "); Serial.println(doorStatus);
  
  delay(1000);
}
