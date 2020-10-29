#ifndef GLOBAL_VARIABLES_CONSTANTS_H
#define GLOBAL_VARIABLES_CONSTANTS_H
const int NUMBER_OF_IMUS = 5;
int IMUoffsetValue = 0; //0-Mukherjee set, 4 = KIN-Yellow set, 8 = KIN-Blue set

int totalNumPCAVectors = 7;
const int lengthOfPCAVectors = NUMBER_OF_IMUS*3; //This has to be this value unless we ignore yaw

#define PI 3.14159265359

int LOOP_FREQUENCY = 100; //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes

bool keepGoing = true; //the loops will check this value
int fd_dongle = 0; // file handle for dongle stream. must be done globally for threading
int fd_teensy = 0;

//const float X_ROBOT_MIN = 0.0;		// Hey! Check these values!!!
//const float X_ROBOT_MAX = 0.50;		// Hey! Check these values!!!
const float Y_ROBOT_MIN = -0.05;
//const float Y_ROBOT_MAX = -0.04;
const float Z_ROBOT_MIN = 0.30;				
//const float Z_ROBOT_MAX = 0.72;

bool CartXOn	= true;
bool CartYOn	= true;
bool CartZOn	= true;
bool RotXOn		= true;
bool RotYOn		= true;
bool RotZOn		= true;
bool GrasperOn	= true;
bool CartControlOn	= true;
bool RotControlOn	= true;

float START_X		= 0.3;
float START_Y		= 0.4;
float START_Z		= 0.5;
float START_ROT_X	= PI/2;
float START_ROT_Y	= -PI/2;
float START_ROT_Z	= PI;

float bufferSize = 0.2; //DeadZone. Don't make it zero!
//float velocityScaleFactor = 10.0; //this is used to scale the velocity....

float F1Gain	= 0.5;
float F2Gain	= 0.5;
float F3Gain	= 0.5;
float F4Gain	= 10.0;
float F5Gain	= 10.0;
float F6Gain	= 10.0;
float F7Gain	= 10.0;
float parameters[7] = {F1Gain, F2Gain, F3Gain, F4Gain, F5Gain, F6Gain, F7Gain};

struct orientation
{
	float pitch = 0.0;
	float yaw   = 0.0;
	float roll  = 0.0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}orient[NUMBER_OF_IMUS]; //must be instantiated globally for threading

#endif //GLOBAL_VARIABLES_CONSTANTS_H
