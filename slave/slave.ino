#include "RS485.h"
#include "Relay.h"
#include "PinDefinitions.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "Button.h"
// Tạo đối tượng RS485
RS485 rs485(RX_PIN, TX_PIN, 9600);

// Khởi tạo mảng relayPins
int relayPins[NUM_RELAYS] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5,
                             RELAY_PIN_6, RELAY_PIN_7, RELAY_PIN_8, RELAY_PIN_9, RELAY_PIN_10,
                             RELAY_PIN_11, RELAY_PIN_12, RELAY_PIN_13, RELAY_PIN_14, RELAY_PIN_15,
                             RELAY_PIN_16, RELAY_PIN_17, RELAY_PIN_18, RELAY_PIN_19, RELAY_PIN_20};

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
bool restFlag = true;
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

        // relay test
        if(relayIndex == 0){
          Serial.println("relay test"+String(relayValues[relayIndex]));
          if(relayValues[relayIndex] == 1){
            Serial.println("relay test on");
            }
          else if(relayValues[relayIndex] == 0){
              // Tắt relay
              Serial.println("relay test off");
            }
        }
        //Serial.println("reset button is pressed");

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

// Hàm cập nhật relayValues dựa trên buttonValues
void updateRelayValues(int* buttonValues, int* relayValues, int numRelays) {
    for (int i = 0; i < numRelays; i++) {
        if (buttonValues[i] == 1) {
            relayValues[i] = 1; // Cập nhật relayValues khi buttonValues = 1
        }
    }
}

// Hàm xử lý nhận dữ liệu từ RS485
void RS485Task(void* pvParameters) {
    while (1) {

        String receivedData = rs485.receive();  // Nhận dữ liệu từ RS485
        // Serial.println("receivedData: "+ receivedData);
        
        // Kiểm tra dữ liệu nhận được và điều khiển relay
        if (receivedData.length() > 0) {
            xSemaphoreTake(xMutex, portMAX_DELAY);  // Lấy Mutex trước khi điều khiển relay

            //Serial.println("receivedData: "+ receivedData);
            
            RS485::parseData(receivedData, buttonValues);

            // Gọi hàm cập nhật relayValues sau khi parse xong
            updateRelayValues(buttonValues, relayValues, NUM_RELAYS);

            xSemaphoreGive(xMutex);  // Giải phóng Mutex sau khi điều khiển relay
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Đợi 100ms trước khi đọc lại
    }
}

void resetRelayValues(int* relayValues, int size) {
    for (int i = 0; i < size; i++) {
        relayValues[i] = 0;
    }
}

void resetButtonTask(void* pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            // Đọc trạng thái các nút nhấn
            if(resetButton.isPressed()){
                // Reset tất cả giá trị về 0
                Serial.println("reset button is pressed");
                restFlag = true;
                rs485.send(String(RESET_VALUE));
                vTaskDelay(1000 / portTICK_PERIOD_MS);  // Đợi 100ms trước khi đọc lại
                rs485.send(String(RESET_VALUE));
                resetRelayValues(relayValues, NUM_RELAYS);

                // vTaskDelay(100 / portTICK_PERIOD_MS);  // Đợi 100ms trước khi đọc lại
                // resetRelayValues(relayValues, NUM_RELAYS);
            }

            // Trả lại mutex
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(19200); // Serial mặc định cho máy tính
    rs485.begin();       // Khởi tạo RS485

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
        256,                     // Kích thước stack (có thể điều chỉnh)
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
    vTaskStartScheduler();

}

void loop() {

}

