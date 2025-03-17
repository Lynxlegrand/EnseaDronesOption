#include "motor.h"
#include "stm32l4xx_hal.h"

// Declaration of the motor
h_motor_t motor1;

// Motor and LED initialization
void System_Init() {
    // Associate the Timer and PWM channel with the motor
    motor1.htim = &htim1;
    motor1.channel = TIM_CHANNEL_1;

    // Initialize the motor
    motor_Init(&motor1);

    // Start the motor at 50% power
    motor_SetPower(&motor1, 50);
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_TIM1_Init(); // Initialize Timer 1

    System_Init(); // Initialize the motor

    while (1) {
        // The motor runs constantly at 50%
        HAL_Delay(1000);
    }
}
