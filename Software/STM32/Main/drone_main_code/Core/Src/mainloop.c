#include "ESC.h"
#include "RF.h"
#include "IMU.h"
#include "ultrasound.h"
#include "variable.h"
#include "PID.h"

#include "stm32f4xx.h"
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Peripherals handlers
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim10;
extern UART_HandleTypeDef huart2;


// Main loop refresh rate
int sample_time_us;

// Movement limits
int time_to_reach_1m;
float height_step;
int time_to_make_full_rotation;
float yaw_step;

// Variables storage structure
control_variables height = {0};
control_variables pitch = {0};
control_variables yaw = {0};
control_variables roll = {0};


// PIDs
PID heightPID = {0};
PID pitchPID = {0};
PID rollPID = {0};
PID yawPID = {0};

// Motors handlers
h_motor_t MOTOR_FRONT_RIGHT;
h_motor_t MOTOR_FRONT_LEFT;
h_motor_t MOTOR_BACK_RIGHT;
h_motor_t MOTOR_BACK_LEFT;



// Flag for security
int flight_allowed;



// Global variables that store measurements
float ultrasound_measure_cm = 0;
extern float accel_g[3], gyro_dps[3], gyro_angle[3];

// Command received via RF
char command[9];



void init(){

	// Variable initializations
	flight_allowed = 1;
	sample_time_us = 825;

	heightPID.sample_time = sample_time_us/1000000;
	pitchPID.sample_time = sample_time_us/1000000;
	rollPID.sample_time = sample_time_us/1000000;
	yawPID.sample_time = sample_time_us/1000000;


	time_to_reach_1m = 5; // seconds
	time_to_make_full_rotation = 5; // seconds

	height_step = sample_time_us/time_to_reach_1m;
	yaw_step = 360*sample_time_us/time_to_make_full_rotation;


	// Initialization of the mainloop timer
	htim3.Instance->PSC = 84-1;// Timer clock is 84 MHz
	htim3.Instance->CNT = sample_time_us;


	// IMU initialization

	IMU_Init();


	// Motors initialization
	MOTOR_FRONT_RIGHT.htim = &htim1;
    MOTOR_FRONT_RIGHT.channel = TIM_CHANNEL_1;
    if (motor_Init(&MOTOR_FRONT_RIGHT)== HAL_ERROR){
    	flight_allowed = 0;
    }


	MOTOR_FRONT_LEFT.htim = &htim1;
    MOTOR_FRONT_LEFT.channel = TIM_CHANNEL_2;
    if (motor_Init(&MOTOR_FRONT_LEFT)== HAL_ERROR){
        	flight_allowed = 0;
        }
	MOTOR_BACK_RIGHT.htim = &htim1;
	MOTOR_BACK_RIGHT.channel = TIM_CHANNEL_3;
	if (motor_Init(&MOTOR_BACK_RIGHT)== HAL_ERROR){
	    	flight_allowed = 0;
	    }

	MOTOR_BACK_LEFT.htim = &htim1;
	MOTOR_BACK_LEFT.channel = TIM_CHANNEL_4;
	if (motor_Init(&MOTOR_BACK_LEFT)== HAL_ERROR){
	    	flight_allowed = 0;
	    }


	// Indicator for sucess (user led)
	if (flight_allowed == 1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
	}
	else{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
	}


	// Timers start

	HAL_TIM_Base_Start(&htim5); // time reference
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1); // ultrasound trigger
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // ultrasound read
	HAL_TIM_Base_Start_IT(&htim2); // IMU trigger
	HAL_TIM_Base_Start_IT(&htim3); // main loop

}


void control_step(){

		if (flight_allowed==1){
			//--------- Reading Sensors ------------//

			roll.measurement = gyro_angle[0];
			pitch.measurement = gyro_angle[1];
			yaw.measurement = gyro_angle[2];

			

			height.measurement = ultrasound_measure_cm/100;

			//--------- Reading Commands ------------//
			
			read_RF();

			if (command[0]=='$'){// Verifying that the command was entirely received
				motor_SetPower(&MOTOR_FRONT_RIGHT, 0);
				motor_SetPower(&MOTOR_FRONT_LEFT, 0);
				motor_SetPower(&MOTOR_BACK_RIGHT, 0);
				motor_SetPower(&MOTOR_BACK_LEFT, 0);
			}
				// Height command extraction
				if (command[1]=="1" && command[2]=="0"){
					height.command+=height_step;
					//height.command = min(height.command, 1.5);
				}
				else if (command[2]=="1" && command[1]=="0"){
					height.command-= height_step;
					//height.command = max(height.command, 0);
				}


				// Pitch command extraction
				if (command[3]=="1" && command[4]=="0"){
					pitch.command=1;
				}
				else if (command[4]=="1" && command[3]=="0"){
					pitch.command=-1;
				}

				else{
					pitch.command=0;
				}


				// Roll command extraction
				if (command[5]=="1" && command[6]=="0"){
					roll.command=1;
				}
				else if (command[6]=="1" && command[5]=="0"){
					roll.command=-1;
				}

				else{
					roll.command=0;
				}

				// Yaw command extraction
				if (command[7]=="1" && command[8]=="0"){
					yaw.command+=yaw_step;
				}
				else if (command[8]=="1" && command[7]=="0"){
					yaw.command-= yaw_step;
				}

				if (strcmp(command, "$11111111")==0){
					flight_allowed = 0;
			}

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

		else{
			motor_SetPower(&MOTOR_FRONT_RIGHT, 0);
			motor_SetPower(&MOTOR_FRONT_LEFT, 0);
			motor_SetPower(&MOTOR_BACK_RIGHT, 0);
			motor_SetPower(&MOTOR_BACK_LEFT, 0);
		}

}
