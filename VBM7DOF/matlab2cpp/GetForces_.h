//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: GetForces.h
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 01-Jun-2017 11:25:03
//
#ifndef GETFORCES_H
#define GETFORCES_H

// Include Files
#include <cmath>

//#include <stddef.h> //why?
//#include <stdlib.h> //why?
#include <string.h> //memset

//#include "rtwtypes.h"
//#include "GetForces_types.h"

// Function Declarations
extern void GetForces(const float IMUEulerOutput[15],	const int BIMUnode[5][3],
                      const short int NumofBodyIMUs,	const float p[283][3],
					  const float GMapping[7][30],		const float Fest_prev[7],
					  const float parametersM[15],		float Fest[7]);

#endif

//
// File trailer for GetForces.h
//
// [EOF]
//
