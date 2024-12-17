#include <Arduino.h>
#include <FreeRTOS.h>
#include <status.h>
#include <task.h>
#include <stdio.h>
#include "semphr.h"

#define PIN_ENC_TRIG 18
#define PIN_ENC_READ 19
#define PIN_ENC_SWTC 20
#define PIN_F_PLUS 6
#define PIN_F_MINUS 7
#define PIN_SETP_PLUS 8
#define PIN_SETP_MINUS 9
#define PIN_SET_0 10
#define PIN_GOTO_0 11
#define PIN_EMERGENCY_STOP 12

void IRQ_encode();
void IRQ_switch();
volatile int encoder_val = 0;
volatile int switch_val = 0;


void TaskEncoder(void* pvParameters) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_ENC_READ, INPUT);
  pinMode(PIN_ENC_TRIG, INPUT);
  attachInterrupt(PIN_ENC_TRIG, IRQ_encode, RISING);
  attachInterrupt(PIN_ENC_SWTC, IRQ_switch, FALLING);

  global_status system_state_local;
  char command_to_send[50]={};
  for (;;) {  // A Task shall never return or exit.

    if (encoder_val) {
      // Serial.println(encoder_val);

      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      system_state_local.x = system_state.x;
      system_state_local.y = system_state.y;
      system_state_local.z = system_state.z;
      system_state_local.f = system_state.f;
      system_state_local.step_size = system_state.step_size;
      system_state_local.selected_axis = system_state.selected_axis;
      system_state_local.communication_state = system_state.communication_state;
      system_state_local.axis_is_selected = system_state.axis_is_selected;
      xSemaphoreGive(xStatusMutex);

      if (!system_state_local.axis_is_selected) {
        xSemaphoreTake(xStatusMutex, portMAX_DELAY);
        system_state.selected_axis =
            (system_state.selected_axis + 1) % NUMBER_OF_AXIS;
        xSemaphoreGive(xStatusMutex);
      }

      else {

        sprintf(command_to_send, "G1 F%.1f %c%.3f ",
                system_state_local.f,
                axis_chr[system_state_local.selected_axis],
                system_state_local.step_size * encoder_val / abs(encoder_val)
                );
        if(xQueueSendToFront(xCommmandQueue, command_to_send, portMAX_DELAY) == pdTRUE) {}//Serial.println("sent");

        switch (system_state_local.selected_axis) {
          case AXIS_X:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.x +=system_state_local.step_size * encoder_val / abs(encoder_val);
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Y:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.y +=system_state_local.step_size * encoder_val / abs(encoder_val);
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Z:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.z +=system_state_local.step_size * encoder_val / abs(encoder_val);
            xSemaphoreGive(xStatusMutex);
            break;
          default:
            break;  
        }
      }
      encoder_val = 0;
    }

    if (switch_val) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      system_state.axis_is_selected = !system_state.axis_is_selected;
      xSemaphoreGive(xStatusMutex);
      switch_val = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void IRQ_encode() {
  // static int last_called =0;
  // if (millis() - last_called > 50){

  // 1 if rotated clockwise, -1 if rotated anti-clockwise
  encoder_val = 1 - 2 * digitalRead(PIN_ENC_READ);

  //}
  // last_called = millis();
}
void IRQ_switch() {
  switch_val = 1;
}