//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: GetForces.h
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 31-May-2017 16:57:50
//
#ifndef GETFORCES_H
#define GETFORCES_H

// Include Files
#include <cmath>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rtwtypes.h"
#include "GetForces_types.h"

// Function Declarations
extern void GetForces(const double IMUEulerOutput[15], const double BIMUnode[15],
                      double NumofBodyIMUs, const double p[849], const double
                      GMapping[210], const double Fest_prev[7], const double
                      parametersM[15], double Fest[7]);

#endif

//
// File trailer for GetForces.h
//
// [EOF]
//
