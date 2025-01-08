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
    //Serial.println("RS485 Initialized");
}

// Gửi dữ liệu qua RS485
void RS485::send(const String &data) {
    rs485Serial.println(data);
    //Serial.println("Sent via RS485: " + data);
}

// Nhận dữ liệu qua RS485
String RS485::receive() {
    String data = "";
    Serial.println("Received via RS485: " + data);
    if (rs485Serial.available()) {
        data = rs485Serial.readString();

        Serial.println("Received via RS485: " + data);
    }
    return data;
}

void RS485::parseData(String data, int* buttonValues) {
    // Tách tiền tố S1
    String prefix = data.substring(0, data.indexOf("-")); // Lấy phần "S1"
    // Serial.println("Prefix: " + prefix);                 // Debug để in tiền tố

    // Bắt đầu xử lý từ sau tiền tố
    int startIndex = data.indexOf("-") + 1; // Vị trí bắt đầu của "B01"
    while (startIndex > 0 && startIndex < data.length()) {
        // Tìm vị trí kết thúc của cặp "BXX:Value"
        int endIndex = data.indexOf("-", startIndex);
        if (endIndex == -1) {
            endIndex = data.length(); // Nếu không tìm thấy "-", lấy đến cuối chuỗi
        }

        // Tách cặp "BXX:Value"
        String pair = data.substring(startIndex, endIndex);

        // Tách số thứ tự button "BXX" và giá trị sau ":"
        int colonIndex = pair.indexOf(":");
        int buttonIndex = pair.substring(1, colonIndex).toInt() - 1; // Lấy số thứ tự (XX) và chuyển về chỉ số mảng
        int value = pair.substring(colonIndex + 1).toInt();          // Lấy giá trị sau dấu ":"

        // Lưu giá trị vào mảng
        if (buttonIndex >= 0 && buttonIndex < NUM_BUTTONS) {
            buttonValues[buttonIndex] = value;
        }

        // Tiến đến phần tiếp theo
        startIndex = data.indexOf("-", endIndex) + 1;
    }
}
