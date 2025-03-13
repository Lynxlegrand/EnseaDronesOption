#ifndef NRF24L01_H
#define NRF24L01_H

#include "stm32l4xx_hal.h"

// Commandes de base du NRF24L01
#define NRF24L01_CMD_R_REGISTER   0x00
#define NRF24L01_CMD_W_REGISTER   0x20
#define NRF24L01_CMD_ACTIVATE     0x50
#define NRF24L01_CMD_R_RX_PAYLOAD 0x61
#define NRF24L01_CMD_W_TX_PAYLOAD 0xA0
#define NRF24L01_CMD_FLUSH_TX     0xE1
#define NRF24L01_CMD_FLUSH_RX     0xE2
#define NRF24L01_CMD_REUSE_TX_PL  0xE3
#define NRF24L01_CMD_NOP          0xFF

// Registres du NRF24L01
#define NRF24L01_REG_CONFIG        0x00
#define NRF24L01_REG_EN_AA         0x01
#define NRF24L01_REG_EN_RXADDR     0x02
#define NRF24L01_REG_SETUP_AW      0x03
#define NRF24L01_REG_SETUP_RETR    0x04
#define NRF24L01_REG_RF_CH         0x05
#define NRF24L01_REG_RF_SETUP      0x06
#define NRF24L01_REG_STATUS        0x07
#define NRF24L01_REG_OBSERVE_TX    0x08
#define NRF24L01_REG_CD            0x09
#define NRF24L01_REG_RX_ADDR_P0    0x0A
#define NRF24L01_REG_RX_ADDR_P1    0x0B
#define NRF24L01_REG_RX_ADDR_P2    0x0C
#define NRF24L01_REG_RX_ADDR_P3    0x0D
#define NRF24L01_REG_RX_ADDR_P4    0x0E
#define NRF24L01_REG_RX_ADDR_P5    0x0F
#define NRF24L01_REG_TX_ADDR       0x10
#define NRF24L01_REG_RX_PW_P0      0x11
#define NRF24L01_REG_RX_PW_P1      0x12
#define NRF24L01_REG_RX_PW_P2      0x13
#define NRF24L01_REG_RX_PW_P3      0x14
#define NRF24L01_REG_RX_PW_P4      0x15
#define NRF24L01_REG_RX_PW_P5      0x16
#define NRF24L01_REG_FIFO_STATUS   0x17
#define NRF24L01_REG_DYNPD         0x1C
#define NRF24L01_REG_FEATURE       0x1D

// Configuration des registres
#define NRF24L01_CONFIG_PWR_UP     (1 << 1)
#define NRF24L01_CONFIG_PRIM_RX    (1 << 0)

// Déclaration des fonctions
void SPI_init(SPI_HandleTypeDef *hspi);
uint8_t SPI_transmit(SPI_HandleTypeDef *hspi, uint8_t data);
void NRF24L01_init(SPI_HandleTypeDef *hspi);
void NRF24L01_write_register(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value);
void NRF24L01_send(SPI_HandleTypeDef *hspi, uint8_t *data, uint8_t length);
uint8_t NRF24L01_receive(SPI_HandleTypeDef *hspi, uint8_t *data, uint8_t max_length);

#endif // NRF24L01_H
