#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

class Relay {
private:
    int pin;        // Chân điều khiển relay
    bool activeLow; // Relay kích hoạt ở mức thấp hay cao

public:
    // Constructor
    Relay(int pin, bool activeLow = true);

    // Phương thức khởi tạo
    void begin();

    // Bật relay
    void on();

    // Tắt relay
    void off();

    // Kiểm tra trạng thái hiện tại
    bool isOn();
};

#endif
