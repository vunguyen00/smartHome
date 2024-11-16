#include <Keypad.h>

// Định nghĩa Keypad 4x3
const byte ROW_NUM    = 4; // Số dòng
const byte COLUMN_NUM = 3; // Số cột

// Cấu hình các chân của Keypad
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};  // Chân kết nối với các dòng
byte pin_column[COLUMN_NUM] = {5, 4, 3}; // Chân kết nối với các cột

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

String password = "";  // Biến lưu trữ mật khẩu

void setup() {
  // Khởi tạo kết nối Serial với ESP8266
  Serial.begin(9600);  // Serial với ESP8266
}

void loop() {
  char key = keypad.getKey();  // Lấy phím nhấn

  if (key) {
    Serial.print(key);
  }
}
