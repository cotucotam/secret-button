#include <SoftwareSerial.h>
#include "PinDefinitions.h"
#include <Arduino_FreeRTOS.h>
#include "Button.h"
#include "RS485.h"
#include <semphr.h>
#include <avr/wdt.h>

SoftwareSerial mySerial(10, 11); // RX, TX

// Khởi tạo mảng buttonPins
int buttonPins[NUM_BUTTONS] = { BUTTON_PIN_1, BUTTON_PIN_2, BUTTON_PIN_3, BUTTON_PIN_4, BUTTON_PIN_5,
                                BUTTON_PIN_6, BUTTON_PIN_7, BUTTON_PIN_8, BUTTON_PIN_9, BUTTON_PIN_10,
                                BUTTON_PIN_11, BUTTON_PIN_12, BUTTON_PIN_13, BUTTON_PIN_14, BUTTON_PIN_15};

// int buttonPins[NUM_BUTTONS] = { BUTTON_PIN_1, BUTTON_PIN_2, BUTTON_PIN_3, BUTTON_PIN_4};

// Mảng các đối tượng Relay
Button* buttons[NUM_BUTTONS];

// Biến toàn cục Mutex để bảo vệ việc truy cập các relay
SemaphoreHandle_t xMutex;

// Mảng lưu trạng thái nút nhấn
bool buttonStates[NUM_BUTTONS] = { false };

// Mảng lưu trạng thái nút nhấn đã được nhấn
bool saveStates[NUM_BUTTONS] = { false };

// Task handle cho việc đọc trạng thái nút nhấn
TaskHandle_t buttonTaskHandle;

void setupButtons() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i] = new Button(buttonPins[i], true);  // Giả sử các button là activeLow
    buttons[i]->begin();
  }
}

bool pressedFlag = false;
int pressedCount = 0;

void readButtonStates(bool* buttonStates) {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttonStates[i] = buttons[i]->isPressed();
    
    if(buttons[i]->stateChanged()){
      pressedFlag = true;
    }
  }
}

// Hàm cập nhật saveStates dựa trên buttonStates
void updateRelayValues(bool* saveStates, bool* buttonStates, int numButtons) {
  for (int i = 0; i < numButtons; i++) {
    if (buttonStates[i] == 1) {
      saveStates[i] = 1;  // Cập nhật relayValues khi buttonValues = 1
    }
  }
}

void resetSaveStates(bool* saveStates, int size) {
  for (int i = 0; i < size; i++) {
    saveStates[i] = 0;
  }
}

void buttonTask(void* pvParameters) {
  while (1) {
    wdt_reset();
    if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000))) {
      // Đọc trạng thái các nút nhấn
      readButtonStates(buttonStates);
      updateRelayValues(saveStates, buttonStates, NUM_BUTTONS);
      // Trả lại mutex
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

String buildDataToSend(int slaveId, bool* saveStates, int numButtons) {
  String dataToSend = "S" + String(slaveId);  // Thêm S + SLAVE_ID vào chuỗi

  for (int i = 0; i < numButtons; i++) {
    // Tạo số thứ tự nút có định dạng 2 chữ số
    String buttonIndex = String(i + 1);
    if (buttonIndex.length() < 2) {
      buttonIndex = "0" + buttonIndex;  // Thêm số 0 nếu cần
    }

    // Thêm trạng thái nút vào chuỗi
    String buttonData = "B" + buttonIndex + String(saveStates[i]);
    dataToSend += buttonData;
  }

  return dataToSend;
}

// Task nhận dữ liệu
void TaskReceiveData(void *pvParameters) {
  const TickType_t interval = 300 / portTICK_PERIOD_MS; // 100ms
  wdt_reset();
  for (;;) {
    if (mySerial.available()) {
      String message = mySerial.readString();
      message.trim();
      // Dùng mutex để bảo vệ Serial
      if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000))) {

        Serial.println("Nhận từ master: "+message);
        if (message.startsWith("s")){
          if(pressedFlag){
            char identifier = message.substring(1).toInt(); // Lấy từ vị trí 1 đến hết chuỗi (bỏ 's')

            // Phản hồi lại Master dựa trên nội dung nhận được
            if (identifier == 1 && SLAVE_ID == 1) {
              vTaskDelay(interval); // Delay task 100ms
              String dataToSend = buildDataToSend(SLAVE_ID, saveStates, NUM_BUTTONS);
              mySerial.println(dataToSend);
              Serial.println(dataToSend);
            }
            if (identifier == 0 && SLAVE_ID == 0) {
              vTaskDelay(interval); // Delay task 100ms
              String dataToSend = buildDataToSend(SLAVE_ID, saveStates, NUM_BUTTONS);
              mySerial.println(dataToSend);
              Serial.println(dataToSend);
            }

            if(pressedCount > 2){
              pressedFlag = false;
              pressedCount = 0;
            } else{
              pressedCount ++;
            }
          }


        } else if (int(message.charAt(0)) == RESET_VALUE_RX){
          resetSaveStates(saveStates, NUM_BUTTONS);
          resetSaveStates(buttonStates, NUM_BUTTONS);
          Serial.println("reset");
        }

     
        xSemaphoreGive(xMutex); // Trả lại mutex
      }
      else{
        Serial.println("Không thể lấy mutex");
      }
    }else {
      
    }

    vTaskDelay(interval); // Delay task 100ms
  }
}

// S1-B01:1-B02:0

void rs485Task(void* pvParameters) {
  int count = 0;
  while (1) {
    if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000))) {

      // Gọi hàm buildDataToSend để tạo chuỗi dữ liệu

      String dataToSend_test = buildDataToSend(SLAVE_ID, buttonStates, NUM_BUTTONS);


      String dataToSend = buildDataToSend(SLAVE_ID, saveStates, NUM_BUTTONS);
  
      xSemaphoreGive(xMutex);

      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}

// const int button = A1;

void setup() {
  wdt_enable(WDTO_8S);

  // Tạo mutex
  xMutex = xSemaphoreCreateMutex();
  // Kiểm tra xem Mutex có được tạo thành công không
  if (xMutex == NULL) {
    Serial.println("Không thể tạo Mutex!");
    while (1)
      ;  // Nếu không tạo được Mutex thì dừng chương trình
  }

  Serial.begin(9600);  // Serial mặc định cho máy tính
  // rs485.begin();        // Khởi tạo RS485
  mySerial.begin(9600); // Khởi tạo cổng serial phần mềm
  setupButtons();       // Khởi tạo các button

  // Tạo task nhận dữ liệu
  xTaskCreate(
    TaskReceiveData,  // Hàm task
    "Receive Task",   // Tên task
    128,              // Kích thước stack
    NULL,             // Tham số
    1,                // Mức ưu tiên
    NULL              // Handle
  );
  // Tạo task Button
  xTaskCreate(
    buttonTask,
    "Button Task",
    128,
    NULL,
    1,
    NULL);

  // Bắt đầu các task
  vTaskStartScheduler();

  Serial.println("Master Ready");
}

void loop() {
}

