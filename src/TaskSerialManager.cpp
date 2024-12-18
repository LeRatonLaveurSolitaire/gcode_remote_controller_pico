#include <Arduino.h>
#include <FreeRTOS.h>
#include <status.h>
#include <task.h>
#include "semphr.h"

void TaskSerialManager(void* pvParameters) {
  Serial.begin(115200);
  static char msg[50] = {};

  for (;;) {  // A Task shall never return or exit.
    // Serial.println(uxQueueMessagesWaiting(xCommmandQueue));
    if (xQueueReceive(xCommmandQueue, msg, 10) == pdTRUE){
      Serial.println(msg);
      //xQueueReceive(xCommmandQueue, msg, portMAX_DELAY);
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}