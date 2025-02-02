#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include <semphr.h>  // Header file for semaphores

SoftwareSerial mySerial(10, 11);  // RX, TX

// Mutex handle
SemaphoreHandle_t xMutex;

unsigned long previousMillis = 0; // Lưu thời gian lần cuối chuyển đổi
const unsigned long interval = 4000; // Thời gian 4 giây
bool sendingS1 = true; // Biến để theo dõi trạng thái (s1 hoặc s0)

// Task gửi dữ liệu
void TaskSendData(void *pvParameters) {
  const TickType_t interval = 1000 / portTICK_PERIOD_MS; // 4 giây

  for (;;) {
    unsigned long currentMillis = millis(); // Lấy thời gian hiện tại

    // Kiểm tra nếu đã hết 4 giây, chuyển trạng thái
    if (currentMillis - previousMillis >= 2000) {
      previousMillis = currentMillis; // Cập nhật thời gian
      sendingS1 = !sendingS1;         // Chuyển trạng thái giữa s1 và s0
    }

    // Dùng mutex để bảo vệ Serial và mySerial
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      if (sendingS1) {
        mySerial.println("s1");
        Serial.println("Gửi: s1");
      } else {
        mySerial.println("s0");
        Serial.println("Gửi: s0");
      }
      xSemaphoreGive(xMutex); // Trả lại mutex
    }

    vTaskDelay(interval);   // Delay task 4 giây
  }
}

// Task nhận dữ liệu
void TaskReceiveData(void *pvParameters) {
  const TickType_t interval = 100 / portTICK_PERIOD_MS; // 100ms

  for (;;) {
    if (mySerial.available()) {
      String message = mySerial.readString();

      // Dùng mutex để bảo vệ Serial
      if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        Serial.print("Nhận từ Slave: ");
        Serial.println(message);
        xSemaphoreGive(xMutex); // Trả lại mutex
      }
    }

    vTaskDelay(interval); // Delay task 100ms
  }
}

void setup() {
  mySerial.begin(9600); // Khởi tạo cổng serial phần mềm
  Serial.begin(9600);   // Cổng serial mặc định cho debug

  // Tạo mutex
  xMutex = xSemaphoreCreateMutex();
  if (xMutex == NULL) {
    Serial.println("Không thể tạo mutex!");
    while (1); // Dừng chương trình nếu không tạo được mutex
  }

  // Tạo task gửi dữ liệu
  xTaskCreate(
    TaskSendData,     // Hàm task
    "Send Task",      // Tên task
    128,              // Kích thước stack
    NULL,             // Tham số
    1,                // Mức ưu tiên
    NULL              // Handle
  );

  // Tạo task nhận dữ liệu
  xTaskCreate(
    TaskReceiveData,  // Hàm task
    "Receive Task",   // Tên task
    128,              // Kích thước stack
    NULL,             // Tham số
    1,                // Mức ưu tiên
    NULL              // Handle
  );

  // Bắt đầu FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {
  // FreeRTOS không sử dụng loop()
  // Các task chạy độc lập
}
