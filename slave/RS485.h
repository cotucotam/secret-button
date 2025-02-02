#ifndef RS485_H
#define RS485_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class RS485 {
private:
    SoftwareSerial rs485Serial; // Đối tượng SoftwareSerial
    int rxPin;
    int txPin;
    int baudRate;

public:
    // Constructor
    RS485(int rx, int tx, int baud);

    // Khởi tạo RS485
    void begin();

    // Gửi dữ liệu qua RS485
    void send(const String &data);

    // Nhận dữ liệu qua RS485
    String receive();

    static void parseData(String data, int &slaveID, int* buttonValues);
};

#endif
