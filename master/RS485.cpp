#include "RS485.h"
#include "PinDefinitions.h"

// Constructor
RS485::RS485(int rx, int tx, int baud) : rs485Serial(rx, tx) {
    rxPin = rx;
    txPin = tx;
    baudRate = baud;
}

// Khởi tạo RS485
void RS485::begin() {
    rs485Serial.begin(baudRate);
}

// Gửi dữ liệu qua RS485
void RS485::send(const String &data) {
    rs485Serial.println(data);
}

// Nhận dữ liệu qua RS485
String RS485::receive() {
    String data = "";
    if (rs485Serial.available()) {
        data = rs485Serial.readString();
    }
    return data;
}

// Phân tích dữ liệu nhận được
void RS485::parseData(String data, int* buttonValues) {
    // Kiểm tra tiền tố và bỏ qua nếu cần
    if (data.length() < 2 || data[0] != 'S') {
        Serial.println("Dữ liệu không hợp lệ");
        return;
    }
    String prefix = data.substring(0, 2); // Tiền tố (vd: "S0")

    // Bắt đầu xử lý từ sau tiền tố (bỏ qua "S0")
    int startIndex = 2; // Bỏ qua tiền tố
    while (startIndex < data.length()) {
        // Tách "BXX" và giá trị
        if (data[startIndex] == 'B') {
            // Lấy số thứ tự của button (XX)
            int buttonIndex = data.substring(startIndex + 1, startIndex + 3).toInt() - 1;

            // Lấy giá trị (sau "BXX")
            int value = data[startIndex + 3] - '0'; // Chuyển ký tự số thành số nguyên

            // Lưu giá trị vào mảng nếu hợp lệ
            if (buttonIndex >= 0 && buttonIndex < NUM_BUTTONS) {
                buttonValues[buttonIndex] = value;
            }

            // Tiến đến phần tiếp theo
            startIndex += 4; // Mỗi mục có dạng "BXXV" (4 ký tự)
        } else {
            Serial.println("Dữ liệu không hợp lệ tại: " + String(startIndex));
            break;
        }
    }
}
