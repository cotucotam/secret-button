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
    String prefix = data.substring(0, 2);  // Lấy phần "S1" hoặc "S2"

    int startIndex = data.indexOf("N");
    int endIndex = data.indexOf("-", startIndex);

    for (int i = 0; i < NUM_BUTTONS; i++) {
        String pair = data.substring(startIndex, endIndex);
        int colonIndex = pair.indexOf(":");
        int value = pair.substring(colonIndex + 1).toInt();

        buttonValues[i] = value;

        startIndex = data.indexOf("N", startIndex + 1);
        endIndex = data.indexOf("-", startIndex);
    }
}
