
// int led = 2;
// void setup() {
//   Serial.begin(9600); // Dùng UART mặc định (TX = 1, RX = 0)
//   Serial.println("RS485 Master is ready");
//   
//   pinMode(led,OUTPUT); // Cài đặt chân D2 dưới dạng OUTPUT
// }

// void loop() {
//   int buttonStatus = digitalRead(button);    //Đọc trạng thái button
//   if (buttonStatus == LOW) { // Nếu mà button bị nhấn
//     digitalWrite(led,HIGH); // Đèn led sáng
//     Serial.write("1");
//     Serial.println("11111111");
//   } else { // ngược lại
//     digitalWrite(led,LOW);
//     Serial.write("0");
//     Serial.println("22222222");
//   }
// }
#include <SoftwareSerial.h>

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

    dataToSend = "0";
    rs485Serial.println(dataToSend); // Gửi qua RS485
    Serial.println("Sent: " + dataToSend); // Debug trên Serial Monitor
    delay(1000);
  } else if (buttonStatus == HIGH) { // ngược lại
    dataToSend = "1";
    rs485Serial.println(dataToSend); // Gửi qua RS485
    Serial.println("Sent: " + dataToSend); // Debug trên Serial Monitor
    delay(1000);
  }
}
