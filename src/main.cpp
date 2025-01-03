#include <Arduino.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <Wire.h>
#include <status.h>
#include <stdint.h>
#include <task.h>

#include "TaskEncoder.h"
#include "TaskScreen.h"
#include "TaskSerialManager.h"
#include "semphr.h"

SemaphoreHandle_t xStatusMutex = NULL;
SemaphoreHandle_t xSerialMutex = NULL;
QueueHandle_t xCommmandQueue = NULL;
global_status system_state;

char axis_chr[] = {'X', 'Y', 'Z'};

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(16, OUTPUT);
  digitalWrite(16,HIGH);
  xStatusMutex = xSemaphoreCreateMutex();
  xSerialMutex = xSemaphoreCreateMutex();
  xCommmandQueue = xQueueCreate(16, 50 * sizeof(char));

  xTaskCreate(

      TaskEncoder, "Encoder",
      1024,  // Stack size
      NULL,
      2,  // Priority
      NULL

  );

  xTaskCreate(

      TaskSerialManager, "SerialManager",
      1024,  // Stack size
      NULL,
      2,  // Priority
      NULL

  );

  xTaskCreate(

      TaskScreen,
      "Screen",  // A name just for humans
      4096,  // This stack size can be checked & adjusted by reading the Stack
             // Highwater
      NULL,
      1,  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and
          // 0 being the lowest.
      NULL

  );
}

// void setup1(){

// }

void loop() {
  // Empty. Things are done in Tasks.
}

// void loop1()
// {
//   // Empty. Things are done in Tasks.
// }
