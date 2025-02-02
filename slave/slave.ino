#include "RS485.h"
#include "Relay.h"
#include "PinDefinitions.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "Button.h"
#include <avr/wdt.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // RX, TX
// Khởi tạo mảng relayPins
int relayPins[NUM_RELAYS] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5,
                             RELAY_PIN_6, RELAY_PIN_7, RELAY_PIN_8, RELAY_PIN_9, RELAY_PIN_10,
                             RELAY_PIN_11, RELAY_PIN_12, RELAY_PIN_13, RELAY_PIN_14, RELAY_PIN_15,
                             RELAY_PIN_16, RELAY_PIN_17, RELAY_PIN_18, RELAY_PIN_19};

// Mảng các đối tượng Relay
Relay* relays[NUM_RELAYS];

// reset button
Button resetButton(RESET_BUTTON_PIN, true); // Nút trên chân 42, activeLow

// Biến toàn cục Mutex để bảo vệ việc truy cập các relay
SemaphoreHandle_t xMutex;


int buttonValues[NUM_RELAYS]; // Mảng để lưu giá trị các N (0 hoặc 1)

// Mảng lưu giá trị trạng thái của relay (ban đầu mặc định = 0)
int relayValues[NUM_RELAYS] = {0};

// Reset flag
bool resetFlag = true;

// Reset count 
int resetCount = 0;


unsigned long previousMillis = 0; // Lưu thời gian lần cuối chuyển đổi
const unsigned long interval = 4000; // Thời gian 4 giây
bool sendingS1 = true; // Biến để theo dõi trạng thái (s1 hoặc s0)

// Hàm setupRelays để quét mảng relayPins và khởi tạo các đối tượng Relay
void setupRelays() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        relays[i] = new Relay(relayPins[i], true); // Sử dụng `new` để khởi tạo đối tượng
        relays[i]->begin(); // Khởi tạo mỗi relay
        relays[i]->off();
    }
}

// Hàm điều khiển một relay
void RelayControlTask(void* pvParameters) {
    int relayIndex = (int)pvParameters;  // Lấy chỉ số relay từ tham số truyền vào
    
    while (1) {
        wdt_reset();

        // Dùng Mutex để bảo vệ việc truy cập vào relay
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            // Bật relay
            if(relayValues[relayIndex] == 1){

              relays[relayIndex]->on();
              // delay(100); // Bật relay 1 giây
            }
            else if(relayValues[relayIndex] == 0){
              // Tắt relay
              relays[relayIndex]->off();
            }
            else{
              // Tắt relay
              relays[relayIndex]->off();
            }

            // Giải phóng Mutex sau khi hoàn tất việc điều khiển relay
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Đợi 100ms
    }
}

// Hàm cập nhật relayValues dựa trên buttonValues và vị trí startRelay, endRelay
void updateRelayValues(int* buttonValues, int* relayValues, int startRelay, int endRelay) {
    for (int i = startRelay; i <= endRelay; i++) {
        int buttonIndex = i - startRelay; // Tính chỉ số tương ứng trong buttonValues
        if (buttonValues[buttonIndex] == 1) {
            relayValues[i] = 1; // Cập nhật relayValues khi buttonValues = 1
        }
    }
}

// Hàm xử lý nhận dữ liệu từ RS485
void RS485Task(void* pvParameters) {
    const TickType_t interval = 100 / portTICK_PERIOD_MS; // 100ms
    while (1) {
      wdt_reset();
      if (mySerial.available()) {
        String message = mySerial.readString();

        // Dùng mutex để bảo vệ Serial
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000))) {
          // Serial.print("Nhận từ Slave: ");
          Serial.println(message);

          if(!resetFlag){
              // Serial.println(String(message).length());
            if(String(message).length() > 5){
              String receivedData = message;

              int startRelay = 0;
              int endRelay = 0;
              int slaveID = 0;
              RS485::parseData(receivedData, slaveID, buttonValues);

            // Gọi hàm cập nhật relayValues sau khi parse xong
              if(slaveID == 0){
                startRelay = SLAVE_1_START_RELAY;
                endRelay = SLAVE_1_END_RELAY;
              }
              else if(slaveID == 1){
                startRelay = SLAVE_2_START_RELAY;
                endRelay = SLAVE_2_END_RELAY;
              }

              // Gọi hàm cập nhật relayValues sau khi parse xong
              updateRelayValues(buttonValues, relayValues, startRelay, endRelay);
              // In giá trị các nút
              // for (int i = 0; i < NUM_BUTTONS; i++) {
              //     Serial.print("relayValues ");
              //     Serial.print(i + 1);
              //     Serial.print(": ");
              //     Serial.println(relayValues[i]);
              // }
            }
         
          }
          xSemaphoreGive(xMutex); // Trả lại mutex
        }
      }else {
        
      }

      vTaskDelay(interval); // Delay task 100ms
    
    }
}

void resetRelayValues(int* relayValues, int size) {
    for (int i = 0; i < size; i++) {
        relayValues[i] = 0;
    }
}

void resetButtonTask(void* pvParameters) {
    while (1) {
        wdt_reset();
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000))) {
            // Đọc trạng thái các nút nhấn
            if(resetButton.stateChanged()){
                // Reset tất cả giá trị về 0
                Serial.println("reset button is pressed");
                resetFlag = true;
                resetRelayValues(relayValues, NUM_RELAYS);
            }

            // Trả lại mutex
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// Task gửi dữ liệu
void TaskSendData(void *pvParameters) {
  const TickType_t interval = 1000 / portTICK_PERIOD_MS; // 4 giây
  
  for (;;) {
    unsigned long currentMillis = millis(); // Lấy thời gian hiện tại
    wdt_reset();
    // Kiểm tra nếu đã hết 4 giây, chuyển trạng thái
    if (currentMillis - previousMillis < 2000) {
      // Dùng mutex để bảo vệ Serial và mySerial
      if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(1000))) {
        
        if(!resetFlag){
          if (sendingS1) {
            mySerial.println("s1");
            Serial.println("Gửi: s1");
          } else {
            mySerial.println("s0");
            Serial.println("Gửi: s0");
          }
        } else{
            mySerial.println(RESET_VALUE_TX);
            Serial.println("Gửi: reset");
            // if(resetCount > 1){
              resetFlag = false;
              // resetCount = 0;
            // }
            // resetCount++;
            
        }

        xSemaphoreGive(xMutex); // Trả lại mutex
      }
    } else if (currentMillis - previousMillis >= 2500) {
      previousMillis = currentMillis; // Cập nhật thời gian
      sendingS1 = !sendingS1;         // Chuyển trạng thái giữa s1 và s0

    }

    vTaskDelay(interval);   // Delay task 4 giây
  }
}

void setup() {
    wdt_enable(WDTO_8S);
    Serial.begin(9600); // Serial mặc định cho máy tính
  
    mySerial.begin(9600); // Khởi tạo cổng serial phần mềm

    // Khởi tạo relay
    Serial.println("Relay initialized");
    setupRelays();

    Serial.println("Slave Ready");

    resetButton.begin();
    // Khởi tạo Mutex
    xMutex = xSemaphoreCreateMutex();

    // Kiểm tra xem Mutex có được tạo thành công không
    if (xMutex == NULL) {
        Serial.println("Không thể tạo Mutex!");
        while (1);  // Nếu không tạo được Mutex thì dừng chương trình
    }

    // Tạo task cho từng relay
    for (int i = 0; i < NUM_RELAYS; i++) {
        xTaskCreate(
            RelayControlTask,        // Hàm task
            "RelayControlTask",      // Tên task
            128,                     // Kích thước stack (có thể điều chỉnh)
            (void*)i,                // Tham số truyền vào (chỉ số relay)
            1,                       // Độ ưu tiên của task
            NULL                     // Handle của task (không cần ở đây)
        );
    }

    // Tạo task RS485
    xTaskCreate(
        RS485Task,               // Hàm task
        "RS485Task",             // Tên task
        128,                     // Kích thước stack (có thể điều chỉnh)
        NULL,                    // Tham số truyền vào task
        1,                       // Độ ưu tiên của task
        NULL                     // Handle của task (không cần ở đây)
    );
  
    // Tạo task resetButton
    xTaskCreate(
        resetButtonTask,               // Hàm task
        "resetButtonTask",             // Tên task
        128,                     // Kích thước stack (có thể điều chỉnh)
        NULL,                    // Tham số truyền vào task
        1,                       // Độ ưu tiên của task
        NULL                     // Handle của task (không cần ở đây)
    );
    // Bắt đầu các task

  // Tạo task gửi dữ liệu
  xTaskCreate(
    TaskSendData,     // Hàm task
    "Send Task",      // Tên task
    128,              // Kích thước stack
    NULL,             // Tham số
    1,                // Mức ưu tiên
    NULL              // Handle
  );

    vTaskStartScheduler();

}

void loop() {

}

