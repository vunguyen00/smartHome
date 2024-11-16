#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Định nghĩa LCD I2C (địa chỉ 0x27, LCD 20x4)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Định nghĩa Keypad 4x3
const byte ROW_NUM    = 4; // Số dòng
const byte COLUMN_NUM = 3; // Số cột

// Cấu hình các phím của Keypad
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};   // Chân kết nối với các dòng
byte pin_column[COLUMN_NUM] = {5, 4, 3}; // Chân kết nối với các cột

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

String password = ""; // Biến lưu trữ mật khẩu

void setup() {
  // Khởi tạo LCD
  lcd.init(); // Bắt đầu LCD
  lcd.backlight(); // Bật đèn nền LCD

  // Hiển thị thông báo ban đầu
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  // Khởi tạo Serial
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey(); // Lấy phím bấm

  if (key) { // Nếu có phím được nhấn
    // Hiển thị phím nhấn lên Serial Monitor
    Serial.print(key);

    // Hiển thị phím nhấn lên LCD
    lcd.setCursor(password.length(), 1); // Di chuyển con trỏ theo độ dài password
    lcd.print(key);

    // Lưu phím vào biến mật khẩu
    password += key;

    // Kiểm tra khi bấm '#'
    if (key == '#') {
      lcd.setCursor(0, 2); // Xuống dòng tiếp theo
      lcd.print("Password Sent!");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter Password:");
      password = ""; // Reset mật khẩu
    }
  }
}
