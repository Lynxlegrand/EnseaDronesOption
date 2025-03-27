#include "PID.h"

float integrate_error(float new_error_point, float previous_integrated_error, float sample_time){
	return previous_integrated_error + new_error_point*sample_time;
}

float differentiate_error(float new_error_point, float previous_error, float sample_time){
	return (new_error_point-previous_error)/sample_time;
}


float update(PID pid, float error){
	float p_term = pid.kp*error;
	float i_term = pid.ki*integrate_error(error, pid.previous_integrated_error, pid.sample_time);
	pid.previous_integrated_error = i_term;
	float d_term = pid.kd*differentiate_error(error, pid.previous_error, pid.sample_time);
	pid.previous_error = error;
	return p_term+i_term+d_term;
}





