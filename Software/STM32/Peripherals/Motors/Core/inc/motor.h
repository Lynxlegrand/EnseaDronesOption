/*
 * motor.h
 *
 *  Created on: May 27, 2024
 *      Author: JoelC
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h" // Pour récupérer les types STM32

// Structure de gestion des moteurs
typedef struct {
    TIM_HandleTypeDef* htim;
    uint32_t channel;
    float PercentageOfTotalPower;
} h_motor_t;

// Déclaration des fonctions
int percentageToMicrosecondsAtHighState(int percentage);
void motor_Init(h_motor_t* h_motor);
void motor_SetPower(h_motor_t* h_motor, int percentage);
void motor_TurnOff(h_motor_t* h_motor);

#endif /* INC_MOTOR_H_ */
