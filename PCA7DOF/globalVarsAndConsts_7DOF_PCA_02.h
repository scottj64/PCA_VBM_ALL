#ifndef PCA_GLOBAL_VARIABLES_CONSTANTS_H
#define PCA_GLOBAL_VARIABLES_CONSTANTS_H

#include "../SharedFiles/globalVarsAndConsts_7DOF_Shared_01.h"

int totalNumPCAVectors = 7;
const int lengthOfPCAVectors = NUMBER_OF_IMUS*3; //This has to be this value unless we ignore yaw

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

#endif //PCA_GLOBAL_VARIABLES_CONSTANTS_H
