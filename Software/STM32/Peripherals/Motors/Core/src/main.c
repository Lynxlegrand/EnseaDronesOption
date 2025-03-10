#include "motor.h"
#include "stm32l4xx_hal.h"

// Déclaration des moteurs
h_motor_t motor1, motor2, motor3, motor4;

// Configuration des timers pour chaque moteur
void Motors_Init() {
    motor1.htim = &htim1;
    motor1.channel = TIM_CHANNEL_1;
    motor_Init(&motor1);

    motor2.htim = &htim1;
    motor2.channel = TIM_CHANNEL_2;
    motor_Init(&motor2);

    motor3.htim = &htim1;
    motor3.channel = TIM_CHANNEL_3;
    motor_Init(&motor3);

    motor4.htim = &htim1;
    motor4.channel = TIM_CHANNEL_4;
    motor_Init(&motor4);
}

// Fonction de test pour démarrer les moteurs à une puissance donnée
void Test_Motors(int power) {
    motor_SetPower(&motor1, power);
    motor_SetPower(&motor2, power);
    motor_SetPower(&motor3, power);
    motor_SetPower(&motor4, power);
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_TIM1_Init();

    // Initialisation des moteurs avec l'armement des ESC
    Motors_Init();

    while (1) {
        Test_Motors(50);  // Test à 50% de puissance
        HAL_Delay(5000);

        Test_Motors(0);  // Arrêt du moteur
        HAL_Delay(5000);
    }
}

