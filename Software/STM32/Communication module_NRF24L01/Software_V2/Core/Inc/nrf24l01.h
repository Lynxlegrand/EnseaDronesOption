/*
 * nrf24l01.h
 *
 *  Created on: Mar 19, 2025
 *      Author: hugof
 */

#ifndef INC_NRF24L01_H_
#define INC_NRF24L01_H_

#include "main.h"

// Définition des broches CE et CSN
#define NRF24L01_CE_HIGH()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)
#define NRF24L01_CE_LOW()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)
#define NRF24L01_CSN_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define NRF24L01_CSN_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

// Commandes NRF24L01
#define NRF24L01_W_REGISTER  0x20
#define NRF24L01_R_REGISTER  0x00
#define NRF24L01_R_RX_PAYLOAD 0x61
#define NRF24L01_W_TX_PAYLOAD 0xA0
#define NRF24L01_FLUSH_TX 0xE1
#define NRF24L01_FLUSH_RX 0xE2
#define NRF24L01_CONFIG 0x00
#define NRF24L01_RF_CH 0x05
#define NRF24L01_RF_SETUP 0x06
#define NRF24L01_STATUS 0x07
#define NRF24L01_TX_ADDR 0x10
#define NRF24L01_RX_ADDR_P0 0x0A

// Fonctions SPI
void NRF24L01_Init(uint8_t mode);
void NRF24L01_WriteReg(uint8_t reg, uint8_t value);
uint8_t NRF24L01_ReadReg(uint8_t reg);
void NRF24L01_SendData(uint8_t* data, uint8_t len);
uint8_t NRF24L01_ReceiveData(uint8_t* data);

#endif /* INC_NRF24L01_H_ */
