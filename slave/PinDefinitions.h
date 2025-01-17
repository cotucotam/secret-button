#ifndef PINDEFINITIONS_H
#define PINDEFINITIONS_H

#define SLAVE_1_START_RELAY 0   // Relay bắt đầu cho Slave 1
#define SLAVE_1_END_RELAY 9     // Relay kết thúc cho Slave 1

#define SLAVE_2_START_RELAY 10  // Relay bắt đầu cho Slave 2
#define SLAVE_2_END_RELAY 19    // Relay kết thúc cho Slave 2

// Các chân điều khiển relay (20 chân)
#define RELAY_PIN_1 22
#define RELAY_PIN_2 23
#define RELAY_PIN_3 24
#define RELAY_PIN_4 25
#define RELAY_PIN_5 26
#define RELAY_PIN_6 27
#define RELAY_PIN_7 28
#define RELAY_PIN_8 29
#define RELAY_PIN_9 30
#define RELAY_PIN_10 31
#define RELAY_PIN_11 32
#define RELAY_PIN_12 33
#define RELAY_PIN_13 34
#define RELAY_PIN_14 35
#define RELAY_PIN_15 36
#define RELAY_PIN_16 37
#define RELAY_PIN_17 38
#define RELAY_PIN_18 39
#define RELAY_PIN_19 40
#define RELAY_PIN_20 41

#define RELAY_ON_VALUE 1
#define RELAY_OFF_VALUE 0

// Số lượng relay
#define NUM_RELAYS 20

// Số lượng relay
#define NUM_BUTTONS 20

// Reset button
#define RESET_BUTTON_PIN A2

// Reset value 
#define RESET_VALUE_RX 1  // Mã ASCII của "1" là 49
#define RESET_VALUE_TX "1"  // Mã ASCII của "1" là 49

// Khai báo mảng relayPins mà không khởi tạo
extern int relayPins[NUM_RELAYS];

// Các chân giao tiếp (Serial)
#define RX_PIN_S1 12
#define TX_PIN_S1 13

#define RX_PIN_S2 10
#define TX_PIN_S2 11

#endif
