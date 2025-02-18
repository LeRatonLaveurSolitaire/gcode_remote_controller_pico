#include <Arduino.h>
#include <FreeRTOS.h>
#include <status.h>
#include <stdio.h>
#include <task.h>


#define PIN_ENC_READ 7
#define PIN_ENC_TRIG 8

volatile int encoder_val;

void TaskEncoder(void* pvParameters) {
  
  pinMode(PIN_ENC_READ, INPUT);
  pinMode(PIN_ENC_TRIG, INPUT);
  encoder_val = 0;
  
  for (;;) { 

  /**
 * Get the encoder delta (-2 -1 0 +1 +2) since the last call, reading the live encoder state.
 * Pins may be debounced to filter noise.
 */

  typedef struct { bool a:1, b:1; } enc_t;

  const enc_t enc = { (bool)digitalRead(PIN_ENC_TRIG),(bool)digitalRead(PIN_ENC_READ) };


  static uint8_t old_pos;
  const uint8_t pos = (enc.a ^ enc.b) | (enc.a << 1); // 0:00  1:10  2:11  3:01
  int8_t delta = 0;
  if (pos != old_pos) {
    delta = (pos - old_pos + 4 + 1) % 4 - 1;
    old_pos = pos;

    static int8_t last_dir;
    if (delta == 2) delta = last_dir * 2;
    else last_dir = delta;

  }
  encoder_val += delta;

  vTaskDelay(2);
  }

}