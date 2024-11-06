#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "https://esp8266-9c680-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "lj98sgCzt1ufAQkiq6WO6fPHRReSep1zzweeTN0p"

// Thông tin kết nối WiFi
const char* ssid = "TP-Link_36AA";
const char* password = "29120574";

// Cấu hình Firebase
FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

// Cấu hình các chân điều khiển LED và cửa
const int ledPin1 = D1; // Chân điều khiển LED 1
const int ledPin2 = D2; // Chân điều khiển LED 2
const int ledPin3 = D3; // Chân điều khiển LED 3
const int doorPin = D5; // Chân điều khiển cửa

// Cấu hình NTP (Network Time Protocol) để lấy thời gian thực
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200); // Thiết lập múi giờ UTC +7

unsigned long previousMillis = 0;
const long interval = 2000; // Khoảng thời gian (ms) để lấy dữ liệu từ Firebase

void setup() {
  Serial.begin(9600);

  // Cài đặt các chân LED và cửa ở chế độ đầu ra
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(doorPin, OUTPUT);
  
  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Cấu hình Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Khởi động NTP để đồng bộ thời gian thực
  timeClient.begin();
}

void loop() {
  // Cập nhật thời gian thực từ NTP
  timeClient.update();
  String currentTime = timeClient.getFormattedTime(); // Lấy thời gian hiện tại ở định dạng HH:MM:SS

  // Kiểm tra nếu đã đủ thời gian (khoảng `interval`) để lấy dữ liệu từ Firebase
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Điều khiển các đèn LED với ưu tiên điều khiển thủ công
    controlLED("led1", ledPin1);
    controlLED("led2", ledPin2);
    controlLED("led3", ledPin3);

    // Lấy trạng thái của cửa từ Firebase và điều khiển chân cửa
    String doorStatus = Firebase.getString(firebaseData, "/door/status") ? firebaseData.stringData() : "";
    digitalWrite(doorPin, doorStatus == "open" ? HIGH : LOW);
    Serial.print("Door status: "); Serial.println(doorStatus);
  }

  delay(1000); // Chờ 1 giây trước khi lặp lại
}

// Hàm điều khiển LED với ưu tiên cho điều khiển thủ công
void controlLED(const String& ledPath, int ledPin) {
  // Lấy trạng thái LED từ Firebase
  String ledStatus = Firebase.getString(firebaseData, "/led/" + ledPath + "/status") ? firebaseData.stringData() : "";
  // Lấy trạng thái điều khiển thủ công từ Firebase
  bool manualControl = Firebase.getBool(firebaseData, "/led/" + ledPath + "/manualControl") ? firebaseData.boolData() : false;

  // Ưu tiên điều khiển thủ công: Nếu manualControl = true, đèn sẽ bật/tắt theo điều khiển thủ công
  if (manualControl) {
    // Điều khiển thủ công, giữ nguyên trạng thái do người dùng chọn
    digitalWrite(ledPin, ledStatus == "on" ? HIGH : LOW);
    Serial.print(ledPath + " is under manual control. Status: "); Serial.println(ledStatus);
  } else {
    // Không có điều khiển thủ công, đèn giữ trạng thái hiện tại
    digitalWrite(ledPin, ledStatus == "on" ? HIGH : LOW);
    Serial.print(ledPath + " automatic mode. Status: "); Serial.println(ledStatus);
  }
}

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
    String doorStatus = Firebase.getString(firebaseData, "/door/status") ? firebaseData.stringData() : "";
    digitalWrite(doorPin, doorStatus == "open" ? HIGH : LOW);
    Serial.print("Door status: "); Serial.println(doorStatus);
  }
  delay(1000); // Wait 1 second before the next loop iteration
} 