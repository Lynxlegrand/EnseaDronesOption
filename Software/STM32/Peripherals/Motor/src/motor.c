#include "motor.h"
#include "stm32l4xx_hal.h"
#include "stdio.h"

#define COUNTER_PERIOD 19999 // Maximal counter value
#define MIN_POWER_DUTY_CYCLE 6 //%
#define MAX_POWER_DUTY_CYCLE 10 //%

void motor_ArmESC(h_motor_t* h_motor) {
    motor_SetPower(h_motor, 0);  // Set motor power to 0
    HAL_Delay(3000);  // Wait for 3 seconds

    motor_SetPower(h_motor, 7);  // Set motor power to 7%
    HAL_Delay(2000);  // Wait for 2 seconds
}

int percentageToMicrosecondsAtHighState(int percentage) {
    int neededDutyCycle = MIN_POWER_DUTY_CYCLE + (MAX_POWER_DUTY_CYCLE - MIN_POWER_DUTY_CYCLE) * 0.01 * percentage;
    return (int)((COUNTER_PERIOD + 1) * 0.01 * neededDutyCycle);  // Convert percentage to microseconds at high state
}

void motor_SetPower(h_motor_t* h_motor, int percentage) {
    int microsecondsAtHighState = percentageToMicrosecondsAtHighState(percentage);
    __HAL_TIM_SET_COMPARE(h_motor->htim, h_motor->channel, microsecondsAtHighState);  // Set PWM duty cycle
    h_motor->PercentageOfTotalPower = percentage;  // Update the motor power percentage
}

void motor_Init(h_motor_t* h_motor) {
    HAL_TIM_PWM_Start(h_motor->htim, h_motor->channel);  // Start PWM signal

    // ESC-specific initialization
    motor_ArmESC(h_motor);  // Arm ESC to prepare for operation
}

void motor_TurnOff(h_motor_t* h_motor) {
    motor_SetPower(h_motor, 0);  // Set motor power to 0 to turn it off
}
