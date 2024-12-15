#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <status.h>
#include "semphr.h"

#define PIN_ENC_TRIG 18
#define PIN_ENC_READ 19
#define PIN_ENC_SWTC 20

void IRQ_encode();
void IRQ_switch();
int encoder_val = 0;
int switch_val = 0;

void TaskEncoder(void *pvParameters){
  
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(PIN_ENC_READ,INPUT);
  pinMode(PIN_ENC_TRIG,INPUT);
  attachInterrupt(PIN_ENC_TRIG,IRQ_encode, RISING);
  attachInterrupt(PIN_ENC_SWTC,IRQ_switch, FALLING);



  for (;;) {// A Task shall never return or exit.


    if(encoder_val){
      Serial.println(encoder_val);
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      bool is_selected = system_state.axis_is_selected;
      int axis = system_state.selected_axis;
      xSemaphoreGive(xStatusMutex);
      if(!is_selected){
        xSemaphoreTake(xStatusMutex, portMAX_DELAY);
        system_state.selected_axis = (system_state.selected_axis+1)%NUMBER_OF_AXIS;
        xSemaphoreGive(xStatusMutex);
      }
      else{
        switch(axis){
          case AXIS_X:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.x += encoder_val/abs(encoder_val);
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Y:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.y += encoder_val/abs(encoder_val);
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Z:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.z += encoder_val/abs(encoder_val);
            xSemaphoreGive(xStatusMutex);
            break;
          default:
            break;
        }
      }
      encoder_val = 0;
    }

    if(switch_val){
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      system_state.axis_is_selected = !system_state.axis_is_selected;
      xSemaphoreGive(xStatusMutex);
      switch_val = 0;
    }
    vTaskDelay(5);
  }
}

void IRQ_encode(){
  encoder_val = 0;
  //static int last_called =0;
  //if (millis() - last_called > 50){
    encoder_val += 1 - 2 * digitalRead(PIN_ENC_READ);
  //}
  // ast_called = millis();
}
void IRQ_switch(){
  switch_val=1;
}