#include "communication.h"
#include "stm32l4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



extern UART_HandleTypeDef huart2;


extern char command[BUFF_SIZE];

int handled = 0;

extern int state;

extern int argc;
extern char* argv[10];
extern int data_received_usb;
int toggle = 0;



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {  // Check which UART triggered the callback
    	data_received_usb = 1;
        HAL_UART_Receive_IT(&huart2, &command, BUFF_SIZE);  // Restart reception
    }
}

void transmit_to_pc(char* trame){
	char message[BUFF_SIZE] = {" "};
	snprintf(message, BUFF_SIZE, trame);
	HAL_UART_Transmit(&huart2, message, BUFF_SIZE, 1000);
}

