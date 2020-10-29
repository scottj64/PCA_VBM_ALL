#ifndef SHARED_GLOBAL_VARIABLES_CONSTANTS_H
#define SHARED_GLOBAL_VARIABLES_CONSTANTS_H
const int NUMBER_OF_IMUS = 5;
const int IMUoffsetValue = 0; //NOT APPLICABLE 0-Mukherjee set, 4 = KIN-Yellow set, 8 = KIN-Blue set
const double PI = 3.14159265359;

//pretty numbers <=100 for 1,000,000 / x = round number: x= 1, 2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 64, 80, 100

const int LOOP_FREQUENCY = 100; //cycles per second
//no point going anything other than 100Hz as that is how fast the Jaco arm controller refreshes

const int IMU_FREQUENCY = 100; //cycles per second
// only need to go about twice as fast as the matlab code is able to refresh which is ~14 Hz and I want it to be a nice number

bool keepGoing = true; //the loops will check this value
int  fd_dongle = 0; // file handle for dongle stream. must be done globally for threading
int  fd_teensy = 0;

bool CartControlOn	= true;
bool RotControlOn	= true;
bool CartXOn		= false;
bool CartYOn		= false;
bool CartZOn		= false;
bool RotXOn			= false;
bool RotYOn			= false;
bool RotZOn			= false;
bool GrasperOn		= false;

float START_X		= 0.0;
float START_Y		= 0.25;
float START_Z		= 0.45;
float START_ROT_X	= PI/2;
float START_ROT_Y	= -PI/2;
float START_ROT_Z	= PI;

const float X_ROBOT_MIN = 0.07;		
const float X_ROBOT_MAX = 0.55;		//matters
const float Y_ROBOT_MIN = 0.00;
const float Y_ROBOT_MAX = 0.48;		//matters
const float Z_ROBOT_MIN = 0.18;		//matters
const float Z_ROBOT_MAX = 0.70;

const float MAX_FINGER_CLOSE  = 6250.0;
const float MIN_FINGER_OPEN = 0000.0;

const float baseTipLEDOffsetX =  0.00;
const float baseTipLEDOffsetY =  0.00;
const float baseTipLEDOffsetZ =  0.10;

int currentlyOccupiedSlot_GLOBAL = -1;
pthread_mutex_t slot_mutex = PTHREAD_MUTEX_INITIALIZER;

struct orientation
{
	float pitch = 0.0;
	float yaw   = 0.0;
	float roll  = 0.0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}orient[NUMBER_OF_IMUS]; //must be instantiated globally for threading

#endif //SHARED_GLOBAL_VARIABLES_CONSTANTS_H
