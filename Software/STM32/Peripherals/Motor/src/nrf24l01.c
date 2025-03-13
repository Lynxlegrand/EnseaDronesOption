#include "nrf24l01.h"

// Initialisation de la communication SPI
void SPI_init(SPI_HandleTypeDef *hspi) {
    // Cette fonction est responsable de l'initialisation de l'interface SPI
    // STM32CubeIDE configure déjà les périphériques SPI dans le fichier `spi.c`.
}

// Fonction pour transmettre un octet via SPI
uint8_t SPI_transmit(SPI_HandleTypeDef *hspi, uint8_t data) {
    uint8_t receivedData;
    HAL_SPI_TransmitReceive(hspi, &data, &receivedData, 1, 1000);  // Transmit/receive un octet
    return receivedData;
}

// Initialiser le module NRF24L01
void NRF24L01_init(SPI_HandleTypeDef *hspi) {
    // Configurer le module NRF24L01 en mode réception (prêt à recevoir des données)

    // Configuration du registre CONFIG pour activer le module et passer en mode réception
    NRF24L01_write_register(hspi, NRF24L01_REG_CONFIG, NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_PRIM_RX);

    // Autres initialisations nécessaires pour le NRF24L01 peuvent être ajoutées ici
}

// Fonction d'écriture dans un registre du NRF24L01
void NRF24L01_write_register(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value) {
    uint8_t txData[2] = { reg, value };
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  // CSN Low
    SPI_transmit(hspi, NRF24L01_CMD_W_REGISTER | reg);
    SPI_transmit(hspi, value);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);    // CSN High
}

// Envoyer un message via le NRF24L01
void NRF24L01_send(SPI_HandleTypeDef *hspi, uint8_t *data, uint8_t length) {
    uint8_t txData[32] = {0};

    // Remplir le buffer de transmission
    for (uint8_t i = 0; i < length; i++) {
        txData[i] = data[i];
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  // CSN Low
    SPI_transmit(hspi, NRF24L01_CMD_W_TX_PAYLOAD);
    for (uint8_t i = 0; i < length; i++) {
        SPI_transmit(hspi, txData[i]);
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);    // CSN High

    // Activer la transmission
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);    // CE High
    HAL_Delay(1);  // Attendre un peu
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);  // CE Low
}

// Recevoir un message via le NRF24L01
uint8_t NRF24L01_receive(SPI_HandleTypeDef *hspi, uint8_t *data, uint8_t max_length) {
    uint8_t length = 0;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);  // CSN Low
    SPI_transmit(hspi, NRF24L01_CMD_R_RX_PAYLOAD);

    while (length < max_length) {
        data[length] = SPI_transmit(hspi, 0xFF);  // Lire la donnée
        length++;
    }

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);    // CSN High
    return length;
}
