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
  // Tách S1 hoặc S2
  String prefix = data.substring(0, 2);  // Lấy phần "S1" hoặc "S2"
  //Serial.println("Prefix: " + prefix);  // In phần "S1" hoặc "S2"

  // Tách các phần "N1:0", "N2:0", ..., "N4:0"
  int startIndex = data.indexOf("N");
  int endIndex = data.indexOf("-");

  // Duyệt qua tất cả các phần N
  for (int i = 0; i < NUM_RELAYS; i++) {
    // Tìm chuỗi "N1:0", "N2:0" v.v.
    String pair = data.substring(startIndex, endIndex);  // Lấy "N1:0"
    
    // Tách giá trị từ "N1:0" thành N1 và giá trị 0
    int colonIndex = pair.indexOf(":");
    int value = pair.substring(colonIndex + 1).toInt(); // Lấy giá trị sau dấu ":"

    // Lưu giá trị vào mảng thông qua con trỏ
    buttonValues[i] = value;

    // Tiến đến phần tiếp theo
    startIndex = data.indexOf("N", startIndex + 1); // Di chuyển chỉ mục đến phần N tiếp theo
    endIndex = data.indexOf("-", startIndex); // Tìm dấu "-" tiếp theo
  }
}
