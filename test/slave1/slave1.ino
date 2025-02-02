#include <SoftwareSerial.h>

// Định nghĩa SoftwareSerial cho Slave
SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  mySerial.begin(9600); // Khởi tạo cổng serial phần mềm
  Serial.begin(9600);   // Cổng serial mặc định cho debug
}

void loop() {
  if (mySerial.available()) {
    String message = mySerial.readString(); // Đọc dữ liệu từ Master
    message.trim(); // Loại bỏ khoảng trắng và ký tự xuống dòng
    Serial.print("Nhận từ Master: ");
    Serial.println(message);

    // Phản hồi lại Master dựa trên nội dung nhận được
    if (message == "s1") {
      mySerial.println("Đây là Slave 1");
      Serial.println("Gửi: Đây là Slave 1");
    }
    
  }
  
}
