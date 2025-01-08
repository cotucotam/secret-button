#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
private:
    int pin;         // Chân của button
    bool activeLow;  // Button kích hoạt ở mức thấp hay cao
    bool lastState;  // Lưu trữ trạng thái button trước đó

public:
    // Constructor
    Button(int pin, bool activeLow = true);

    // Khởi tạo
    void begin();

    // Kiểm tra xem button có được nhấn hay không
    bool isPressed();

    // Kiểm tra sự thay đổi trạng thái
    bool stateChanged();

    // Lấy trạng thái hiện tại của button
    bool getState();
};

#endif