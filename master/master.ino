#include <SoftwareSerial.h>
#include "PinDefinitions.h"
#include <Arduino_FreeRTOS.h>
// RS485 chân
#define RX_PIN 10
#define TX_PIN 11
const int button = A1;

SoftwareSerial rs485Serial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(19200);       // Serial mặc định cho máy tính
  rs485Serial.begin(19200);  // SoftwareSerial cho RS485
  pinMode(button, INPUT);  //Cài đặt chân D11 ở trạng thái đọc dữ liệu
  Serial.println("Master Ready");
}

void loop() {
  String dataToSend = "1";


  int buttonStatus = digitalRead(button);    //Đọc trạng thái button
  if (buttonStatus == LOW) { // Nếu mà button bị nhấn

    dataToSend = "S1-N1:0";
    rs485Serial.println(dataToSend); // Gửi qua RS485
    Serial.println("Sent: " + dataToSend); // Debug trên Serial Monitor
    delay(1000);
  } else if (buttonStatus == HIGH) { // ngược lại
    dataToSend = "S1-N1:1";
    rs485Serial.println(dataToSend); // Gửi qua RS485
    Serial.println("Sent: " + dataToSend); // Debug trên Serial Monitor
    delay(1000);
  }
}
