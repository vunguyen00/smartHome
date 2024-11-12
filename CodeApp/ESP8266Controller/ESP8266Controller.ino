#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h> // Thư viện Servo

#define FIREBASE_HOST "esp8266-9c680-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "lj98sgCzt1ufAQkiq6WO6fPHRReSep1zzweeTN0p"

// WiFi credentials
const char* ssid = "MAT XA";
const char* password = "22228888";

// Firebase variables
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

// Pin configuration
const int ledPin1 = D1; // Pin for LED 1
const int ledPin2 = D2; // Pin for LED 2
const int ledPin3 = D3; // Pin for LED 3
const int doorServoPin = D5; // Pin for the door servo

// NTP setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Servo setup
Servo doorServo; // Servo object for controlling the door

// Time interval for getting Firebase data
unsigned long previousMillis = 0;
const long interval = 2000; // Interval to read from Firebase

void setup() {
  Serial.begin(9600);
  pinMode(ledPin1, OUTPUT); // Set LED pins as output
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  
  // Attach servo to the specified pin
  doorServo.attach(doorServoPin);

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
  String currentTime = timeClient.getFormattedTime().substring(0, 5); // Lấy HH:MM
  
  // Check if it's time to get the LED and door status from Firebase
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read LED statuses from Firebase
    String ledStatus1 = Firebase.getString(firebaseData, "/led/led1/status") ? firebaseData.stringData() : "";
    String ledStatus2 = Firebase.getString(firebaseData, "/led/led2/status") ? firebaseData.stringData() : "";
    String ledStatus3 = Firebase.getString(firebaseData, "/led/led3/status") ? firebaseData.stringData() : "";

    // Lấy thời gian hẹn giờ từ Firebase
    String led1OnTime = Firebase.getString(firebaseData, "/led/led1/onTime") ? firebaseData.stringData() : "";
    String led1OffTime = Firebase.getString(firebaseData, "/led/led1/offTime") ? firebaseData.stringData() : "";
    String led2OnTime = Firebase.getString(firebaseData, "/led/led2/onTime") ? firebaseData.stringData() : "";
    String led2OffTime = Firebase.getString(firebaseData, "/led/led2/offTime") ? firebaseData.stringData() : "";
    String led3OnTime = Firebase.getString(firebaseData, "/led/led3/onTime") ? firebaseData.stringData() : "";
    String led3OffTime = Firebase.getString(firebaseData, "/led/led3/offTime") ? firebaseData.stringData() : "";

    // Kiểm tra và cập nhật trạng thái LED theo hẹn giờ
    digitalWrite(ledPin1, (currentTime == led1OnTime) ? HIGH : ((currentTime == led1OffTime) ? LOW : digitalRead(ledPin1)));
    digitalWrite(ledPin2, (currentTime == led2OnTime) ? HIGH : ((currentTime == led2OffTime) ? LOW : digitalRead(ledPin2)));
    digitalWrite(ledPin3, (currentTime == led3OnTime) ? HIGH : ((currentTime == led3OffTime) ? LOW : digitalRead(ledPin3)));

    // Control door status from Firebase
    String doorStatus = Firebase.getString(firebaseData, "/door/status") ? firebaseData.stringData() : "";
    if (doorStatus == "open") {
      doorServo.write(180); // Mở cửa (servo quay đến 90 độ)
    } else {
      doorServo.write(0); // Đóng cửa (servo quay về 0 độ)
    }

    // Debug output for LED and door statuses
    Serial.print("LED 1 status: "); Serial.println(digitalRead(ledPin1) ? "on" : "off");
    Serial.print("LED 2 status: "); Serial.println(digitalRead(ledPin2) ? "on" : "off");
    Serial.print("LED 3 status: "); Serial.println(digitalRead(ledPin3) ? "on" : "off");
    Serial.print("Door status: "); Serial.println(doorStatus);
    Serial.print("Current Time: "); Serial.println(currentTime);
  }

  delay(1000);
}
