#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <status.h>



U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void TaskScreen(void *pvParameters){
  u8g2.begin();
  u8g2.setFont(u8g2_font_7x14B_mf);
  u8g2.firstPage();
  do
  {
    u8g2.drawStr(30, 20, "Loading,");
    u8g2.drawStr(5, 40, "Please wait...");
  } while (u8g2.nextPage());



  

  for (;;) {// A Task shall never return or exit.

  
//     u8g2.clearBuffer();

//     u8g2.firstPage();
//     do
//     {
//         if (system_state.selected_axis == AXIS_X)
//         {
//             u8g2.setDrawColor(0);
//         }
//         else
//         {
//             u8g2.setDrawColor(1);
//         }
//         u8g2.setCursor(3, 14);
//         u8g2.print("X");
//         u8g2.setDrawColor(1);
//         sprintf(msg, " %03.2f ", system_state.x);
//         u8g2.print(msg);

//         if (system_state.selected_axis == AXIS_Y)
//         {
//             u8g2.setDrawColor(0);
//         }
//         else
//         {
//             u8g2.setDrawColor(1);
//         }
//         u8g2.setCursor(3, 16 * 2 - 2);
//         u8g2.print("Y");
//         u8g2.setDrawColor(1);
//         sprintf(msg, " %03.2f ", system_state.y);
//         u8g2.print(msg);

//         if (system_state.selected_axis == AXIS_Z)
//         {
//             u8g2.setDrawColor(0);
//         }
//         else
//         {
//             u8g2.setDrawColor(1);
//         }
//         u8g2.setCursor(3, 16 * 3 - 2);
//         u8g2.print("Z");
//         u8g2.setDrawColor(1);
//         sprintf(msg, " %3.2f", system_state.z);
//         u8g2.print(msg);

//         if (system_state.selected_axis == AXIS_F)
//         {
//             u8g2.setDrawColor(0);
//         }
//         else
//         {
//             u8g2.setDrawColor(1);
//         }
//         u8g2.setCursor(66, 16 * 1 - 2);
//         u8g2.print("F");
//         u8g2.setDrawColor(1);
//         sprintf(msg, "    %3.0f", system_state.f);
//         u8g2.print(msg);

//         if (system_state.selected_axis == AXIS_STEP)
//         {
//             u8g2.setDrawColor(0);
//         }
//         else
//         {
//             u8g2.setDrawColor(1);
//         }
//         u8g2.setCursor(66, 16 * 2 - 2);
//         u8g2.print("Step");
//         u8g2.setDrawColor(1);

//         system_state.step_size < 0.5 ? sprintf(msg, " %.2f", system_state.step_size) : sprintf(msg, "  %2.0f", system_state.step_size);
//         u8g2.print(msg);

//         switch (system_state.communication_state)
//         {
//         case STATE_UNKNWON:
//             u8g2.drawStr(3, 62, "Status: Unknown");
//             break;
//         case STATE_WAITING:
//             u8g2.drawStr(3, 62, "Status: Waiting");
//             break;
//         case STATE_READY:
//             u8g2.drawStr(3, 62, "Status: Ready");
//             break;
//         default:
//             break;
//         }

//     } while (u8g2.nextPage());




    // digitalWrite(LED_BUILTIN,1);
    // vTaskDelay(500);
    // digitalWrite(LED_BUILTIN,0);
    // vTaskDelay(500);
  }
}