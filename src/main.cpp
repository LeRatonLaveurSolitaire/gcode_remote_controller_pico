#include <Arduino.h>

#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include "TaskList.h"
#include <stdint.h>
#include <Wire.h>



// the setup function runs once when you press reset or power the board
void setup() {
  
  pinMode(LED_BUILTIN,OUTPUT);

  Serial.begin(9600);
  // Wire.setSDA(8);
  // Wire.setSCL(9);
  Wire.begin();


  xTaskCreate(
    TaskEncoder
    ,  "Encoder"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );

  xTaskCreate(
    TaskSerialManager
    ,  "SerialManager"
    ,  1024  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );

  xTaskCreate(
    TaskScreen
    ,  "Screen"   // A name just for humans
    ,  4096  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

}

// void setup1(){

// }

void loop()
{
  // Empty. Things are done in Tasks.
}

// void loop1()
// {
//   // Empty. Things are done in Tasks.
// }

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/




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