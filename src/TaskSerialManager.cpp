#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <status.h>
#include "semphr.h"


void TaskSerialManager(void *pvParameters){
  Serial.begin(115200);
  char msg[20];
  float x;
  for (;;) {// A Task shall never return or exit.
    xSemaphoreTake(xStatusMutex, portMAX_DELAY);
    x = system_state.x;
    xSemaphoreGive(xStatusMutex);
    sprintf(msg, " %03.2f , %d", x, millis());
    //Serial.println(msg);
    vTaskDelay(2000);
  }
}