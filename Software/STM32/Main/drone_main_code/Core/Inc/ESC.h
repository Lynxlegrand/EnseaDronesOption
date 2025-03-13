#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

typedef struct {
    TIM_HandleTypeDef* htim;
    uint32_t channel;
    float PercentageOfTotalPower;
} h_motor_t;

int percentageToMicrosecondsAtHighState(int percentage);
void motor_Init(h_motor_t* h_motor);
void motor_SetPower(h_motor_t* h_motor, int percentage);
void motor_TurnOff(h_motor_t* h_motor);

#define MOTOR_FRONT_LEFT 0
#define MOTOR_FRONT_RIGHT 1
#define MOTOR_BACK_LEFT 2
#define MOTOR_BACK_RIGHT 3

#endif

