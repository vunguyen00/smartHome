#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "https://esp8266-9c680-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "lj98sgCzt1ufAQkiq6WO6fPHRReSep1zzweeTN0p"

// WiFi credentials
const char* ssid = "TP-Link_36AA";
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
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200); // UTC +7 for timezone adjustment

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
}

void loop() {
  // Update NTP client to get the current time
  timeClient.update();
  String currentTime = timeClient.getFormattedTime();

  // Check if it's time to get the LED status from Firebase
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // LED control for each LED with manual override
    controlLED("led1", ledPin1, currentTime);
    controlLED("led2", ledPin2, currentTime);
    controlLED("led3", ledPin3, currentTime);

    // Control door status from Firebase
    String doorStatus = Firebase.getString(firebaseData, "/door/status") ? firebaseData.stringData() : "";
    digitalWrite(doorPin, doorStatus == "open" ? HIGH : LOW);
    Serial.print("Door status: "); Serial.println(doorStatus);
  }

  delay(1000); // Wait 1 second before the next loop iteration
}

// Function to control LED with schedule and manual control
void controlLED(const String& ledPath, int ledPin, const String& currentTime) {
  String ledStatus = Firebase.getString(firebaseData, "/led/" + ledPath + "/status") ? firebaseData.stringData() : "";
  bool manualControl = Firebase.getBool(firebaseData, "/led/" + ledPath + "/manualControl") ? firebaseData.boolData() : false;
  String onTime = Firebase.getString(firebaseData, "/led/" + ledPath + "/schedule/onTime") ? firebaseData.stringData() : "";
  String offTime = Firebase.getString(firebaseData, "/led/" + ledPath + "/schedule/offTime") ? firebaseData.stringData() : "";

  // Check if LED should be on based on schedule
  bool scheduledOn = (currentTime >= onTime && currentTime < offTime);

  if (manualControl) {
    // If manual control is enabled, use the manual status
    digitalWrite(ledPin, ledStatus == "on" ? HIGH : LOW);
    Serial.print(ledPath + " is under manual control. Status: "); Serial.println(ledStatus);
  } else {
    // Follow the schedule if manual control is not active
    digitalWrite(ledPin, scheduledOn ? HIGH : LOW);
    Serial.print(ledPath + " is scheduled. Current time: "); Serial.print(currentTime);
    Serial.print(" | On time: "); Serial.print(onTime); Serial.print(" | Off time: "); Serial.println(offTime);
  }

  // Reset manual control if it was activated manually
  if (manualControl && !scheduledOn) {
    Firebase.setBool(firebaseData, "/led/" + ledPath + "/manualControl", false);
  }
}
