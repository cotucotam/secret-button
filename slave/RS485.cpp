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
    Serial.println("RS485 Initialized");
}

// Gửi dữ liệu qua RS485
void RS485::send(const String &data) {
    rs485Serial.println(data);
    Serial.println("Sent via RS485: " + data);
}

// Nhận dữ liệu qua RS485
String RS485::receive() {
    String data = "";
    // Serial.println("Received via RS485: " + data);
    if (rs485Serial.available()) {
        data = rs485Serial.readString();

        Serial.println("Received via RS485: " + data);
    }
    return data;
}

void RS485::parseData(String data, int &slaveID, int* buttonValues) {
  // Kiểm tra chuỗi có bắt đầu bằng "S" không
    if (data.length() < 2 || data[0] != 'S') {
        // Serial.println("Dữ liệu không hợp lệ");
        return;
    }

    // Lấy Slave ID
    slaveID = data.substring(1, 2).toInt();
    // Serial.println("Slave ID: " + String(slaveId));

    // Xử lý các button và giá trị
    int startIndex = 2; // Bỏ qua "S0"
    while (startIndex < data.length()) {
        if (data[startIndex] == 'B') {
            // Lấy chỉ số nút (XX)
            int buttonIndex = data.substring(startIndex + 1, startIndex + 3).toInt() - 1;

            // Lấy giá trị nút (V)
            int value = data[startIndex + 3] - '0'; // Chuyển ký tự số sang số nguyên

            // Lưu giá trị vào mảng nếu hợp lệ
            if (buttonIndex >= 0 && buttonIndex < NUM_BUTTONS) {
                buttonValues[buttonIndex] = value;
            } else {
                // Serial.println("Button index không hợp lệ: " + String(buttonIndex));
            }

            // Tiến tới mục tiếp theo
            startIndex += 4; // "BXXV" có 4 ký tự
        } else {
            // Serial.println("Dữ liệu không hợp lệ tại: " + String(startIndex));
            break;
        }
    }
}