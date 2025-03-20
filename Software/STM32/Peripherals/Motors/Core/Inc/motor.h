#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

<<<<<<< HEAD:Software/STM32/Peripherals/Motors/Core/Inc/motor.h
#include "main.h"

// Motors management
=======
#include "main.h" // To include STM32 types

// Motor management structure
>>>>>>> 1757674ef70307a1e5354c0b158612a1be97031c:Software/STM32/Peripherals/Motor/inc/motor.h
typedef struct {
	TIM_HandleTypeDef* htim;
	uint32_t channel;
	float PercentageOfTotalPower;
} h_motor_t;

<<<<<<< HEAD:Software/STM32/Peripherals/Motors/Core/Inc/motor.h
=======
// Function declarations
>>>>>>> 1757674ef70307a1e5354c0b158612a1be97031c:Software/STM32/Peripherals/Motor/inc/motor.h
int percentageToMicrosecondsAtHighState(int percentage);
void motor_Init(h_motor_t* h_motor);
void motor_SetPower(h_motor_t* h_motor, int percentage);
void motor_TurnOff(h_motor_t* h_motor);

#endif /* INC_MOTOR_H_ */
