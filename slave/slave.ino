// void setup() {
//   Serial.begin(9600); // Dùng UART mặc định (TX = 1, RX = 0)
//   Serial.println("RS485 Slave is ready");
// }

// void loop() {
//   if (Serial.available()) {
//     String received = Serial.readString();
//     char data_rcvd = Serial.read();
//     if(data_rcvd == "0"){
//       Serial.print("Received: 0 ");
//     }
//     else{
//       Serial.print("Received: 1 ");
//     }
//   }
// }

// #include <SoftwareSerial.h>

// // RS485 chân
// #define RX_PIN 10
// #define TX_PIN 11
// SoftwareSerial rs485Serial(RX_PIN, TX_PIN);

// #define Relay 1

// void setup() {
//   Serial.begin(19200);       // Serial mặc định cho máy tính
//   rs485Serial.begin(19200);  // SoftwareSerial cho RS485
//   Serial.println("Slave Ready");

//   pinMode(Relay, OUTPUT);
//   digitalWrite(Relay, HIGH);
// }

// void loop() {
//   // if (rs485Serial.available()) {
//   //   String receivedData = rs485Serial.readString();
//   //   Serial.println("Received: " + receivedData); // Hiển thị dữ liệu nhận được
//   //   if(receivedData == "1"){

//   //   }
//   //   // Phản hồi lại Master
//   //   // String response = "Ack: " + receivedData;
//   //   // rs485Serial.println(response);
//   //   // Serial.println("Sent: " + response);
//   // }

//   digitalWrite(Relay, LOW);
//   Serial.println("on");
//   delay(3000);
//   digitalWrite(Relay, HIGH);
//   Serial.println("off");
//   delay(3000);

// }


#include "RS485.h"
#include "Relay.h"
#include "PinDefinitions.h"

// Tạo đối tượng RS485
RS485 rs485(RX_PIN, TX_PIN, 19200);

// Khởi tạo mảng relayPins
int relayPins[NUM_RELAYS] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5,
                             RELAY_PIN_6, RELAY_PIN_7, RELAY_PIN_8, RELAY_PIN_9, RELAY_PIN_10,
                             RELAY_PIN_11, RELAY_PIN_12, RELAY_PIN_13, RELAY_PIN_14, RELAY_PIN_15,
                             RELAY_PIN_16, RELAY_PIN_17, RELAY_PIN_18, RELAY_PIN_19, RELAY_PIN_20};

// Mảng các đối tượng Relay
Relay* relays[NUM_RELAYS];

// Hàm setupRelays để quét mảng relayPins và khởi tạo các đối tượng Relay
void setupRelays() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        relays[i] = new Relay(relayPins[i], true); // Sử dụng `new` để khởi tạo đối tượng
        relays[i]->begin(); // Khởi tạo mỗi relay
        relays[i]->off();
    }
}

void setup() {
    Serial.begin(19200); // Serial mặc định cho máy tính
    rs485.begin();       // Khởi tạo RS485

    // Khởi tạo relay
    Serial.println("Relay initialized");
    setupRelays();
    
    

    // // Bật relay trong 3 giây, sau đó tắt
    // relay.on();
    // Serial.println("Relay turned ON");
    // delay(3000);

    // relay.off();
    // Serial.println("Relay turned OFF");

    Serial.println("Slave Ready");
}

void loop() {
    // Nhận dữ liệu từ RS485
    String receivedData = rs485.receive();

    // Kiểm tra dữ liệu nhận được
    if (receivedData.length() > 0) {
        if (receivedData == "1") {
            relays[0]->on(); 
            Serial.println("Relay ON");
        } else if (receivedData == "0") {
            relays[0]->off();
            Serial.println("Relay OFF");
        }
    }

    // (Tùy chọn) Gửi phản hồi về Master
    rs485.send("ACK: " + receivedData);
    delay(100); // Để tránh giao tiếp quá nhanh
}

