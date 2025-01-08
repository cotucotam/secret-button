#ifndef PINDEFINITIONS_H
#define PINDEFINITIONS_H

#include <Arduino.h>

#define SLAVE_ID 0

// Các chân điều khiển BUTTON (20 chân)
#define BUTTON_PIN_1 A1
#define BUTTON_PIN_2 23
#define BUTTON_PIN_3 24
#define BUTTON_PIN_4 25
#define BUTTON_PIN_5 26
#define BUTTON_PIN_6 27
#define BUTTON_PIN_7 28
#define BUTTON_PIN_8 29
#define BUTTON_PIN_9 30
#define BUTTON_PIN_10 31
#define BUTTON_PIN_11 32
#define BUTTON_PIN_12 33
#define BUTTON_PIN_13 34
#define BUTTON_PIN_14 35
#define BUTTON_PIN_15 36
#define BUTTON_PIN_16 37
#define BUTTON_PIN_17 38
#define BUTTON_PIN_18 39
#define BUTTON_PIN_19 40
#define BUTTON_PIN_20 41

// Số lượng BUTTON
#define NUM_BUTTONS 20

// Khai báo mảng BUTTONPins mà không khởi tạo
extern int buttonPins[NUM_BUTTONS];

// Các chân giao tiếp (Serial)
#define RX_PIN 10
#define TX_PIN 11

#endif
