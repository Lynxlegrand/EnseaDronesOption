#include "main.h"
#include <stdio.h>
#include <stdlib.h>


int pulse_duration;
int measure_cm;
extern UART_HandleTypeDef huart2;


extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

extern UART_HandleTypeDef huart2;


void run(){

    HAL_TIM_Base_Start(&htim5);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);

    char message[1024] = {0};
	sprintf(message, "Starting...\n\r");
	HAL_UART_Transmit(&huart2, &message, sizeof(message), 1000);

    while(1){
        char message[1024] = {0};
        sprintf(message, "Distance measured : %d cm\n\r", measure_cm);
        HAL_UART_Transmit(&huart2, &message, sizeof(message), 1000);
        pulse_duration = -1;
        HAL_Delay(1000);  // delay 1 second between each measurement

    }

}
