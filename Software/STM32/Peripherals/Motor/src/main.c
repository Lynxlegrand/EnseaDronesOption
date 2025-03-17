#include "motor.h"
#include "stm32l4xx_hal.h"

// Déclaration du moteur et de la LED
h_motor_t motor1;
#define LED_GPIO_PORT GPIOB
#define LED_PIN GPIO_PIN_0

// Initialisation du moteur et de la LED
void System_Init() {
    // Associer le Timer et le canal PWM au moteur
    motor1.htim = &htim1;
    motor1.channel = TIM_CHANNEL_1;

    // Initialisation du moteur
    motor_Init(&motor1);

    // Démarrer le moteur à 50% de sa puissance
    motor_SetPower(&motor1, 50);

    // Configuration de la LED en sortie
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

// Callback appelé sur front montant du signal PWM
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM1) {
        HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN); // Changer l'état de la LED
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_TIM1_Init(); // Initialisation du Timer 1

    System_Init(); // Initialisation du moteur et de la LED

    while (1) {
        // Le moteur tourne constamment à 50%
        HAL_Delay(1000);
    }
}
