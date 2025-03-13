#include "ESC.h"
#include "RF.h"
#include "IMU.h"
#include "ultrasound.h"
#include "variable.h"
#include "PID.h"

#include "stm32f4xx.h"
#include <math.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;


int sample_time_us;
control_variables height = {0};
control_variables pitch = {0};
control_variables yaw = {0};
control_variables roll = {0};

PID heightPID = {0};
PID pitchPID = {0};
PID rollPID = {0};
PID yawPID = {0};

h_motor_t MOTOR_FRONT_RIGHT;
h_motor_t MOTOR_FRONT_LEFT;
h_motor_t MOTOR_BACK_RIGHT;
h_motor_t MOTOR_BACK_LEFT;



void init(){


	sample_time_us = 825;

	// Timer clock is 84 MHz

	TIM3->PSC = 84-1;
	TIM3->CNT = sample_time_us;


	MOTOR_FRONT_RIGHT.htim = &htim1;
    MOTOR_FRONT_RIGHT.channel = TIM_CHANNEL_1;
    motor_Init(&MOTOR_FRONT_RIGHT);

	MOTOR_FRONT_LEFT.htim = &htim1;
    MOTOR_FRONT_LEFT.channel = TIM_CHANNEL_2;
    motor_Init(&MOTOR_FRONT_LEFT);

	MOTOR_BACK_RIGHT.htim = &htim1;
	MOTOR_BACK_RIGHT.channel = TIM_CHANNEL_3;
	motor_Init(&MOTOR_BACK_RIGHT);

	MOTOR_BACK_LEFT.htim = &htim1;
	MOTOR_BACK_LEFT.channel = TIM_CHANNEL_4;
	motor_Init(&MOTOR_BACK_LEFT);



}


void control_step(){
			//--------- Reading Sensors ------------//
			read_IMU();
			read_ultrasound();

			//--------- Reading Commands ------------//
			read_RF();


			//--------- Processing data ------------//
			// Calculating control inputs

			height.error = height.measurement - height.command;
			pitch.error = pitch.measurement - pitch.command;
			yaw.error = yaw.measurement - yaw.command;
			roll.error = roll.measurement - roll.command;

			// Calculating control outputs

			height.u = update(heightPID,height.error);
			pitch.u = update(pitchPID, pitch.error);
			yaw.u = update(yawPID, yaw.error);
			roll.u = update(rollPID,roll.error);

			//--------- Changing motors behaviour ------------//

			// motor mixing algorithm
			int FR_percentage = fmin(100, fmax(0, height.u+yaw.u+pitch.u+roll.u));
			int FL_percentage = fmin(100, fmax(0, height.u-yaw.u+pitch.u-roll.u));
			int BR_percentage = fmin(100, fmax(0, height.u-yaw.u-pitch.u+roll.u));
			int BL_percentage = fmin(100, fmax(0, height.u+yaw.u-pitch.u-roll.u));
			
			motor_SetPower(&MOTOR_FRONT_RIGHT, FR_percentage);
			motor_SetPower(&MOTOR_FRONT_LEFT, FL_percentage);
			motor_SetPower(&MOTOR_BACK_RIGHT, BR_percentage);
			motor_SetPower(&MOTOR_BACK_LEFT, BL_percentage);
}
