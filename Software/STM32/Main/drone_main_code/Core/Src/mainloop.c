#include "ESC.h"
#include "RF.h"
#include "IMU.h"
#include "ultrasound.h"
#include "variable.h"
#include "PID.h"

#include "stm32f4xx.h"
#include <math.h>

#include <stdlib.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;


int sample_time_us;
int time_to_reach_1m;
float height_step;
int time_to_make_full_rotation;
float yaw_step;
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


char command[8];



void init(){


	sample_time_us = 825;

	heightPID.sample_time = sample_time_us/1000000;
	pitchPID.sample_time = sample_time_us/1000000;
	rollPID.sample_time = sample_time_us/1000000;
	yawPID.sample_time = sample_time_us/1000000;


	time_to_reach_1m = 5; // seconds
	time_to_make_full_rotation = 5; // seconds

	height_step = sample_time_us/time_to_reach_1m;
	yaw_step = 360*sample_time_us/time_to_make_full_rotation;

	// Timer clock is 84 MHz

	htim3.Instance->PSC = 84-1;
	htim3.Instance->CNT = sample_time_us;


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

			float pitch_speed;
			float yaw_speed;
			float roll_speed;

			pitch.measurement = pitch.previous_measurement + pitch_speed*sample_time_us/1000000;
			pitch.previous_measurement = pitch.measurement;

			roll.measurement = roll.previous_measurement + roll_speed*sample_time_us/1000000;
			roll.previous_measurement = roll.measurement;

			yaw.measurement = yaw.previous_measurement + yaw_speed*sample_time_us/1000000;
			yaw.previous_measurement = yaw.measurement;


			
			read_ultrasound();

			//--------- Reading Commands ------------//
			
			read_RF();

			if (command[0]=='$'){// Verifying that the command was entirely received
				// Height command extraction
				if (command[1]=="1" && command[2]=="0"){
					height.command+=height_step;
				}
				else if (command[2]=="1" && command[1]=="0"){
					height.command-= height_step;
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
