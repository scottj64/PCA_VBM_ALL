//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: GetForces_emxutil.h
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 31-May-2017 16:57:50
//
#ifndef GETFORCES_EMXUTIL_H
#define GETFORCES_EMXUTIL_H

// Include Files
#include <cmath>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rtwtypes.h"
#include "GetForces_types.h"

// Function Declarations
extern void emxEnsureCapacity(emxArray__common *emxArray, int oldNumel, int
  elementSize);
extern void emxFree_real_T(emxArray_real_T **pEmxArray);
extern void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions);

#endif

//
// File trailer for GetForces_emxutil.h
//
// [EOF]
//
