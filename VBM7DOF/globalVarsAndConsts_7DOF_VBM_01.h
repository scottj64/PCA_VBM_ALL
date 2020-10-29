#ifndef GLOBAL_VARIABLES_CONSTANTS_H
#define GLOBAL_VARIABLES_CONSTANTS_H
const int NUMBER_OF_IMUS = 5;
const int IMUoffsetValue = 0;
const double PI = 3.14159265359;


//pretty numbers <=100 for 1,000,000 / x = round number: x= 1, 2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 64, 80, 100

int LOOP_FREQUENCY = 100; //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes

int IMU_FREQUENCY = 100; //cycles per second
// only need to go about twice as fast as the matlab code is able to refresh which is ~14 Hz and I want it to be a nice number

int MATLAB_FREQUENCY = 100; //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes
//Not implemented as Matlab isn't cycling fast enough to matter

bool keepGoing = true; //the loops will check this value
int  fd_dongle = 0; // file handle for dongle stream. must be done globally for threading
int  fd_teensy = 0;


float F1Thresh      = 0.5e3;
float F1Gain        = 5e-6;
float F2Thresh      = 1e3;
float F2Gain        = 1.4e-6;
float F3Thresh      = 0.2e4;
float F3Gain        = 7e-6;
float F4Thresh      = 1e3;
float F4Gain        = 10e-5;
float F5Thresh      = 1e3;
float F5Gain        = 10e-5;
float F6Thresh      = 2.5e3;
float F6Gain        = 5e-5;
float F7ThreshOpen  = 3e4;
float F7ThreshClose = 1e4;
float F7Gain        = 6e-6;

float parametersM[15] = {F1Thresh,F1Gain,F2Thresh,F2Gain,F3Thresh,F3Gain,F4Thresh,F4Gain,F5Thresh,F5Gain,F6Thresh,F6Gain,F7ThreshClose,F7ThreshOpen,F7Gain};

bool CartControlOn	= true;
bool RotControlOn	= true;
bool CartXOn		= true;
bool CartYOn		= true;
bool CartZOn		= true;
bool RotXOn			= true;
bool RotYOn			= true;
bool RotZOn			= true;
bool GrasperOn		= true;

float START_X		= 0.4;
float START_Y		= 0.4;
float START_Z		= 0.5;
float START_ROT_X	= PI/2;
float START_ROT_Y	= -PI/2;
float START_ROT_Z	= PI;

//const float X_ROBOT_MIN = 0.0;		// Hey! Check these values!!!
//const float X_ROBOT_MAX = 0.50;		// Hey! Check these values!!!
const float Y_ROBOT_MIN = -0.05;
//const float Y_ROBOT_MAX = -0.04;
const float Z_ROBOT_MIN = 0.30;				
//const float Z_ROBOT_MAX = 0.72;


const float MAX_FINGER_CLOSE  = 6000.0;
const float MIN_FINGER_OPEN = 0000.0;


struct orientation
{
	float pitch = 0.0;
	float yaw   = 0.0;
	float roll  = 0.0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}orient[NUMBER_OF_IMUS]; //must be instantiated globally for threading

struct force
{
	float P1 = 0.0;
	float P2 = 0.0;
	float P3 = 0.0;
	float P4 = 0.0; // roll
	float P5 = 0.0; // pitch
	float P6 = 0.0; // yaw
	float P7 = 0.0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}forces; //must be instantiated globally for threading


pthread_mutex_t Matlab_initialized_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ready_to_loop_mutex = PTHREAD_MUTEX_INITIALIZER;


#endif //GLOBAL_VARIABLES_CONSTANTS_H
