#include <Arduino.h>
#include <FreeRTOS.h>
#include <status.h>
#include <string.h>
#include <task.h>

#include "semphr.h"

int get_state_from_string(const char *);
void parse_status_message(const char *, global_status *);

void TaskSerialManager(void *pvParameters) {
  Serial1.begin(115200);
  static char msg[50] = {};
  char msg_received_local[50];
  bool is_available = false;

  for (;;) {  // A Task shall never return or exit.
    // xSemaphoreTake(xSerialMutex, portMAX_DELAY);
    // Serial.println('?');
    // xSemaphoreGive(xSerialMutex);
    // vTaskDelay(pdMS_TO_TICKS(10));

    // if (Serial1.available()) {
    //   is_available = true;
    //   int i = 0;
    //   Serial1.readBytesUntil('\n', msg_received_local, 50);
    //   xSemaphoreTake(xStatusMutex, portMAX_DELAY);
    //   for (int i = 0; i < 50; i++) {
    //     system_state.msg_received[i] = msg_received_local[i];
    //   }
    //   xSemaphoreGive(xStatusMutex);
    //   // TODO : parse answerd str
    // } else {
    //   is_available = false;
    //   xSemaphoreTake(xStatusMutex, portMAX_DELAY);
    //   system_state.communication_state = STATE_UNKNWON;
    //   xSemaphoreGive(xStatusMutex);
    // }

    if (xQueueReceive(xCommmandQueue, msg, 10) == pdTRUE) {
      strip.SetPixelColor(0, orange);
      strip.Show();
      xSemaphoreTake(xSerialMutex, portMAX_DELAY);
      Serial1.println(msg);
      xSemaphoreGive(xSerialMutex);
      vTaskDelay(pdMS_TO_TICKS(10));
      // TODO : handle 'OK' response
      if (msg[0] == '?') {
        int i = 0;
        while (Serial1.available() && i < 50) {
          msg_received_local[i] = Serial1.read();
          i++;
        }
        if (i == 0) {
          xSemaphoreTake(xStatusMutex, portMAX_DELAY);
          system_state.communication_state = STATE_UNKNWON;
          xSemaphoreGive(xStatusMutex);
        } else {
          xSemaphoreTake(xStatusMutex, portMAX_DELAY);
          parse_status_message(msg_received_local, &system_state);
          xSemaphoreGive(xStatusMutex);
        }
      }

    }

    else {
      strip.SetPixelColor(0, green);
      strip.Show();
    }

    vTaskDelay(pdMS_TO_TICKS(30));
  }
}

int get_state_from_string(const char *state_str) {
  if (strcmp(state_str, "Idle") == 0) return STATE_IDLE;
  if (strcmp(state_str, "Run") == 0) return STATE_RUN;
  if (strcmp(state_str, "Jog") == 0) return STATE_JOG;
  if (strcmp(state_str, "Hold") == 0) return STATE_HOLD;
  if (strcmp(state_str, "Alarm") == 0) return STATE_ALARM;
  if (strcmp(state_str, "Check") == 0) return STATE_CHECK;
  if (strcmp(state_str, "Work") == 0) return STATE_WORK;
  return STATE_UNKNWON;
}

// Main parser function
void parse_status_message(const char *msg, global_status *status) {
  if (!msg || !status) return;

  // Copy raw message into struct for reference
  strncpy(status->msg_received, msg, sizeof(status->msg_received) - 1);
  status->msg_received[sizeof(status->msg_received) - 1] = '\0';

  // Extract the state (between '<' and '|')
  const char *start = strchr(msg, '<');
  if (start) {
    start++;
    const char *end = strchr(start, '|');
    if (end) {
      char state_buf[20] = {0};
      size_t len = end - start;
      if (len < sizeof(state_buf)) {
        strncpy(state_buf, start, len);
        state_buf[len] = '\0';
        status->communication_state = get_state_from_string(state_buf);
      }
    }
  }

  // Find MPos part
  const char *mpos = strstr(msg, "MPos:");
  if (mpos) {
    mpos += strlen("MPos:");
    float x = 0, y = 0, z = 0;
    if (sscanf(mpos, "%f,%f,%f", &x, &y, &z) == 3) {
      status->x = x;
      status->y = y;
      status->z = z;
    }
  }
}