#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

void TaskEncoder(void *pvParameters){
  
  
  for (;;) {// A Task shall never return or exit.
    digitalWrite(LED_BUILTIN,1);
    vTaskDelay(500);
    digitalWrite(LED_BUILTIN,0);
    vTaskDelay(500);
  }
}
