#include "ESC.h"
#include "RF.h"
#include "IMU.h"
#include "ultrasound.h"
#include "variable.h"
#include "PID.h"


void run(){

	control_variables height = {0};
	control_variables pitch = {0};
	control_variables yaw = {0};
	control_variables roll = {0};

	PID heightPID = {0};
	PID pitchPID = {0};
	PID rollPID = {0};
	PID yawPID = {0};


	while(1){
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
		set_ESC(MOTOR_FRONT_RIGHT, height.u+yaw.u+pitch.u+roll.u);
		set_ESC(MOTOR_FRONT_LEFT, height.u-yaw.u+pitch.u-roll.u);
		set_ESC(MOTOR_BACK_RIGHT, height.u-yaw.u-pitch.u+roll.u);
		set_ESC(MOTOR_BACK_LEFT, height.u+yaw.u-pitch.u-roll.u);




	}
}
