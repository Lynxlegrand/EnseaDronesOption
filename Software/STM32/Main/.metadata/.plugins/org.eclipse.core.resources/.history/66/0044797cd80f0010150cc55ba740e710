/*
 * communication.h
 *
 *  Created on: Mar 13, 2025
 *      Author: hugoc
 */

#ifndef INC_COMMUNICATION_H_
#define INC_COMMUNICATION_H_


#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFF_SIZE 16


extern UART_HandleTypeDef huart2;
extern char command[BUFF_SIZE];
extern int state;
extern int argc;
extern char* argv[10];
extern int data_received_usb;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void transmit_to_pc(char* trame);
void transmit_to_pc(char*);


#endif /* INC_COMMUNICATION_H_ */
