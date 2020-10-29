#ifndef MATLAB_THREAD_H
#define MATLAB_THREAD_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
//#include "/usr/local/MATLAB/R2016b/extern/include/engine.h"

#include "../SharedFiles/globalVarsAndConsts_7DOF_Shared_01.h"
#include "globalVarsAndConsts_7DOF_VBM_02.h"

//#include <sys/time.h>


void *matlabFunction(void*)
{
	printf("Matlab locking matlab.\n");
	pthread_mutex_lock( &Matlab_initialized_mutex );
	printf("Matlab locked matlab.\n");
	
	
//	float 		IMUEulerOutput[15] = {-0.01086, -0.450859, -0.119547, 0.029571, -0.522158, -0.272827, 0.204297, -0.38194, 0.082748, 0.012609, -0.158873, 0.019847, -0.987821, 0.152327, 0.031722};
	float 		IMUEulerOutput[15];
	int 		BIMUnode[5][3];
//	short int 	NUMBER_OF_IMUS = 5;
	float 		p[283][3];
	float 		GMapping[7][30];
//	float 		Fest_prev[7] = { 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0}; 
//	float 		parametersM[15] = {2500, 2.50E-05, 3000, 1.40E-05, 7500, 7.00E-06, 1000, 2.00E-04, 1000, 2.00E-04, 1000, 2.00E-04, 10000, 26500, 6.00E-06};
//	float 		parametersM[15]
	float 		Fest[7] = { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0};
	
	short int col;
	short int row;
	
	
	std::ifstream infile("VBM7DOF/matlab2cpp/GMapping.txt");
	if (infile.fail())
	{
		keepGoing = false;
		printf("Failed to open \"VBM7DOF/matlab2cpp/GMapping.txt\". Do not continue!\n");
	}
	for (row = 0; row <7; row ++)
	{
		for (col = 0; col <30; col ++)
		{	
			infile >> GMapping[row][col];
		}
	}
	infile.close();

	
	infile.open("VBM7DOF/matlab2cpp/p.txt", std::ifstream::in);
	if (infile.fail())
	{
		keepGoing = false;
		printf("Failed to open \"VBM7DOF/matlab2cpp/p.txt\". Do not continue!\n");
	}
	for (row = 0; row <283; row ++)
	{
		for (col = 0; col <3; col ++)
		{	
			infile >> p[row][col];
		}
	}
	infile.close();

	
	infile.open("VBM7DOF/matlab2cpp/BIMUnode.txt", std::ifstream::in);
	if (infile.fail())
	{
		keepGoing = false;
		printf("Failed to open \"VBM7DOF/matlab2cpp/BIMUnode.txt\". Do not continue!\n");
	}
	for (col = 0; col <3; col ++)
	{
		for (row = 0; row <5; row ++)
		{	
			infile >> BIMUnode[row][col];
		}
	}
	infile.close();
	
		
	
	struct timeval startOfProgramTime, currentTime;
	//float programTime;
//	float programTime = 0.0;

	unsigned long int counter = 0;
	unsigned long int sleepTime = 0;
	
	
	
	
	//loop variables
	//iterators
	short int k; // over EulerAngles
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
	
	
	
	printf("Matlab unlocking matlab.\n");
	//The program needs to tell the main loop that it is ready to work
	pthread_mutex_unlock( &Matlab_initialized_mutex );
	printf("Matlab unlocked matlab.\n");
	
	
	printf("Matlab locking ready.\n");
	//The program needs to wait for a signal to say that it should start this calculation
	pthread_mutex_lock( &ready_to_loop_mutex );
	//thread_mutex_unlock( &ready_to_loop_mutex ); //is there any danger of leaving amutex that isn't going to be referenced again, locked? Memory loss?
	printf("Matlab locked ready.\n");
	
	
	
	
	usleep(250000);
	
	
	gettimeofday(&startOfProgramTime, NULL);
	
	while(keepGoing)
	{
		for (int i = 0; i < NUMBER_OF_IMUS; i++)
		{
			pthread_mutex_lock( &orient[i].mutex );
			
			IMUEulerOutput[i*3  ] = orient[i].roll;
			IMUEulerOutput[i*3+1] = orient[i].yaw;
			IMUEulerOutput[i*3+2] = orient[i].pitch;
	
			pthread_mutex_unlock( &orient[i].mutex );	
	
		}
		
		//GetForces(IMUEulerOutput, BIMUnode, NUMBER_OF_IMUS, p, GMapping, Fest_prev, parametersM, Fest);
		smallEnough = true;
		// 'GetForces:3' if all(abs(IMUEulerOutput)<=1.2)
		for (k = 0; k < 15; k++)
		{
			if( std::abs(IMUEulerOutput[k]) >= 1.2)
			{
/*				for (i1 = 0; i1 < 7; i1++) //this may be unnecesary if we simply don't change what the previous Fest is....
				{
					Fest[i1] = Fest_prev[i1];
				}
				return;	//remove need for if statement below 
						//might want to add a pause to not loop insanely fast ACTUALLY do that in the main program
				
*/				printf("IMU reading %d too big\n",k);
				smallEnough = false;
				break;
			}
		}

		if (smallEnough)
		{
			// 'GetForces:8' BROT(:,:,1)=Euler2RotMatrix(IMUEulerOutput(1,1:3),1);
			cos_roll  = std::cos(IMUEulerOutput[0]);
			sin_roll  = std::sin(IMUEulerOutput[0]);
			cos_yaw   = std::cos(IMUEulerOutput[1]);
			sin_yaw   = std::sin(IMUEulerOutput[1]);
			cos_pitch = std::cos(IMUEulerOutput[2]);
			sin_pitch = std::sin(IMUEulerOutput[2]);

			rotation_Matrices[0][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
			rotation_Matrices[0][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
			rotation_Matrices[0][0][2] = -cos_yaw * sin_pitch;
			rotation_Matrices[0][1][0] = -cos_yaw * sin_roll;
			rotation_Matrices[0][1][1] =  cos_roll * cos_yaw;
			rotation_Matrices[0][1][2] =  sin_yaw;
			rotation_Matrices[0][2][0] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
			rotation_Matrices[0][2][1] =  sin_pitch * sin_roll - cos_pitch * cos_roll * sin_yaw;
			rotation_Matrices[0][2][2] =  cos_pitch * cos_yaw;
	

			// 'GetForces:9' BROT(:,:,2)=Euler2RotMatrix(IMUEulerOutput(1,4:6),1);
			cos_roll  = std::cos(IMUEulerOutput[3]);
			sin_roll  = std::sin(IMUEulerOutput[3]);
			cos_yaw   = std::cos(IMUEulerOutput[4]);
			sin_yaw   = std::sin(IMUEulerOutput[4]);
			cos_pitch = std::cos(IMUEulerOutput[5]);
			sin_pitch = std::sin(IMUEulerOutput[5]);

			rotation_Matrices[1][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
			rotation_Matrices[1][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
			rotation_Matrices[1][0][2] = -cos_yaw * sin_pitch;
			rotation_Matrices[1][1][0] = -cos_yaw * sin_roll;
			rotation_Matrices[1][1][1] =  cos_roll * cos_yaw;
			rotation_Matrices[1][1][2] =  sin_yaw;
			rotation_Matrices[1][2][0] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
			rotation_Matrices[1][2][1] =  sin_pitch * sin_roll - cos_pitch * cos_roll * sin_yaw;
			rotation_Matrices[1][2][2] =  cos_pitch * cos_yaw;
	

			// 'GetForces:10' BROT(:,:,3)=Euler2RotMatrix(IMUEulerOutput(1,7:9),1);
			cos_roll  = std::cos(IMUEulerOutput[6]);
			sin_roll  = std::sin(IMUEulerOutput[6]);
			cos_yaw   = std::cos(IMUEulerOutput[7]);
			sin_yaw   = std::sin(IMUEulerOutput[7]);
			cos_pitch = std::cos(IMUEulerOutput[8]);
			sin_pitch = std::sin(IMUEulerOutput[8]);


			rotation_Matrices[2][0][0] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
			rotation_Matrices[2][0][1] =  cos_pitch * sin_roll + cos_roll * sin_pitch * sin_yaw;
			rotation_Matrices[2][0][2] = -cos_yaw * sin_pitch;
			rotation_Matrices[2][1][0] = -cos_yaw * sin_roll;
			rotation_Matrices[2][1][1] =  cos_roll * cos_yaw;
			rotation_Matrices[2][1][2] =  sin_yaw;
			rotation_Matrices[2][2][0] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
			rotation_Matrices[2][2][1] =  sin_pitch * sin_roll - cos_pitch * cos_roll * sin_yaw;
			rotation_Matrices[2][2][2] =  cos_pitch * cos_yaw;

	
			// 'GetForces:11' BROT(:,:,4)=Euler2RotMatrix(IMUEulerOutput(1,10:12),5);
			cos_roll  = std::cos(IMUEulerOutput[9]);
			sin_roll  = std::sin(IMUEulerOutput[9]);
			cos_yaw   = std::cos(IMUEulerOutput[10]);
			sin_yaw   = std::sin(IMUEulerOutput[10]);
			cos_pitch = std::cos(IMUEulerOutput[11]);
			sin_pitch = std::sin(IMUEulerOutput[11]);

			rotation_Matrices[3][0][0] =  cos_pitch * cos_yaw;
			rotation_Matrices[3][0][1] =  cos_pitch * cos_roll * sin_yaw - sin_pitch * sin_roll;
			rotation_Matrices[3][0][2] =  cos_roll * sin_pitch + cos_pitch * sin_roll * sin_yaw;
			rotation_Matrices[3][1][0] = -sin_yaw;
			rotation_Matrices[3][1][1] =  cos_roll * cos_yaw;
			rotation_Matrices[3][1][2] =  cos_yaw * sin_roll;
			rotation_Matrices[3][2][0] = -cos_yaw * sin_pitch;
			rotation_Matrices[3][2][1] = -cos_pitch * sin_roll - cos_roll * sin_pitch * sin_yaw;
			rotation_Matrices[3][2][2] =  cos_pitch * cos_roll - sin_pitch * sin_roll * sin_yaw;
	

			// 'GetForces:12' BROT(:,:,5)=Euler2RotMatrix(IMUEulerOutput(1,13:15),2);
			cos_roll  = std::cos(IMUEulerOutput[12]);
			sin_roll  = std::sin(IMUEulerOutput[12]);
			cos_yaw   = std::cos(IMUEulerOutput[13]);
			sin_yaw   = std::sin(IMUEulerOutput[13]);
			cos_pitch = std::cos(IMUEulerOutput[14]);
			sin_pitch = std::sin(IMUEulerOutput[14]);

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
			for (n = 0; n < NUMBER_OF_IMUS; n++)
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
			for (i1 = 0; i1 < 7; i1++)
			{
				for (i2 = 0; i2 < 30; i2++)
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

			Fest[0] = Fin[0] *  parametersM[1];
			Fest[1] = Fin[1] * -parametersM[3];
			Fest[2] = Fin[2] *  parametersM[5];
			Fest[3] = Fin[3] * -parametersM[7];
			Fest[4] = Fin[4] * -parametersM[9];
			Fest[5] = Fin[5] * -parametersM[11];
			Fest[6] = Fin[6] *  parametersM[14];
	

				
			//lock rawVelocity output
			pthread_mutex_lock( &forces.mutex );
	
			//expand this to incorporate additional forces/torques -> velocities, twists
			forces.P1 = Fest[1];
			forces.P2 = Fest[0];
			forces.P3 = Fest[2];
			forces.P4 = Fest[3];
			forces.P5 = Fest[4];
			forces.P6 = Fest[5];
			forces.P7 = Fest[6];
		
			//unlock rawVelocity output
			pthread_mutex_unlock( &forces.mutex );
		} //end of if smallEnough
		else
		{
			//lock rawVelocity output
			pthread_mutex_lock( &forces.mutex );
	
			//expand this to incorporate additional forces/torques -> velocities, twists
			forces.P1 = 0;
			forces.P2 = 0;
			forces.P3 = 0;
			forces.P4 = 0;
			forces.P5 = 0;
			forces.P6 = 0;
			forces.P7 = 0;
		
			//unlock rawVelocity output
			pthread_mutex_unlock( &forces.mutex );
		}//end of else smallEnough
			
		counter++;
		gettimeofday(&currentTime, NULL);
		
		sleepTime = (startOfProgramTime.tv_sec - currentTime.tv_sec)*1000000
				  + (startOfProgramTime.tv_usec - currentTime.tv_usec)
				  +	(1000000/MATLAB_FREQUENCY)*counter;
//		printf("Matlab sleeptime: %ld\n", sleepTime);
		
		if ( (sleepTime>0) && (sleepTime<1000000/MATLAB_FREQUENCY) )
		{
			usleep(	sleepTime );
		}
	}
	
	printf("Matlab Thread Closed\n");
}
//return EXIT_SUCCESS;

#endif //MATLAB_THREAD_H
