

typedef struct{
	float kp;
	float kd;
	float ki;

	float previous_error;
	float previous_integrated_error;

	float sample_time;
}PID;


float integrate_error(float new_error_point, float previous_integrated_error, float sample_time);
float differentiate_error(float new_error_point, float previous_error, float sample_time);
float update(PID pid, float error);

