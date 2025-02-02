#ifndef PINDEFINITIONS_H
#define PINDEFINITIONS_H

#include <Arduino.h>

#define SLAVE_ID 0

// Các chân điều khiển BUTTON (20 chân)
#define BUTTON_PIN_1 26
#define BUTTON_PIN_2 27
#define BUTTON_PIN_3 28
#define BUTTON_PIN_4 29
#define BUTTON_PIN_5 30
#define BUTTON_PIN_6 31
#define BUTTON_PIN_7 32
#define BUTTON_PIN_8 33
#define BUTTON_PIN_9 34
#define BUTTON_PIN_10 35
#define BUTTON_PIN_11 36
#define BUTTON_PIN_12 37
#define BUTTON_PIN_13 38
#define BUTTON_PIN_14 39
#define BUTTON_PIN_15 40

// Số lượng BUTTON
#define NUM_BUTTONS 15

// #define SLAVE_ID 1

// // Các chân điều khiển BUTTON (20 chân)
// #define BUTTON_PIN_1 26
// #define BUTTON_PIN_2 27
// #define BUTTON_PIN_3 28
// #define BUTTON_PIN_4 29


// // Số lượng BUTTON
// #define NUM_BUTTONS 4
// Khai báo mảng BUTTONPins mà không khởi tạo
extern int buttonPins[NUM_BUTTONS];

// Các chân giao tiếp (Serial)
#define RX_PIN 10
#define TX_PIN 11

// Reset value 
#define RESET_VALUE_RX 49  // Mã ASCII của "1" là 49
#define RESET_VALUE_TX "1"  // Mã ASCII của "1" là 49

#endif
