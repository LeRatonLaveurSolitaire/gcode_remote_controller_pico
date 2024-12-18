#ifndef STATUS_H
#define STATUS_H

#include "FreeRTOS.h"
#include "semphr.h"

#define STATE_UNKNWON 0
#define STATE_WORK 1
#define STATE_IDLE 2

#define NUMBER_OF_AXIS 3

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

#define MAX_SPEED 500
#define MIN_SPEED 10

#define MAX_STEP 10
#define MIN_STEP 0.01

struct {
  float x = 0;
  float y = 0;
  float z = 0;
  float f = MIN_SPEED;
  float step_size = MAX_STEP;
  bool axis_is_selected = false;
  int selected_axis = AXIS_X;
  int communication_state = STATE_UNKNWON;
} typedef global_status;

extern char axis_chr[];

extern global_status system_state;
extern SemaphoreHandle_t xStatusMutex;
extern QueueHandle_t xCommmandQueue;

#endif