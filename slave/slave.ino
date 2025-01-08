#include "RS485.h"
#include "Relay.h"
#include "PinDefinitions.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// Tạo đối tượng RS485
RS485 rs485(RX_PIN, TX_PIN, 9600);

// Khởi tạo mảng relayPins
int relayPins[NUM_RELAYS] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5,
                             RELAY_PIN_6, RELAY_PIN_7, RELAY_PIN_8, RELAY_PIN_9, RELAY_PIN_10,
                             RELAY_PIN_11, RELAY_PIN_12, RELAY_PIN_13, RELAY_PIN_14, RELAY_PIN_15,
                             RELAY_PIN_16, RELAY_PIN_17, RELAY_PIN_18, RELAY_PIN_19, RELAY_PIN_20};

// Mảng các đối tượng Relay
Relay* relays[NUM_RELAYS];

// Biến toàn cục Mutex để bảo vệ việc truy cập các relay
SemaphoreHandle_t xMutex;


int buttonValues[NUM_RELAYS]; // Mảng để lưu giá trị các N (0 hoặc 1)

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
        // Dùng Mutex để bảo vệ việc truy cập vào relay
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            // Bật relay
            if(buttonValues[relayIndex] == "1"){
              relays[relayIndex]->on();
              delay(1000); // Bật relay 1 giây
            }
            else if(buttonValues[relayIndex] == "0"){
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

            Serial.println("receivedData: "+ receivedData);
            
            // RS485::parseData(receivedData, buttonValues);

            // if (receivedData == "1") {
            //     relays[0]->on(); // Bật relay 0
            //     //Serial.println("Relay ON");
            // } else if (receivedData == "0") {
            //     relays[0]->off(); // Tắt relay 0
            //     //Serial.println("Relay OFF");
            // }

            xSemaphoreGive(xMutex);  // Giải phóng Mutex sau khi điều khiển relay
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // Đợi 100ms trước khi đọc lại
    }
}

void setup() {
    Serial.begin(19200); // Serial mặc định cho máy tính
    rs485.begin();       // Khởi tạo RS485

    // Khởi tạo relay
    Serial.println("Relay initialized");
    setupRelays();

    Serial.println("Slave Ready");

    // Khởi tạo Mutex
    xMutex = xSemaphoreCreateMutex();

    // Kiểm tra xem Mutex có được tạo thành công không
    if (xMutex == NULL) {
        Serial.println("Không thể tạo Mutex!");
        while (1);  // Nếu không tạo được Mutex thì dừng chương trình
    }

        // Tạo task cho từng relay
    // for (int i = 0; i < NUM_RELAYS; i++) {
    //     xTaskCreate(
    //         RelayControlTask,        // Hàm task
    //         "RelayControlTask",      // Tên task
    //         128,                     // Kích thước stack (có thể điều chỉnh)
    //         (void*)i,                // Tham số truyền vào (chỉ số relay)
    //         1,                       // Độ ưu tiên của task
    //         NULL                     // Handle của task (không cần ở đây)
    //     );
    // }

    // Tạo task RS485
    xTaskCreate(
        RS485Task,               // Hàm task
        "RS485Task",             // Tên task
        256,                     // Kích thước stack (có thể điều chỉnh)
        NULL,                    // Tham số truyền vào task
        1,                       // Độ ưu tiên của task
        NULL                     // Handle của task (không cần ở đây)
    );
    // Bắt đầu các task
    vTaskStartScheduler();

}

void loop() {

}

