//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: GetForces.cpp
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 31-May-2017 16:57:50
//

// Include Files
#include "rt_nonfinite.h"
#include "GetForces.h"
#include "norm.h"
#include "GetForces_emxutil.h"

// Function Definitions

//
// check orientation condition
// Arguments    : const double IMUEulerOutput[15]
//                const double BIMUnode[15]
//                double NumofBodyIMUs
//                const double p[849]
//                const double GMapping[210]
//                const double Fest_prev[7]
//                const double parametersM[15]
//                double Fest[7]
// Return Type  : void
//
void GetForces(const double IMUEulerOutput[15], const double BIMUnode[15],
               double NumofBodyIMUs, const double p[849], const double GMapping
               [210], const double Fest_prev[7], const double parametersM[15],
               double Fest[7])
{
  double y[15];
  int k;
  boolean_T b_y;
  boolean_T exitg1;
  emxArray_real_T *BROT;
  int i0;
  double dv0[9];
  double dv1[9];
  static const signed char iv0[3] = { 0, 1, 0 };

  static const signed char iv1[3] = { 1, 0, 0 };

  double dv2[9];
  double dv3[9];
  int i1;
  static const signed char iv2[3] = { 0, 0, 1 };

  int i2;
  double Error[30];
  int n;
  double c_y;
  double d_y;
  signed char tmp_data[30];
  double Fin[7];
  double v[3];
  double w[3];
  double b_v[3];
  double b_w[3];
  double b_BROT[6];
  for (k = 0; k < 15; k++) {
    y[k] = std::abs(IMUEulerOutput[k]);
  }

  b_y = true;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 15)) {
    if (!(y[k] <= 1.2)) {
      b_y = false;
      exitg1 = true;
    } else {
      k++;
    }
  }

  if (b_y) {
    emxInit_real_T(&BROT, 3);

    //  Data Processing
    // Convert to FEM orientation
    i0 = BROT->size[0] * BROT->size[1] * BROT->size[2];
    BROT->size[0] = 3;
    BROT->size[1] = 3;
    BROT->size[2] = (int)NumofBodyIMUs;
    emxEnsureCapacity((emxArray__common *)BROT, i0, (int)sizeof(double));
    k = 9 * (int)NumofBodyIMUs;
    for (i0 = 0; i0 < k; i0++) {
      BROT->data[i0] = 0.0;
    }

    // rotation about y-axis
    // rotation about x-axis
    // rotation about z-axis
    // Pitch, row, yaw order
    dv0[0] = std::cos(-IMUEulerOutput[2]);
    dv0[3] = 0.0;
    dv0[6] = std::sin(-IMUEulerOutput[2]);
    dv0[2] = -std::sin(-IMUEulerOutput[2]);
    dv0[5] = 0.0;
    dv0[8] = std::cos(-IMUEulerOutput[2]);
    for (i0 = 0; i0 < 3; i0++) {
      dv0[1 + 3 * i0] = iv0[i0];
      dv1[3 * i0] = iv1[i0];
    }

    dv1[1] = 0.0;
    dv1[4] = std::cos(-IMUEulerOutput[1]);
    dv1[7] = -std::sin(-IMUEulerOutput[1]);
    dv1[2] = 0.0;
    dv1[5] = std::sin(-IMUEulerOutput[1]);
    dv1[8] = std::cos(-IMUEulerOutput[1]);
    dv3[0] = std::cos(-IMUEulerOutput[0]);
    dv3[3] = -std::sin(-IMUEulerOutput[0]);
    dv3[6] = 0.0;
    dv3[1] = std::sin(-IMUEulerOutput[0]);
    dv3[4] = std::cos(-IMUEulerOutput[0]);
    dv3[7] = 0.0;
    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        dv2[i0 + 3 * i1] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          dv2[i0 + 3 * i1] += dv0[i0 + 3 * i2] * dv1[i2 + 3 * i1];
        }
      }

      dv3[2 + 3 * i0] = iv2[i0];
    }

    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        BROT->data[i0 + BROT->size[0] * i1] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          BROT->data[i0 + BROT->size[0] * i1] += dv2[i0 + 3 * i2] * dv3[i2 + 3 *
            i1];
        }
      }
    }

    // rotation about y-axis
    // rotation about x-axis
    // rotation about z-axis
    // Pitch, row, yaw order
    dv0[0] = std::cos(-IMUEulerOutput[5]);
    dv0[3] = 0.0;
    dv0[6] = std::sin(-IMUEulerOutput[5]);
    dv0[2] = -std::sin(-IMUEulerOutput[5]);
    dv0[5] = 0.0;
    dv0[8] = std::cos(-IMUEulerOutput[5]);
    for (i0 = 0; i0 < 3; i0++) {
      dv0[1 + 3 * i0] = iv0[i0];
      dv1[3 * i0] = iv1[i0];
    }

    dv1[1] = 0.0;
    dv1[4] = std::cos(-IMUEulerOutput[4]);
    dv1[7] = -std::sin(-IMUEulerOutput[4]);
    dv1[2] = 0.0;
    dv1[5] = std::sin(-IMUEulerOutput[4]);
    dv1[8] = std::cos(-IMUEulerOutput[4]);
    dv3[0] = std::cos(-IMUEulerOutput[3]);
    dv3[3] = -std::sin(-IMUEulerOutput[3]);
    dv3[6] = 0.0;
    dv3[1] = std::sin(-IMUEulerOutput[3]);
    dv3[4] = std::cos(-IMUEulerOutput[3]);
    dv3[7] = 0.0;
    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        dv2[i0 + 3 * i1] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          dv2[i0 + 3 * i1] += dv0[i0 + 3 * i2] * dv1[i2 + 3 * i1];
        }
      }

      dv3[2 + 3 * i0] = iv2[i0];
    }

    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        BROT->data[(i0 + BROT->size[0] * i1) + BROT->size[0] * BROT->size[1]] =
          0.0;
        for (i2 = 0; i2 < 3; i2++) {
          BROT->data[(i0 + BROT->size[0] * i1) + BROT->size[0] * BROT->size[1]] +=
            dv2[i0 + 3 * i2] * dv3[i2 + 3 * i1];
        }
      }
    }

    // rotation about y-axis
    // rotation about x-axis
    // rotation about z-axis
    // Pitch, row, yaw order
    dv0[0] = std::cos(-IMUEulerOutput[8]);
    dv0[3] = 0.0;
    dv0[6] = std::sin(-IMUEulerOutput[8]);
    dv0[2] = -std::sin(-IMUEulerOutput[8]);
    dv0[5] = 0.0;
    dv0[8] = std::cos(-IMUEulerOutput[8]);
    for (i0 = 0; i0 < 3; i0++) {
      dv0[1 + 3 * i0] = iv0[i0];
      dv1[3 * i0] = iv1[i0];
    }

    dv1[1] = 0.0;
    dv1[4] = std::cos(-IMUEulerOutput[7]);
    dv1[7] = -std::sin(-IMUEulerOutput[7]);
    dv1[2] = 0.0;
    dv1[5] = std::sin(-IMUEulerOutput[7]);
    dv1[8] = std::cos(-IMUEulerOutput[7]);
    dv3[0] = std::cos(-IMUEulerOutput[6]);
    dv3[3] = -std::sin(-IMUEulerOutput[6]);
    dv3[6] = 0.0;
    dv3[1] = std::sin(-IMUEulerOutput[6]);
    dv3[4] = std::cos(-IMUEulerOutput[6]);
    dv3[7] = 0.0;
    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        dv2[i0 + 3 * i1] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          dv2[i0 + 3 * i1] += dv0[i0 + 3 * i2] * dv1[i2 + 3 * i1];
        }
      }

      dv3[2 + 3 * i0] = iv2[i0];
    }

    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        BROT->data[(i0 + BROT->size[0] * i1) + (BROT->size[0] * BROT->size[1] <<
          1)] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          BROT->data[(i0 + BROT->size[0] * i1) + (BROT->size[0] * BROT->size[1] <<
            1)] += dv2[i0 + 3 * i2] * dv3[i2 + 3 * i1];
        }
      }
    }

    // rotation about y-axis
    // rotation about z-axis
    // rotation about x-axis
    // Pitch, row, yaw order
    dv0[0] = std::cos(IMUEulerOutput[11]);
    dv0[3] = 0.0;
    dv0[6] = std::sin(IMUEulerOutput[11]);
    dv0[2] = -std::sin(IMUEulerOutput[11]);
    dv0[5] = 0.0;
    dv0[8] = std::cos(IMUEulerOutput[11]);
    dv1[0] = std::cos(-IMUEulerOutput[10]);
    dv1[3] = -std::sin(-IMUEulerOutput[10]);
    dv1[6] = 0.0;
    dv1[1] = std::sin(-IMUEulerOutput[10]);
    dv1[4] = std::cos(-IMUEulerOutput[10]);
    dv1[7] = 0.0;
    for (i0 = 0; i0 < 3; i0++) {
      dv0[1 + 3 * i0] = iv0[i0];
      dv1[2 + 3 * i0] = iv2[i0];
    }

    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        dv2[i0 + 3 * i1] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          dv2[i0 + 3 * i1] += dv0[i0 + 3 * i2] * dv1[i2 + 3 * i1];
        }
      }

      dv3[3 * i0] = iv1[i0];
    }

    dv3[1] = 0.0;
    dv3[4] = std::cos(-IMUEulerOutput[9]);
    dv3[7] = -std::sin(-IMUEulerOutput[9]);
    dv3[2] = 0.0;
    dv3[5] = std::sin(-IMUEulerOutput[9]);
    dv3[8] = std::cos(-IMUEulerOutput[9]);
    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        BROT->data[(i0 + BROT->size[0] * i1) + BROT->size[0] * BROT->size[1] * 3]
          = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          BROT->data[(i0 + BROT->size[0] * i1) + BROT->size[0] * BROT->size[1] *
            3] += dv2[i0 + 3 * i2] * dv3[i2 + 3 * i1];
        }
      }
    }

    // rotation about y-axis
    // rotation about x-axis
    // rotation about z-axis
    // Pitch, row, yaw order
    dv0[0] = std::cos(IMUEulerOutput[14]);
    dv0[3] = 0.0;
    dv0[6] = std::sin(IMUEulerOutput[14]);
    dv0[2] = -std::sin(IMUEulerOutput[14]);
    dv0[5] = 0.0;
    dv0[8] = std::cos(IMUEulerOutput[14]);
    for (i0 = 0; i0 < 3; i0++) {
      dv0[1 + 3 * i0] = iv0[i0];
      dv1[3 * i0] = iv1[i0];
    }

    dv1[1] = 0.0;
    dv1[4] = std::cos(IMUEulerOutput[13]);
    dv1[7] = -std::sin(IMUEulerOutput[13]);
    dv1[2] = 0.0;
    dv1[5] = std::sin(IMUEulerOutput[13]);
    dv1[8] = std::cos(IMUEulerOutput[13]);
    dv3[0] = std::cos(-IMUEulerOutput[12]);
    dv3[3] = -std::sin(-IMUEulerOutput[12]);
    dv3[6] = 0.0;
    dv3[1] = std::sin(-IMUEulerOutput[12]);
    dv3[4] = std::cos(-IMUEulerOutput[12]);
    dv3[7] = 0.0;
    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        dv2[i0 + 3 * i1] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          dv2[i0 + 3 * i1] += dv0[i0 + 3 * i2] * dv1[i2 + 3 * i1];
        }
      }

      dv3[2 + 3 * i0] = iv2[i0];
    }

    for (i0 = 0; i0 < 3; i0++) {
      for (i1 = 0; i1 < 3; i1++) {
        BROT->data[(i0 + BROT->size[0] * i1) + (BROT->size[0] * BROT->size[1] <<
          2)] = 0.0;
        for (i2 = 0; i2 < 3; i2++) {
          BROT->data[(i0 + BROT->size[0] * i1) + (BROT->size[0] * BROT->size[1] <<
            2)] += dv2[i0 + 3 * i2] * dv3[i2 + 3 * i1];
        }
      }
    }

    //      boolean=1;
    //      zd_hat=zeros(NumofBodyIMUs*6,1);
    //     %% Body Bending Computation
    // calculate set of estimated body forces
    memset(&Error[0], 0, 30U * sizeof(double));
    for (n = 0; n < (int)NumofBodyIMUs; n++) {
      c_y = ((1.0 + (double)n) - 1.0) * 6.0 + 1.0;
      d_y = (1.0 + (double)n) * 6.0;
      if (c_y > d_y) {
        i0 = 1;
        i1 = 0;
      } else {
        i0 = (int)c_y;
        i1 = (int)d_y;
      }

      k = (signed char)((signed char)i1 - (signed char)i0);
      for (i2 = 0; i2 <= k; i2++) {
        tmp_data[i2] = (signed char)((signed char)((signed char)i0 + i2) - 1);
      }

      // initialization
      for (i2 = 0; i2 < 3; i2++) {
        v[i2] = p[((int)BIMUnode[1 + 3 * n] + 283 * i2) - 1] - p[((int)BIMUnode
          [3 * n] + 283 * i2) - 1];
      }

      c_y = norm(v);
      for (i2 = 0; i2 < 3; i2++) {
        w[i2] = p[((int)BIMUnode[2 + 3 * n] + 283 * i2) - 1] - p[((int)BIMUnode
          [3 * n] + 283 * i2) - 1];
      }

      d_y = norm(w);

      // Calculate deformed vectors based on rotation matrices
      // solve for set of forces iternatively
      for (i2 = 0; i2 < 3; i2++) {
        b_v[i2] = v[i2] / c_y;
        b_w[i2] = w[i2] / d_y;
      }

      for (i2 = 0; i2 < 3; i2++) {
        w[i2] = 0.0;
        for (k = 0; k < 3; k++) {
          c_y = w[i2] + BROT->data[(i2 + BROT->size[0] * k) + BROT->size[0] *
            BROT->size[1] * n] * b_v[k];
          w[i2] = c_y;
        }
      }

      for (i2 = 0; i2 < 3; i2++) {
        v[i2] = 0.0;
        for (k = 0; k < 3; k++) {
          c_y = v[i2] + BROT->data[(i2 + BROT->size[0] * k) + BROT->size[0] *
            BROT->size[1] * n] * b_w[k];
          v[i2] = c_y;
        }
      }

      for (i2 = 0; i2 < 3; i2++) {
        b_BROT[i2] = w[i2];
        b_BROT[i2 + 3] = v[i2];
      }

      k = (signed char)((signed char)i1 - (signed char)i0) + 1;
      for (i0 = 0; i0 < k; i0++) {
        Error[tmp_data[i0]] = b_BROT[i0];
      }
    }

    emxFree_real_T(&BROT);

    // deformed vectors
    //   Error = zeros(30,1);
    //   for i=0:4
    //       Error(i*6+1:(i+1)*6,1) = zd_hat{i+1,1};
    //   end
    //       Error=zd_hat;
    // Use Ordinary Least Squares to estimate the set of virtual forces
    for (i0 = 0; i0 < 7; i0++) {
      Fin[i0] = 0.0;
      for (i1 = 0; i1 < 30; i1++) {
        Fin[i0] += GMapping[i0 + 7 * i1] * Error[i1];
      }
    }

    // send to scaling
    //      Fest=PostCalc(F,parametersM);
    if (Fin[0] > parametersM[0]) {
      Fin[0] -= parametersM[0];
    } else if (Fin[0] < -parametersM[0]) {
      Fin[0] += parametersM[0];
    } else {
      Fin[0] = 0.0;
    }

    if (Fin[1] > parametersM[2]) {
      Fin[1] -= parametersM[2];
    } else if (Fin[1] < -parametersM[2]) {
      Fin[1] += parametersM[2];
    } else {
      Fin[1] = 0.0;
    }

    if (Fin[2] > parametersM[4]) {
      Fin[2] -= parametersM[4];
    } else if (Fin[2] < -parametersM[4]) {
      Fin[2] += parametersM[4];
    } else {
      Fin[2] = 0.0;
    }

    if (Fin[3] > parametersM[6]) {
      Fin[3] -= parametersM[6];
    } else if (Fin[3] < -parametersM[6]) {
      Fin[3] += parametersM[6];
    } else {
      Fin[3] = 0.0;
    }

    if (Fin[4] > parametersM[8]) {
      Fin[4] -= parametersM[8];
    } else if (Fin[4] < -parametersM[8]) {
      Fin[4] += parametersM[8];
    } else {
      Fin[4] = 0.0;
    }

    if (Fin[5] > parametersM[10]) {
      Fin[5] -= parametersM[10];
    } else if (Fin[5] < -parametersM[10]) {
      Fin[5] += parametersM[10];
    } else {
      Fin[5] = 0.0;
    }

    if (Fin[6] > parametersM[12]) {
      Fin[6] -= parametersM[12];
    } else if (Fin[6] < -parametersM[13]) {
      Fin[6] += parametersM[13];
    } else {
      Fin[6] = 0.0;
    }

    Fest[0] = Fin[0] * parametersM[1];
    Fest[1] = Fin[1] * parametersM[3];
    Fest[2] = Fin[2] * parametersM[5];
    Fest[3] = Fin[3] * parametersM[7];
    Fest[4] = Fin[4] * parametersM[9];
    Fest[5] = Fin[5] * parametersM[11];
    Fest[6] = Fin[6] * parametersM[14];
  } else {
    for (i0 = 0; i0 < 7; i0++) {
      Fest[i0] = Fest_prev[i0];
    }

    //      boolean=0;
  }
}

//
// File trailer for GetForces.cpp
//
// [EOF]
//
