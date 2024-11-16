#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

#define FIREBASE_HOST "esp8266-9c680-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "lj98sgCzt1ufAQkiq6WO6fPHRReSep1zzweeTN0p"

// WiFi credentials
const char* ssid = "Xom Tro Vui Ve";
const char* password = "Hoang123";

// Firebase variables
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

// Pin configuration
const int ledPin1 = D1;
const int ledPin2 = D2;
const int ledPin3 = D3;
const int ledPin4 = D4;  // LED4 Pin declaration
const int doorServoPin = D5;
const int pirPin = D6;  // PIR sensor connected to D6

// Servo setup
Servo doorServo;

// Time interval for getting Firebase data
unsigned long previousMillis = 0;
const long interval = 2000;
unsigned long pirActiveMillis = 0;  // Timer for PIR active state
bool pirMotionDetected = false;  // Flag to track motion detection

// Variable to store entered password from Arduino
String enteredPassword = ""; 

void setup() {
  Serial.begin(9600); // Start Serial communication for debugging
  pinMode(ledPin1, OUTPUT); 
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);  // Set LED4 pin as output
  pinMode(pirPin, INPUT);  // Set PIR sensor pin as input

  doorServo.attach(doorServoPin);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Get LED and door status from Firebase
    String ledStatus1 = Firebase.getString(firebaseData, "/led/led1/status") ? firebaseData.stringData() : "";
    String ledStatus2 = Firebase.getString(firebaseData, "/led/led2/status") ? firebaseData.stringData() : "";
    String ledStatus3 = Firebase.getString(firebaseData, "/led/led3/status") ? firebaseData.stringData() : "";
    String doorStatus = Firebase.getString(firebaseData, "/door/status") ? firebaseData.stringData() : "";
    String pirStatus = Firebase.getString(firebaseData, "/pir/status") ? firebaseData.stringData() : "";  // Get PIR status from Firebase

    // Control LEDs from Firebase
    digitalWrite(ledPin1, (ledStatus1 == "on") ? HIGH : LOW);
    digitalWrite(ledPin2, (ledStatus2 == "on") ? HIGH : LOW);
    digitalWrite(ledPin3, (ledStatus3 == "on") ? HIGH : LOW);

    // Control door from Firebase
    if (doorStatus == "open") {
      doorServo.write(180);
    } else {
      doorServo.write(0);
    }

    // Check if PIR is active and detect motion
    if (pirStatus == "active") {
      // Start detecting motion if PIR is active
      int pirStatusValue = digitalRead(pirPin);  // Read the PIR sensor

      if (pirStatusValue == HIGH && !pirMotionDetected) {
        // If motion detected, turn on LED4 and start the timer
        digitalWrite(ledPin4, HIGH);
        pirMotionDetected = true;  // Flag motion detected
        pirActiveMillis = currentMillis; // Start timer for 10 seconds
      }

      // Turn off LED4 after 10 seconds if motion was detected
      if (pirMotionDetected && currentMillis - pirActiveMillis >= 10000) {
        digitalWrite(ledPin4, LOW);
        pirMotionDetected = false;  // Reset motion detection flag
      }
    } else {
      // If PIR is not active, ensure LED4 is turned off
      digitalWrite(ledPin4, LOW);
      pirMotionDetected = false;  // Reset motion detection flag
    }

    // Debug output
    Serial.print("LED 1: "); Serial.println(digitalRead(ledPin1) ? "on" : "off");
    Serial.print("LED 2: "); Serial.println(digitalRead(ledPin2) ? "on" : "off");
    Serial.print("LED 3: "); Serial.println(digitalRead(ledPin3) ? "on" : "off");
    Serial.print("LED 4: "); Serial.println(digitalRead(ledPin4) ? "on" : "off");
    Serial.print("Door: "); Serial.println(doorStatus);
    Serial.print("PIR Status: "); Serial.println(pirStatus);
  }

  // Get entered password from Arduino (via Serial)
  if (Serial.available() > 0) {
    enteredPassword = Serial.readString();  // Get the input password
    enteredPassword.trim();  // Remove any extra spaces or newlines

    // Debugging: Print the entered password before conversion
    Serial.print("Entered password (before conversion): ");
    Serial.println(enteredPassword);

    // Convert entered password to integer
    int enteredPasswordInt = enteredPassword.toInt();  // Convert entered password to integer
    Serial.print("Entered password as integer: ");
    Serial.println(enteredPasswordInt);  // Print entered password as integer

    // Retrieve the stored password from Firebase
    String storedPassword = Firebase.getString(firebaseData, "/door/password") ? firebaseData.stringData() : "";
    storedPassword.trim();  // Remove any extra spaces or newlines from stored password

    // Convert stored password to integer
    int storedPasswordInt = storedPassword.toInt();  // Convert stored password from Firebase to integer
    Serial.print("Stored password as integer: ");
    Serial.println(storedPasswordInt);  // Print stored password as integer

    // Compare the two integer passwords
    if (enteredPasswordInt == storedPasswordInt) {
      Serial.println("Password correct. Door opening...");
      doorServo.write(180);  // Open the door
      delay(20000);  // Keep door open for 2 seconds
      doorServo.write(0);  // Close the door
    } else {
      Serial.println("Password incorrect.");
    }
  }

  delay(10000); // Delay before next loop iteration
}
