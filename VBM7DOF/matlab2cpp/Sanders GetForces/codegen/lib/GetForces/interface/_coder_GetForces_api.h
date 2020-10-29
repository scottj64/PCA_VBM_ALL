/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_GetForces_api.h
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 31-May-2017 16:57:50
 */

#ifndef _CODER_GETFORCES_API_H
#define _CODER_GETFORCES_API_H

/* Include Files */
#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"
#include <stddef.h>
#include <stdlib.h>
#include "_coder_GetForces_api.h"

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

/* Function Declarations */
extern void GetForces(real_T IMUEulerOutput[15], real_T BIMUnode[15], real_T
                      NumofBodyIMUs, real_T p[849], real_T GMapping[210], real_T
                      Fest_prev[7], real_T parametersM[15], real_T Fest[7]);
extern void GetForces_api(const mxArray *prhs[7], const mxArray *plhs[1]);
extern void GetForces_atexit(void);
extern void GetForces_initialize(void);
extern void GetForces_terminate(void);
extern void GetForces_xil_terminate(void);

#endif

/*
 * File trailer for _coder_GetForces_api.h
 *
 * [EOF]
 */
