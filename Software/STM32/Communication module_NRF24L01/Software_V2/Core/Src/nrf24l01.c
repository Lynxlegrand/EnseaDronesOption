/*
 * nrf24l01.c
 *
 *  Created on: Mar 19, 2025
 *      Author: hugof
 */

#include "main.h"
#include "nrf24l01.h"
#include <string.h>

extern SPI_HandleTypeDef hspi2;

void NRF24L01_WriteReg(uint8_t reg, uint8_t value) {
    uint8_t txData[2] = { NRF24L01_W_REGISTER | reg, value };
    NRF24L01_CSN_LOW();
    HAL_SPI_Transmit(&hspi2, txData, 2, 100);
    NRF24L01_CSN_HIGH();
}

uint8_t NRF24L01_ReadReg(uint8_t reg) {
    uint8_t txData[2] = { NRF24L01_R_REGISTER | reg, 0xFF };
    uint8_t rxData[2];

    NRF24L01_CSN_LOW();
    HAL_SPI_TransmitReceive(&hspi2, txData, rxData, 2, 100);
    NRF24L01_CSN_HIGH();

    return rxData[1];
}

void NRF24L01_Init(uint8_t mode) {
    NRF24L01_CE_LOW();
    HAL_Delay(100);

    NRF24L01_WriteReg(NRF24L01_CONFIG, mode);
    NRF24L01_WriteReg(NRF24L01_RF_CH, 76);
    NRF24L01_WriteReg(NRF24L01_RF_SETUP, 0x06);

    if (mode == 0x0B) {
        uint8_t addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
        NRF24L01_CSN_LOW();
        HAL_SPI_Transmit(&hspi2, (uint8_t[]){NRF24L01_W_REGISTER | NRF24L01_RX_ADDR_P0}, 1, 100);
        HAL_SPI_Transmit(&hspi2, addr, 5, 100);
        NRF24L01_CSN_HIGH();
    }

    HAL_Delay(10);
    NRF24L01_CE_HIGH();
}

void NRF24L01_SendData(uint8_t* data, uint8_t len) {
    NRF24L01_CE_LOW();
    HAL_Delay(10);

    NRF24L01_CSN_LOW();
    uint8_t cmd = NRF24L01_W_TX_PAYLOAD;
    HAL_SPI_Transmit(&hspi2, &cmd, 1, 100);
    HAL_SPI_Transmit(&hspi2, data, len, 100);
    NRF24L01_CSN_HIGH();

    NRF24L01_CE_HIGH();
    HAL_Delay(10);
    NRF24L01_CE_LOW();
}

uint8_t NRF24L01_ReceiveData(uint8_t* data) {
    NRF24L01_CSN_LOW();
    uint8_t cmd = NRF24L01_R_RX_PAYLOAD;
    HAL_SPI_Transmit(&hspi2, &cmd, 1, 100);
    HAL_SPI_Receive(&hspi2, data, 5, 100);
    NRF24L01_CSN_HIGH();
    return 1;
}
