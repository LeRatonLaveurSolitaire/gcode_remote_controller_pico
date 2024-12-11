#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <status.h>


void TaskSerialManager(void *pvParameters){
  
  
  
  for (;;) {// A Task shall never return or exit.

    Serial.println("Hello world !");
    vTaskDelay(2000);
  }
}