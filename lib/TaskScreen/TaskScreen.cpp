#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <FreeRTOS.h>
#include <task.h>

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
    // u8g2.firstPage();
    // do
    // {
    //   // u8g2.drawXBMP(0, 0, 128, 64, &images[current_img * IMG_SIZE]);
    //   //u8g2
    
    // } while (u8g2.nextPage());
    // digitalWrite(LED_BUILTIN,1);
    // vTaskDelay(500);
    // digitalWrite(LED_BUILTIN,0);
    // vTaskDelay(500);
  }
}