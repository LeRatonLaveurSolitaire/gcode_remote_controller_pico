#include <Arduino.h>
#include <FreeRTOS.h>
#include <status.h>
#include <task.h>
#include "semphr.h"

void TaskSerialManager(void* pvParameters) {
  Serial.begin(115200);
  static char msg[50] = {};
  char msg_received_local[50];
  bool is_available = false;

  for (;;) {  // A Task shall never return or exit.
    // xSemaphoreTake(xSerialMutex, portMAX_DELAY);
    // Serial.println('?');
    // xSemaphoreGive(xSerialMutex);
    // vTaskDelay(pdMS_TO_TICKS(10));

    if (Serial.available()){
      is_available = true;
      int i = 0;
      Serial.readBytesUntil('\n', msg_received_local, 50);
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      for(int i=0; i<50;i++){
        system_state.msg_received[i] = msg_received_local[i];
      }
      xSemaphoreGive(xStatusMutex);
      // TODO : parse answerd str
    }
    else{
      is_available = false;
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      system_state.communication_state = STATE_UNKNWON;
      xSemaphoreGive(xStatusMutex);
    }
    // TODO : send messages only if connected
    if (xQueueReceive(xCommmandQueue, msg, 10) == pdTRUE){
      strip.SetPixelColor(0, orange);
      strip.Show();
      xSemaphoreTake(xSerialMutex, portMAX_DELAY);
      Serial.println(msg);
      xSemaphoreGive(xSerialMutex);
      vTaskDelay(pdMS_TO_TICKS(10));
      // TODO : handle 'OK' response
    }
    else{
      strip.SetPixelColor(0, green);
      strip.Show();
    }

    vTaskDelay(pdMS_TO_TICKS(30));
  }
}