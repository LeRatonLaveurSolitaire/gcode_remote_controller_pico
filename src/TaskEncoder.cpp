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
#define PIN_SETP_MINUS 22
#define PIN_SET_0 10
#define PIN_GOTO_0 11
#define PIN_EMERGENCY_STOP 12

void IRQ_encode();
void IRQ_switch();
void IRQ_F_plus();
void IRQ_F_minus();
void IRQ_step_plus();
void IRQ_step_minus();
void IRQ_set_0();
void IRQ_goto_0();
void IRQ_emergency_stop();

volatile int encoder_val = 0;
volatile int switch_val = 0;
volatile int F_plus = 0;
volatile int F_minus = 0;
volatile int step_plus_val = 0;
volatile int step_minus_val = 0;
volatile int set_0_val = 0;
volatile int goto_0_val = 0;

void TaskEncoder(void* pvParameters) {
  pinMode(PIN_ENC_READ, INPUT_PULLDOWN);
  pinMode(PIN_ENC_TRIG, INPUT_PULLDOWN);

  pinMode(PIN_F_PLUS, INPUT_PULLUP);
  pinMode(PIN_F_MINUS, INPUT_PULLUP);
  pinMode(PIN_SETP_PLUS, INPUT_PULLUP);
  pinMode(PIN_SETP_MINUS, INPUT_PULLUP);
  pinMode(PIN_SET_0, INPUT_PULLUP);
  pinMode(PIN_GOTO_0, INPUT_PULLUP);
  pinMode(PIN_EMERGENCY_STOP, INPUT_PULLUP);

  attachInterrupt(PIN_ENC_TRIG, IRQ_encode, RISING);
  attachInterrupt(PIN_ENC_SWTC, IRQ_switch, FALLING);

  attachInterrupt(PIN_F_PLUS,IRQ_F_plus , FALLING);
  attachInterrupt(PIN_F_MINUS,IRQ_F_minus , FALLING);
  attachInterrupt(PIN_SETP_PLUS,IRQ_step_plus , FALLING);
  attachInterrupt(PIN_SETP_MINUS,IRQ_step_minus , FALLING);
  attachInterrupt(PIN_SET_0,IRQ_set_0 , FALLING);
  attachInterrupt(PIN_GOTO_0, IRQ_goto_0, FALLING);
  attachInterrupt(PIN_EMERGENCY_STOP,IRQ_emergency_stop , FALLING);

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
                system_state_local.step_size * encoder_val
                );
        if(xQueueSendToFront(xCommmandQueue, command_to_send, portMAX_DELAY) == pdTRUE) {}//Serial.println("sent");

        switch (system_state_local.selected_axis) {
          case AXIS_X:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.x +=system_state_local.step_size * encoder_val;
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Y:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.y +=system_state_local.step_size * encoder_val;
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Z:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.z +=system_state_local.step_size * encoder_val;
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

    if (F_plus) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.f < MAX_SPEED){
        system_state.f += 10;
      }
      xSemaphoreGive(xStatusMutex);
      F_plus = 0;
    }

    if (F_minus) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.f > MIN_SPEED){
        system_state.f -= 10;
      }
      xSemaphoreGive(xStatusMutex);
      F_minus= 0;
    }
    if (step_plus_val) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.step_size < 0.9*MAX_STEP){
        system_state.step_size *= 10;
      }
      xSemaphoreGive(xStatusMutex);
      step_plus_val= 0;
    }

    if (step_minus_val) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.step_size > 1.1 * MIN_STEP){
        system_state.step_size /= 10;
      }
      xSemaphoreGive(xStatusMutex);
      step_minus_val= 0;
    }

    if (set_0_val) {
      sprintf(command_to_send, "G28.1");
      if(xQueueSendToFront(xCommmandQueue, command_to_send, portMAX_DELAY) == pdTRUE) {}
      set_0_val= 0;
    }

    if (goto_0_val) {
      sprintf(command_to_send, "G28");
      if(xQueueSendToFront(xCommmandQueue, command_to_send, portMAX_DELAY) == pdTRUE) {}
      goto_0_val= 0;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void IRQ_encode() {
  // static int last_called =0;
  // if (millis() - last_called > 10){

  // 1 if rotated clockwise, -1 if rotated anti-clockwise
  encoder_val = 1 - 2 * digitalRead(PIN_ENC_READ);

  // }
  // last_called = millis();
}
void IRQ_switch() {
  switch_val = 1;
}

void IRQ_F_plus(){
  F_plus = 1;
}

void IRQ_F_minus(){
  F_minus = 1;
}

void IRQ_step_plus(){
  step_plus_val = 1;
}

void IRQ_step_minus(){
  step_minus_val = 1;
}

void IRQ_set_0(){
  set_0_val = 1;
}

void IRQ_goto_0(){
  goto_0_val = 1;
}

void IRQ_emergency_stop(){

}
