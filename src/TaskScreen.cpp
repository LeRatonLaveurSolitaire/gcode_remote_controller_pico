#include <Arduino.h>
#include <FreeRTOS.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <status.h>
#include <task.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void TaskScreen(void* pvParameters) {
  Wire.begin();
  u8g2.begin();
  u8g2.setFont(u8g2_font_7x14B_mf);
  u8g2.firstPage();
  do {
    u8g2.drawStr(30, 30, "Loading");
    u8g2.drawStr(15, 50, "Please wait...");
  } while (u8g2.nextPage());

  char msg[20];
  global_status system_state_local;

  for (;;) {  // A Task shall never return or exit.

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
    u8g2.clearBuffer();

    u8g2.firstPage();
    do {
      (system_state_local.selected_axis == AXIS_X) ? u8g2.setDrawColor(0)
                                                   : u8g2.setDrawColor(1);

      u8g2.setCursor(3, 16 * 2 - 2);
      u8g2.print("X");
      u8g2.setDrawColor(1);
      sprintf(msg, " %03.2f ", system_state_local.x);
      u8g2.print(msg);

      (system_state_local.selected_axis == AXIS_Y) ? u8g2.setDrawColor(0)
                                                   : u8g2.setDrawColor(1);
      u8g2.setCursor(3, 16 * 3 - 2);
      u8g2.print("Y");
      u8g2.setDrawColor(1);
      sprintf(msg, " %03.2f ", system_state_local.y);
      u8g2.print(msg);

      (system_state_local.selected_axis == AXIS_Z) ? u8g2.setDrawColor(0)
                                                   : u8g2.setDrawColor(1);
      u8g2.setCursor(3, 16 * 4 - 2);
      u8g2.print("Z");
      u8g2.setDrawColor(1);
      sprintf(msg, " %3.2f", system_state_local.z);
      u8g2.print(msg);

      u8g2.setDrawColor(1);
      u8g2.setCursor(66, 16 * 2 - 2);
      u8g2.print("F");
      u8g2.setDrawColor(1);
      sprintf(msg, "    %3.0f", system_state_local.f);
      u8g2.print(msg);

      u8g2.setDrawColor(1);
      u8g2.setCursor(66, 16 * 3 - 2);
      u8g2.print("Step");
      system_state_local.step_size < 0.5
          ? sprintf(msg, " %.2f", system_state_local.step_size)
          : sprintf(msg, "  %2.0f", system_state_local.step_size);
      u8g2.print(msg);

      u8g2.setCursor(66, 16 * 4 - 2);
      sprintf(msg, "Select %c", axis_chr[system_state_local.selected_axis]);
      u8g2.print(msg);

      u8g2.setCursor(66, 16 * 1 - 2);
      u8g2.print(system_state_local.axis_is_selected);

      switch (system_state_local.communication_state) {
        case STATE_UNKNWON:
          u8g2.drawStr(3, 16 * 1 - 2, "UKNW");
          break;
        case STATE_IDLE:
          u8g2.drawStr(3, 16 * 1 - 2, "IDLE");
          break;
        case STATE_WORK:
          u8g2.drawStr(3, 16 * 1 - 2, "WORK");
          break;
        default:
          break;
      }

    } while (u8g2.nextPage());

    vTaskDelay(200);

    // digitalWrite(LED_BUILTIN,1);
    // vTaskDelay(500);
    // digitalWrite(LED_BUILTIN,0);
    // vTaskDelay(500);
  }
}