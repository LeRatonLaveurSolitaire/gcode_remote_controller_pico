// #include <Arduino.h>
// #include <U8g2lib.h>
// #include <Wire.h>
// #include <stdint.h>

// #define STATE_UNKNWON 0
// #define STATE_WAITING 1
// #define STATE_READY 2

// #define NUMBER_OF_AXIS 5

// #define AXIS_X 0
// #define AXIS_Y 1
// #define AXIS_Z 2
// #define AXIS_F 3
// #define AXIS_STEP 4

// #define MAX_SPEED 150
// #define MIN_SPEED 10

// #define MAX_STEP 10
// #define MIN_STEP 0.01

// #define SWITCH_ROTARY D20
// #define DT_ROTARY D21
// #define CLK_ROTARY D22

// struct global_status
// {
//     float x = 0;
//     float y = 0;
//     float z = 0;
//     float f = MIN_SPEED;
//     float step_size = MIN_STEP;
//     int selected_axis = AXIS_X;
//     int communication_state = STATE_UNKNWON;
// };

// int32_t encoder_value = 0;
// uint32_t last_isr_call = 0;
// int32_t flag = 0;
// int32_t trig = 0;
// char msg[20];

// global_status system_state;

// U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2);

// void Interrupt_encoder();
// void Interrupt_encoder_switch();
// void display();

// void setup()
// {
//     Wire.setSDA(8);
//     Wire.setSCL(9);
//     Wire.begin();
//     u8g2.begin();
//     u8g2.setFont(u8g2_font_7x14B_mf);
//     u8g2.firstPage();
//     do
//     {
//         u8g2.drawStr(30, 20, "Loading,");
//         u8g2.drawStr(5, 40, "Please wait...");
//     } while (u8g2.nextPage());

//     pinMode(SWITCH_ROTARY, INPUT_PULLDOWN);
//     pinMode(CLK_ROTARY, INPUT_PULLUP);
//     pinMode(DT_ROTARY, INPUT_PULLUP);

//     attachInterrupt(CLK_ROTARY, Interrupt_encoder, FALLING);
//     attachInterrupt(SWITCH_ROTARY, Interrupt_encoder_switch, FALLING);

//     Serial.begin(9600);
//     display();
// }

// void loop()
// {
//     if (flag)
//     {
//         Serial.print(encoder_value);
//         switch (system_state.selected_axis)
//         {
//         case AXIS_X:
//             system_state.x += trig * system_state.step_size;
//             break;
//         case AXIS_Y:
//             system_state.y += trig * system_state.step_size;
//             break;
//         case AXIS_Z:
//             system_state.z += trig * system_state.step_size;
//             break;
//         case AXIS_F:
//             system_state.f += 10 * trig;
//             system_state.f = constrain(system_state.f, MIN_SPEED, MAX_SPEED);
//             break;
//         case AXIS_STEP:
//             if (trig == 1)
//             {
//                 system_state.step_size *= 10;
//             }
//             else if (trig == -1)
//             {
//                 system_state.step_size /= 10;
//             }
//             system_state.step_size = constrain(system_state.step_size, MIN_STEP, MAX_STEP);
//             break;
//         default:
//             break;
//         }
//         Serial.print(" ");
//         Serial.println(trig);
//         flag = 0;
//         display();
//     }
// }

// void display()
// {

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
// }

// void Interrupt_encoder()
// {
//     if (millis() - last_isr_call > 20)
//     {
//         if (digitalRead(DT_ROTARY))
//         {
//             encoder_value++;
//             trig = 1;
//         }
//         else
//         {
//             encoder_value--;
//             trig = -1;
//         }
//         last_isr_call = millis();
//         flag = 1;
//     }
// }

// void Interrupt_encoder_switch()
// {
//     if (millis() - last_isr_call > 200)
//     {
//         system_state.selected_axis += 1;
//         system_state.selected_axis %= NUMBER_OF_AXIS;
//         flag = 1;
//         trig = 0;
//     }
//     last_isr_call = millis();
// }
