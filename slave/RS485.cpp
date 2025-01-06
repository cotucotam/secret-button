#include "RS485.h"

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
    if (rs485Serial.available()) {
        data = rs485Serial.readString();
        Serial.println("Received via RS485: " + data);
    }
    return data;
}
