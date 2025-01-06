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

#include <SoftwareSerial.h>

// RS485 chân
#define RX_PIN 10
#define TX_PIN 11
SoftwareSerial rs485Serial(RX_PIN, TX_PIN);

#define Relay 1

void setup() {
  Serial.begin(19200);       // Serial mặc định cho máy tính
  rs485Serial.begin(19200);  // SoftwareSerial cho RS485
  Serial.println("Slave Ready");

  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, HIGH);
}

void loop() {
  // if (rs485Serial.available()) {
  //   String receivedData = rs485Serial.readString();
  //   Serial.println("Received: " + receivedData); // Hiển thị dữ liệu nhận được
  //   if(receivedData == "1"){

  //   }
  //   // Phản hồi lại Master
  //   // String response = "Ack: " + receivedData;
  //   // rs485Serial.println(response);
  //   // Serial.println("Sent: " + response);
  // }

  digitalWrite(Relay, LOW);
  Serial.println("on");
  delay(3000);
  digitalWrite(Relay, HIGH);
  Serial.println("off");
  delay(3000);

}
