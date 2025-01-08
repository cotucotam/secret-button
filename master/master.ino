#include <SoftwareSerial.h>
#include "PinDefinitions.h"
#include <Arduino_FreeRTOS.h>
#include "Button.h"
#include "RS485.h"
#include <semphr.h>

// Tạo đối tượng RS485
RS485 rs485(RX_PIN, TX_PIN, 9600);

// Khởi tạo mảng buttonPins
int buttonPins[NUM_BUTTONS] = {BUTTON_PIN_1, BUTTON_PIN_2, BUTTON_PIN_3, BUTTON_PIN_4, BUTTON_PIN_5,
                             BUTTON_PIN_6, BUTTON_PIN_7, BUTTON_PIN_8, BUTTON_PIN_9, BUTTON_PIN_10,
                             BUTTON_PIN_11, BUTTON_PIN_12, BUTTON_PIN_13, BUTTON_PIN_14, BUTTON_PIN_15,
                             BUTTON_PIN_16, BUTTON_PIN_17, BUTTON_PIN_18, BUTTON_PIN_19, BUTTON_PIN_20};

// Mảng các đối tượng Relay
Button* buttons[NUM_BUTTONS];

// Biến toàn cục Mutex để bảo vệ việc truy cập các relay
SemaphoreHandle_t xMutex;

// Mảng lưu trạng thái nút nhấn
bool buttonStates[NUM_BUTTONS] = {false};

// Task handle cho việc đọc trạng thái nút nhấn
TaskHandle_t buttonTaskHandle;

void setupButtons() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = new Button(buttonPins[i], true); // Giả sử các button là activeLow
        buttons[i]->begin();
    }
}

void readButtonStates(bool* buttonStates) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttonStates[i] = buttons[i]->isPressed();
    }
}

void buttonTask(void* pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            // Đọc trạng thái các nút nhấn
            readButtonStates(buttonStates);

            // Trả lại mutex
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
// S1-B01:1-B02:0

void rs485Task(void* pvParameters) {
  int count = 0;
    while (1) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {

          // Tạo chuỗi dataToSend
          String dataToSend = "S" + String(SLAVE_ID); // Thêm S + SLAVE_ID vào chuỗi

          for (int i = 0; i < NUM_BUTTONS; i++) {
              // Tạo số thứ tự nút có định dạng 2 chữ số
              String buttonIndex = String(i + 1);
              if (buttonIndex.length() < 2) {
                  buttonIndex = "0" + buttonIndex; // Thêm số 0 nếu cần
              }

              // Thêm trạng thái nút vào chuỗi
              String buttonData = "-B" + buttonIndex + ":" + String(buttonStates[i]);
              dataToSend += buttonData;


            }
              // Gửi dữ liệu qua Serial hoặc RS485
              // Serial.println("Data to send: " + dataToSend);
              //dataToSend = "1";
               Serial.println("Data to send: " + dataToSend);
              rs485.send(dataToSend);
              count ++;
               Serial.println(count);

              // Trả lại mutex
              xSemaphoreGive(xMutex);
          
          vTaskDelay(1000 / portTICK_PERIOD_MS); 
        }
      }
}

// const int button = A1;

void setup() {

  // Tạo mutex
  xMutex = xSemaphoreCreateMutex();
  // Kiểm tra xem Mutex có được tạo thành công không
  if (xMutex == NULL) {
      Serial.println("Không thể tạo Mutex!");
      while (1);  // Nếu không tạo được Mutex thì dừng chương trình
  }

  Serial.begin(19200);       // Serial mặc định cho máy tính
  rs485.begin();             // Khởi tạo RS485
  setupButtons();            // Khởi tạo các button

  // pinMode(button, INPUT);  //Cài đặt chân D11 ở trạng thái đọc dữ liệu

  // Tạo task RS485
  xTaskCreate(
      rs485Task,               // Hàm task
      "rs485Task",             // Tên task
      256,                     // Kích thước stack (có thể điều chỉnh)
      NULL,                    // Tham số truyền vào task
      1,                       // Độ ưu tiên của task
      NULL                     // Handle của task (không cần ở đây)
  );

  // Tạo task Button
  xTaskCreate(
      buttonTask,
      "Button Task",
      128,
      NULL,
      1,
      NULL
  );

  // Bắt đầu các task
  vTaskStartScheduler();

  Serial.println("Master Ready");
}

void loop() {

}
