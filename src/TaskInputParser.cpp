#include <Arduino.h>
#include <FreeRTOS.h>
#include <status.h>
#include <stdio.h>
#include <task.h>

#include "semphr.h"

#define DEBOUNCE_MS 100
#define PIN_ENC_SWTC 6
#define PIN_F_PLUS 27
#define PIN_F_MINUS 26
#define PIN_SETP_PLUS 9
#define PIN_SETP_MINUS 10
#define PIN_SET_0 13
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

volatile int switch_val = 0;
volatile int F_plus = 0;
volatile int F_minus = 0;
volatile int step_plus_val = 0;
volatile int step_minus_val = 0;
volatile int set_0_val = 0;
volatile int goto_0_val = 0;

void TaskInputParser(void* pvParameters) {
  pinMode(PIN_ENC_SWTC, INPUT_PULLUP);
  pinMode(PIN_F_PLUS, INPUT_PULLUP);
  pinMode(PIN_F_MINUS, INPUT_PULLUP);
  pinMode(PIN_SETP_PLUS, INPUT_PULLUP);
  pinMode(PIN_SETP_MINUS, INPUT_PULLUP);
  pinMode(PIN_SET_0, INPUT_PULLUP);
  pinMode(PIN_GOTO_0, INPUT_PULLUP);
  pinMode(PIN_EMERGENCY_STOP, INPUT_PULLUP);

  attachInterrupt(PIN_ENC_SWTC, IRQ_switch, FALLING);

  attachInterrupt(PIN_F_PLUS, IRQ_F_plus, FALLING);
  attachInterrupt(PIN_F_MINUS, IRQ_F_minus, FALLING);
  attachInterrupt(PIN_SETP_PLUS, IRQ_step_plus, FALLING);
  attachInterrupt(PIN_SETP_MINUS, IRQ_step_minus, FALLING);
  attachInterrupt(PIN_SET_0, IRQ_set_0, FALLING);
  attachInterrupt(PIN_GOTO_0, IRQ_goto_0, FALLING);
  attachInterrupt(PIN_EMERGENCY_STOP, IRQ_emergency_stop, FALLING);

  global_status system_state_local;
  char command_to_send[50] = {};
  int rotated = 0;
  for (;;) {  // A Task shall never return or exit.

    rotated = encoder_val >> 2;
    if (rotated) {
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
        // system_state.selected_axis =
        //     (system_state.selected_axis + rotated) % NUMBER_OF_AXIS;
        if (system_state.selected_axis < 0) {
          system_state.selected_axis = NUMBER_OF_AXIS - 1;
        }
        xSemaphoreGive(xStatusMutex);
      }

      else {
        sprintf(command_to_send, "G91 G1 F%.1f %c%.2f ", system_state_local.f,
                axis_chr[system_state_local.selected_axis],
                system_state_local.step_size * rotated);
        if (xQueueSendToFront(xCommmandQueue, command_to_send, portMAX_DELAY) ==
            pdTRUE) {
        }

        switch (system_state_local.selected_axis) {
          case AXIS_X:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.x += system_state_local.step_size * rotated;
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Y:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.y += system_state_local.step_size * rotated;
            xSemaphoreGive(xStatusMutex);
            break;
          case AXIS_Z:
            xSemaphoreTake(xStatusMutex, portMAX_DELAY);
            system_state.z += system_state_local.step_size * rotated;
            xSemaphoreGive(xStatusMutex);
            break;
          default:
            break;
        }
      }

      encoder_val -= rotated << 2;
    }

    if (switch_val) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      system_state.axis_is_selected = !system_state.axis_is_selected;
      xSemaphoreGive(xStatusMutex);
      switch_val = 0;
    }

    if (F_plus) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.f < MAX_SPEED) {
        system_state.f += 10;
      }
      xSemaphoreGive(xStatusMutex);
      F_plus = 0;
    }

    if (F_minus) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.f > MIN_SPEED) {
        system_state.f -= 10;
      }
      xSemaphoreGive(xStatusMutex);
      F_minus = 0;
    }
    if (step_plus_val) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.step_size < 0.9 * MAX_STEP) {
        system_state.step_size *= 10;
      }
      xSemaphoreGive(xStatusMutex);
      step_plus_val = 0;
    }

    if (step_minus_val) {
      xSemaphoreTake(xStatusMutex, portMAX_DELAY);
      if (system_state.step_size > 1.1 * MIN_STEP) {
        system_state.step_size /= 10;
      }
      xSemaphoreGive(xStatusMutex);
      step_minus_val = 0;
    }

    if (set_0_val) {
      const char stop = 0x18;
      sprintf(command_to_send, &stop);
      if (xQueueSendToFront(xCommmandQueue, command_to_send, portMAX_DELAY) ==
          pdTRUE) {
      }
      set_0_val = 0;
    }

    if (goto_0_val) {
      sprintf(command_to_send, "G28");
      if (xQueueSendToFront(xCommmandQueue, command_to_send, portMAX_DELAY) ==
          pdTRUE) {
      }
      goto_0_val = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void IRQ_switch() {
  static unsigned long last_call = 0;
  if (last_call - millis() > DEBOUNCE_MS) {
    switch_val = 1;
    last_call = millis();
  }
}

void IRQ_F_plus() {
  static unsigned long last_call = 0;
  if (last_call - millis() > DEBOUNCE_MS) {
    F_plus = 1;
    last_call = millis();
  }
}

void IRQ_F_minus() {
  static unsigned long last_call = 0;
  if (last_call - millis() > DEBOUNCE_MS) {
    F_minus = 1;
    last_call = millis();
  }
}

void IRQ_step_plus() {
  static unsigned long last_call = 0;
  if (last_call - millis() > DEBOUNCE_MS) {
    step_plus_val = 1;
    last_call = millis();
  }
}

void IRQ_step_minus() {
  static unsigned long last_call = 0;
  if (last_call - millis() > DEBOUNCE_MS) {
    step_minus_val = 1;
    last_call = millis();
  }
}

void IRQ_set_0() {
  static unsigned long last_call = 0;
  if (last_call - millis() > DEBOUNCE_MS) {
    set_0_val = 1;
    last_call = millis();
  }
}

void IRQ_goto_0() {
  static unsigned long last_call = 0;
  if (last_call - millis() > DEBOUNCE_MS) {
    goto_0_val = 1;
    last_call = millis();
  }
}

void IRQ_emergency_stop() {
  xSemaphoreTake(xSerialMutex, portMAX_DELAY);
  Serial1.println(0x85);
  xQueueReset(xCommmandQueue);
  xSemaphoreGive(xSerialMutex);
}