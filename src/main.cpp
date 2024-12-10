#include <Arduino.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// define tasks 
void TaskScreen( void *pvParameters );
void TaskEncoder( void *pvParameters );
void TaskSerialManager( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {
  
    // Initialize the display



  Serial.begin(115200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }

  

  xTaskCreate(
    TaskEncoder
    ,  "Encoder"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  xTaskCreate(
    TaskSerialManager
    ,  "SerialManager"
    ,  1024  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void setup1(){
 

  xTaskCreate(
      TaskScreen
      ,  "Screen"   // A name just for humans
      ,  4096  // This stack size can be checked & adjusted by reading the Stack Highwater
      ,  NULL
      ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,  NULL );

}

void loop()
{
  // Empty. Things are done in Tasks.
}

void loop1()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskScreen(void *pvParameters){
  
  Wire.begin();
  u8g2.begin();
  for (;;) {// A Task shall never return or exit.
    u8g2.firstPage();
    do
    {
      // u8g2.drawXBMP(0, 0, 128, 64, &images[current_img * IMG_SIZE]);
      u8g2

    } while (u8g2.nextPage());
    vTaskDelay(200);
  }
}

void TaskEncoder(void *pvParameters){
  
  
  for (;;) {// A Task shall never return or exit.

    vTaskDelay(200);
  }
}

void TaskSerialManager(void *pvParameters){
  
  
  for (;;) {// A Task shall never return or exit.
    while(Serial.available()){

    }
    vTaskDelay(200);
  }
}

// void TaskBlink(void *pvParameters)  // This is a task.
// {
//   (void) pvParameters;


//   // initialize digital LED_BUILTIN on pin 13 as an output.
//   pinMode(LED_BUILTIN, OUTPUT);

//   for (;;) // A Task shall never return or exit.
//   {
//     digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//     vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
//     digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//     vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
//   }
// }

// void TaskAnalogRead(void *pvParameters)  // This is a task.
// {
//   (void) pvParameters;
  
// /*
//   AnalogReadSerial
//   Reads an analog input on pin 0, prints the result to the serial monitor.
//   Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
//   Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

//   This example code is in the public domain.
// */

//   for (;;)
//   {
//     // read the input on analog pin 0:
//     int sensorValue = analogRead(A0);
//     // print out the value you read:
//     Serial.println(sensorValue);
//     vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
//   }
// }