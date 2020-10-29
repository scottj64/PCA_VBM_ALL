#ifndef VBM_GLOBAL_VARIABLES_CONSTANTS_H
#define VBM_GLOBAL_VARIABLES_CONSTANTS_H

#include "../SharedFiles/globalVarsAndConsts_7DOF_Shared_01.h"

const int MATLAB_FREQUENCY = 100; //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes


// Threshold is checked with respect to the VBM model.
// THEN the gains are applied to turn them into real values.
// This means that changing the gain has no effect on the threshold
const float F1Thresh      = 0.5e3;
const float F1Gain        = 7.5e-6;//5e-6;
const float F2Thresh      = 1e3;
const float F2Gain        = 2e-6;//1.4e-6;
const float F3Thresh      = 0.2e4;
const float F3Gain        = 1e-5;//7e-6;
const float F4Thresh      = 1e3;
const float F4Gain        = 10e-5;
const float F5Thresh      = 1e3;
const float F5Gain        = 10e-5;
const float F6Thresh      = 2.5e3;
const float F6Gain        = 5e-5;
const float F7ThreshOpen  = 3e4;
const float F7ThreshClose = 1e4;
const float F7Gain        = 6e-6;

const float parametersM[15] = {F1Thresh,F1Gain,F2Thresh,F2Gain,F3Thresh,F3Gain,F4Thresh,F4Gain,F5Thresh,F5Gain,F6Thresh,F6Gain,F7ThreshClose,F7ThreshOpen,F7Gain};

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

#endif //VBM_GLOBAL_VARIABLES_CONSTANTS_H
