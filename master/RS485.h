#ifndef RS485_H
#define RS485_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "PinDefinitions.h"

class RS485 {
private:
    int rxPin;              // Chân RX
    int txPin;              // Chân TX
    int baudRate;           // Tốc độ truyền
    SoftwareSerial rs485Serial; // Giao tiếp RS485

public:
    // Constructor
    RS485(int rx, int tx, int baud);

    // Khởi tạo giao tiếp RS485
    void begin();

    // Gửi dữ liệu qua RS485
    void send(const String &data);

    // Nhận dữ liệu qua RS485
    String receive();

    // Phân tích dữ liệu nhận được
    void parseData(String data, int* buttonValues);
};

#endif
