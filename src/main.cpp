#include <Arduino.h>
#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <NeoPixelBus.h>
#include <Wire.h>
#include <stdint.h>
#include <task.h>

#include <status.h>
#include "TaskEncoder.h"
#include "TaskScreen.h"
#include "TaskSerialManager.h"
#include "semphr.h"

SemaphoreHandle_t xStatusMutex = NULL;
SemaphoreHandle_t xSerialMutex = NULL;
QueueHandle_t xCommmandQueue = NULL;
global_status system_state;

const uint16_t PixelCount = 1;
const uint8_t PixelPin = 16;

RgbColor red(0, colorSaturation, 0);
RgbColor green(colorSaturation, 0, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

char axis_chr[] = {'X', 'Y', 'Z'};

// the setup function runs once when you press reset or power the board
void setup() {
  strip.Begin();
  strip.SetPixelColor(0, red);
  strip.Show();

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
