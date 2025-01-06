#include "Relay.h"

// Constructor
Relay::Relay(int pin, bool activeLow) {
    this->pin = pin;
    this->activeLow = activeLow;
}

// Khởi tạo relay
void Relay::begin() {
    pinMode(pin, OUTPUT);
    off(); // Đặt trạng thái ban đầu là tắt
}

// Bật relay
void Relay::on() {
    digitalWrite(pin, activeLow ? LOW : HIGH);
}

// Tắt relay
void Relay::off() {
    digitalWrite(pin, activeLow ? HIGH : LOW);
}

// Kiểm tra trạng thái hiện tại
bool Relay::isOn() {
    return digitalRead(pin) == (activeLow ? LOW : HIGH);
}
