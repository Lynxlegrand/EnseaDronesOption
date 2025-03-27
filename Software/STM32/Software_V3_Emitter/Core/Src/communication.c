#include "communication.h"


extern char command[BUFF_SIZE];
extern int data_received_usb;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {  // Check which UART triggered the callback
    	data_received_usb = 1;
        HAL_UART_Receive_IT(&huart2, (uint8_t*)command, BUFF_SIZE);  // Restart reception
    }
}

void transmit_to_pc(char* trame)
{
	char message[BUFF_SIZE] = {" "};
	snprintf(message, BUFF_SIZE, trame);
	HAL_UART_Transmit(&huart2, (uint8_t*)message, BUFF_SIZE, HAL_MAX_DELAY);
}
