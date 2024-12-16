#include <Arduino.h>
#include <FreeRTOS.h>
#include <status.h>
#include <task.h>
#include "semphr.h"

void TaskSerialManager(void* pvParameters) {
    Serial.begin(115200);
  char msg[50];

  for (;;) {  // A Task shall never return or exit.
    if (uxQueueMessagesWaiting(xCommmandQueue)) {
      xQueueReceive(xCommmandQueue, msg, 100);
      Serial.println(msg);
    }
    vTaskDelay(100);
  }
}