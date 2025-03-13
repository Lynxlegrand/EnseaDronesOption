#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

typedef struct {
    TIM_HandleTypeDef* htim;
    int channel;
    float PercentageOfTotalPower;
} h_motor_t;

int percentageToMicrosecondsAtHighState(int percentage);
void motor_Init(h_motor_t* h_motor);
void motor_SetPower(h_motor_t* h_motor, int percentage);
void motor_TurnOff(h_motor_t* h_motor);


#endif /* INC_MOTOR_H_ */

