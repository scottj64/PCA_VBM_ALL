#ifndef MATLAB_THREAD_H
#define MATLAB_THREAD_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
//#include "/usr/local/MATLAB/R2016b/extern/include/engine.h"

#include "../SharedFiles/globalVarsAndConsts_7DOF_Shared_01.h"
//#include "globalVarsAndConsts_7DOF_VBM_02.h"

//#include <sys/time.h>


bool matlabFunction(dataPoint * currDP_, int BIMUnode[/*5*/][3], float p[/*283*/][3], float GMapping[/*7*/][30])
{
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

	const float parametersM[15] = {F1Thresh, F1Gain, F2Thresh, F2Gain, F3Thresh, F3Gain, F4Thresh, F4Gain, F5Thresh, F5Gain, F6Thresh, F6Gain, F7ThreshClose, F7ThreshOpen, F7Gain};

	
	
	
	
	
	
	
	//GetForces(currDP_->IMU_values, BIMUnode, NUMBER_OF_IMUS, p, GMapping, Fest_prev, parametersM, Fest);
//	short int k; // over EulerAngles
	short int n; // over IMUs
	short int i1; 
	short int i2; 
	
	float cos_roll, sin_roll, cos_yaw, sin_yaw, cos_pitch, sin_pitch;
	
	float rotation_Matrices[5][3][3];
	
	float Error[30]; //for GMapping

	float v[3]; // vectors between nodes
	float w[3];
	
	float norm_v;
	float norm_w;
	
	float Fin[7];
	
	bool smallEnough = true;
	
	// 'GetForces:3' if all(abs(currDP_->IMU_values)<=1.2)
	for (short int k = 0; k < 3*NUMBER_OF_IMUS; k++)
	{
		if( std::abs(currDP_->IMU_values[k]) >= 1.2)
		{
			printf("IMU reading %d too big\n",k);
			smallEnough = false;
			break;
		}
	}

	if (smallEnough)
	{
		// 'GetForces:8' BROT(:,:,1)=Euler2RotMatrix(currDP_->IMU_values(1,1:3),1);
		cos_roll  = std::cos(currDP_->IMU_values[0]);
		sin_roll  = std::sin(currDP_->IMU_values[0]);
		cos_yaw   = std::cos(currDP_->IMU_values[1]);
		sin_yaw   = std::sin(currDP_->IMU_values[1]);
		cos_pitch = std::cos(currDP_->IMU_values[2]);
		sin_pitch = std::sin(currDP_->IMU_values[2]);

		rotation_Matrices[0][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
		rotation_Matrices[0][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
		rotation_Matrices[0][0][2] = -cos_yaw * sin_pitch;
		rotation_Matrices[0][1][0] = -cos_yaw * sin_roll;
		rotation_Matrices[0][1][1] =  cos_roll * cos_yaw;
		rotation_Matrices[0][1][2] =  sin_yaw;
		rotation_Matrices[0][2][0] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
		rotation_Matrices[0][2][1] =  sin_pitch * sin_roll - cos_pitch * cos_roll * sin_yaw;
		rotation_Matrices[0][2][2] =  cos_pitch * cos_yaw;


		// 'GetForces:9' BROT(:,:,2)=Euler2RotMatrix(currDP_->IMU_values(1,4:6),1);
		cos_roll  = std::cos(currDP_->IMU_values[3]);
		sin_roll  = std::sin(currDP_->IMU_values[3]);
		cos_yaw   = std::cos(currDP_->IMU_values[4]);
		sin_yaw   = std::sin(currDP_->IMU_values[4]);
		cos_pitch = std::cos(currDP_->IMU_values[5]);
		sin_pitch = std::sin(currDP_->IMU_values[5]);

		rotation_Matrices[1][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
		rotation_Matrices[1][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
		rotation_Matrices[1][0][2] = -cos_yaw * sin_pitch;
		rotation_Matrices[1][1][0] = -cos_yaw * sin_roll;
		rotation_Matrices[1][1][1] =  cos_roll * cos_yaw;
		rotation_Matrices[1][1][2] =  sin_yaw;
		rotation_Matrices[1][2][0] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
		rotation_Matrices[1][2][1] =  sin_pitch * sin_roll - cos_pitch * cos_roll * sin_yaw;
		rotation_Matrices[1][2][2] =  cos_pitch * cos_yaw;


		// 'GetForces:10' BROT(:,:,3)=Euler2RotMatrix(currDP_->IMU_values(1,7:9),1);
		cos_roll  = std::cos(currDP_->IMU_values[6]);
		sin_roll  = std::sin(currDP_->IMU_values[6]);
		cos_yaw   = std::cos(currDP_->IMU_values[7]);
		sin_yaw   = std::sin(currDP_->IMU_values[7]);
		cos_pitch = std::cos(currDP_->IMU_values[8]);
		sin_pitch = std::sin(currDP_->IMU_values[8]);


		rotation_Matrices[2][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
		rotation_Matrices[2][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
		rotation_Matrices[2][0][2] = -cos_yaw * sin_pitch;
		rotation_Matrices[2][1][0] = -cos_yaw * sin_roll;
		rotation_Matrices[2][1][1] =  cos_roll * cos_yaw;
		rotation_Matrices[2][1][2] =  sin_yaw;
		rotation_Matrices[2][2][0] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
		rotation_Matrices[2][2][1] =  sin_pitch * sin_roll - cos_pitch * cos_roll * sin_yaw;
		rotation_Matrices[2][2][2] =  cos_pitch * cos_yaw;


		// 'GetForces:11' BROT(:,:,4)=Euler2RotMatrix(currDP_->IMU_values(1,10:12),5);
		cos_roll  = std::cos(currDP_->IMU_values[9]);
		sin_roll  = std::sin(currDP_->IMU_values[9]);
		cos_yaw   = std::cos(currDP_->IMU_values[10]);
		sin_yaw   = std::sin(currDP_->IMU_values[10]);
		cos_pitch = std::cos(currDP_->IMU_values[11]);
		sin_pitch = std::sin(currDP_->IMU_values[11]);

		rotation_Matrices[3][0][0] =  cos_pitch * cos_yaw;
		rotation_Matrices[3][0][1] =  cos_pitch * cos_roll * sin_yaw - sin_pitch * sin_roll;
		rotation_Matrices[3][0][2] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
		rotation_Matrices[3][1][0] = -sin_yaw;
		rotation_Matrices[3][1][1] =  cos_roll * cos_yaw;
		rotation_Matrices[3][1][2] =  cos_yaw * sin_roll;
		rotation_Matrices[3][2][0] = -cos_yaw * sin_pitch;
		rotation_Matrices[3][2][1] = -cos_pitch * sin_roll - cos_roll * sin_pitch * sin_yaw;
		rotation_Matrices[3][2][2] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;


		// 'GetForces:12' BROT(:,:,5)=Euler2RotMatrix(currDP_->IMU_values(1,13:15),2);
		cos_roll  = std::cos(currDP_->IMU_values[12]);
		sin_roll  = std::sin(currDP_->IMU_values[12]);
		cos_yaw   = std::cos(currDP_->IMU_values[13]);
		sin_yaw   = std::sin(currDP_->IMU_values[13]);
		cos_pitch = std::cos(currDP_->IMU_values[14]);
		sin_pitch = std::sin(currDP_->IMU_values[14]);

		rotation_Matrices[4][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
		rotation_Matrices[4][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
		rotation_Matrices[4][0][2] =  cos_yaw * sin_pitch;
		rotation_Matrices[4][1][0] = -cos_yaw * sin_roll;
		rotation_Matrices[4][1][1] =  cos_roll * cos_yaw;
		rotation_Matrices[4][1][2] = -sin_yaw;
		rotation_Matrices[4][2][0] = -cos_roll * sin_pitch - cos_pitch * sin_roll * sin_yaw;
		rotation_Matrices[4][2][1] = -sin_pitch * sin_roll + cos_pitch * cos_roll * sin_yaw;
		rotation_Matrices[4][2][2] =  cos_pitch * cos_yaw;
/*
		rotation_Matrices[4][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
		rotation_Matrices[4][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
		rotation_Matrices[4][0][2] = -cos_yaw * sin_pitch;
		rotation_Matrices[4][1][0] = -cos_yaw * sin_roll;
		rotation_Matrices[4][1][1] =  cos_roll * cos_yaw;
		rotation_Matrices[4][1][2] =  sin_yaw;
		rotation_Matrices[4][2][0] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
		rotation_Matrices[4][2][1] =  sin_pitch * sin_roll - cos_pitch * cos_roll * sin_yaw;
		rotation_Matrices[4][2][2] =  cos_pitch * cos_yaw;	
*/
		// Body Bending Computation
		// calculate set of estimated body forces
		// deformed vectors
		memset(&Error[0], 0, 30 * sizeof(float)); //Why the U?

		// 'GetForces:18' for n=1:NUMBER_OF_IMUS
		for (short int n = 0; n < NUMBER_OF_IMUS; n++)
		{
			// 'GetForces:19' Error((n-1)*6+1:n*6,1) = ComputeDeformedVectors(BIMUnode(1,n),BIMUnode(2,n),BIMUnode(3,n),BROT(:,:,n),p); 
	
			v[0] = p[ BIMUnode[n][1]-1 ][0] - p[ BIMUnode[n][0]-1 ][0];
			v[1] = p[ BIMUnode[n][1]-1 ][1] - p[ BIMUnode[n][0]-1 ][1];
			v[2] = p[ BIMUnode[n][1]-1 ][2] - p[ BIMUnode[n][0]-1 ][2];
	
			w[0] = p[ BIMUnode[n][2]-1 ][0] - p[ BIMUnode[n][0]-1 ][0];
			w[1] = p[ BIMUnode[n][2]-1 ][1] - p[ BIMUnode[n][0]-1 ][1];
			w[2] = p[ BIMUnode[n][2]-1 ][2] - p[ BIMUnode[n][0]-1 ][2];
	
			norm_v = std::sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
			norm_w = std::sqrt( w[0]*w[0] + w[1]*w[1] + w[2]*w[2] );
	
			v[0] /= norm_v;
			v[1] /= norm_v;
			v[2] /= norm_v;
	
			w[0] /= norm_w;
			w[1] /= norm_w;
			w[2] /= norm_w;
		
		
			// Calculate deformed vectors based on rotation matrices
			// solve for set of forces iternatively
			
			//rotation_Matrices*v
/*				for (i1=0; i1<3; i1++)
			{
				for (i2=0; i2<3; i2++)
				{
					Error[n * 6 + i1] += rotation_Matrices[n][i1][i2] * v[i2];
				}
			}
	
			//rotation_Matrices*w
			for (i1=0; i1<3; i1++)
			{
				for (i2=0; i2<3; i2++)
				{
					Error[n * 6 + i1 + 3] += rotation_Matrices[n][i1][i2] * w[i2];
				}
			}*/
			//rotation_Matrices*w
			for (i1=0; i1<3; i1++)
			{
				for (i2=0; i2<3; i2++)
				{
					//rotation_Matrices*v
					Error[n * 6 + i1] += rotation_Matrices[n][i1][i2] * v[i2];
					//rotation_Matrices*w
					Error[n * 6 + i1 + 3] += rotation_Matrices[n][i1][i2] * w[i2];
				}
			}
		}


		// Use Ordinary Least Squares to estimate the set of virtual forces

		memset(&Fin[0], 0, 7 * sizeof(float));
		for (short int i1 = 0; i1 < 7; i1++)
		{
			for (short i2 = 0; i2 < 30; i2++)
			{
				Fin[i1] += GMapping[i1][i2] * Error[i2];
			}
		}

		// send to scaling
		if (Fin[0] > parametersM[0])
		{
			Fin[0] -= parametersM[0];
		}
		else if (Fin[0] < -parametersM[0])
		{
			Fin[0] += parametersM[0];
		}
		else
		{
			Fin[0] = 0.0;
		}

		if (Fin[1] > parametersM[2])
		{
			Fin[1] -= parametersM[2];
		}
		else if (Fin[1] < -parametersM[2])
		{
			Fin[1] += parametersM[2];
		}
		else
		{
			Fin[1] = 0.0;
		}

		if (Fin[2] > parametersM[4])
		{
			Fin[2] -= parametersM[4];
		}
		else if (Fin[2] < -parametersM[4])
		{
			Fin[2] += parametersM[4];
		}
		else
		{
			Fin[2] = 0.0;
		}

		if (Fin[3] > parametersM[6])
		{
			Fin[3] -= parametersM[6];
		}
		else if (Fin[3] < -parametersM[6])
		{
			Fin[3] += parametersM[6];
		}
		else
		{
			Fin[3] = 0.0;
		}

		if (Fin[4] > parametersM[8])
		{
			Fin[4] -= parametersM[8];
		}
		else if (Fin[4] < -parametersM[8])
		{
			Fin[4] += parametersM[8];
		}
		else
		{
			Fin[4] = 0.0;
		}

		if (Fin[5] > parametersM[10])
		{
			Fin[5] -= parametersM[10];
		}
		else if (Fin[5] < -parametersM[10])
		{
			Fin[5] += parametersM[10];
		}
		else
		{
			Fin[5] = 0.0;
		}

		if (Fin[6] > parametersM[12])
		{
			Fin[6] -= parametersM[12];
		}
		else if (Fin[6] < -parametersM[13])
		{
			Fin[6] += parametersM[13];
		}
		else
		{
			Fin[6] = 0.0;
		}

		currDP_->velocityYraw	= Fin[0] *  parametersM[1];
		currDP_->velocityXraw	= Fin[1] * -parametersM[3];
		currDP_->velocityZraw	= Fin[2] *  parametersM[5];
		currDP_->thetaY			= Fin[3] * -parametersM[7];
		currDP_->thetaZ			= Fin[4] * -parametersM[9];
		currDP_->thetaX			= Fin[5] * -parametersM[11];
		currDP_->grasp			= Fin[6] *  parametersM[14];

		
//		currDP.velocityXraw = forces.P1;
//		currDP.velocityYraw = forces.P2;
//		currDP.velocityZraw = forces.P3;
//		currDP.thetaX		= forces.P5;
//		currDP.thetaY		= forces.P6;
//		currDP.thetaZ		= forces.P4;
//		currDP.grasp		= forces.P7;
		

	} //end of if smallEnough
	else
	{
		//expand this to incorporate additional forces/torques -> velocities, twists
		currDP_->velocityXraw = 
		currDP_->velocityYraw = 
		currDP_->velocityZraw = 
		currDP_->thetaX		= 
		currDP_->thetaY		= 
		currDP_->thetaZ		= 
		currDP_->grasp		= 0.0;
		return false;
	}//end of else smallEnough

}
//return EXIT_SUCCESS;

#endif //MATLAB_THREAD_H
