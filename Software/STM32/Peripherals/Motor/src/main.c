#include "motor.h"
#include "nrf24l01.h"
#include "stm32l4xx_hal.h"
#include <string.h>

// Déclaration des moteurs et variables
h_motor_t motor1;
int motor_speed = 50; // Vitesse initiale des moteurs

// Buffer de réception
char rxData[32];

// Initialisation des moteurs et NRF24L01
void System_Init() {
    // Initialisation des moteurs
    motor1.htim = &htim1;
    motor1.channel = TIM_CHANNEL_1;
    motor_Init(&motor1);

    // Initialisation du module NRF24L01
    NRF24_Init();
    NRF24_RxMode(); // Mode réception
    printf("NRF24L01 prêt en mode réception...\r\n");
}

// Ajustement de la vitesse du moteur
void Update_Motor_Speed() {
    motor_SetPower(&motor1, motor_speed);
    printf("Nouvelle vitesse moteur: %d%%\r\n", motor_speed);
}

// Lecture et traitement des commandes reçues
void Listen_For_Commands() {
    if (NRF24_Receive(rxData)) {
        if (strcmp(rxData, "UP") == 0 && motor_speed < 100) {
            motor_speed += 10;
        } else if (strcmp(rxData, "DOWN") == 0 && motor_speed > 0) {
            motor_speed -= 10;
        }
        Update_Motor_Speed();
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_TIM1_Init();

    System_Init(); // Initialisation du moteur et du NRF24L01

    while (1) {
        Listen_For_Commands();
        HAL_Delay(100); // Attente pour éviter une surcharge CPU
    }
}
