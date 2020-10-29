#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <vector>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/time.h>

const int TOTAL_POSSIBLE_DOFS = 7;

struct dataPoint
{
	//output
	float programTime		= 0.0; //since start of streaming
	float IMU_values[NUMBER_OF_IMUS*3]	= {0.0};
	
	float		currentLoc[3]	= {0.0,0.0,0.0};
	float		currentRot[3]	= {0.0,0.0,0.0};
	float		tipLEDLoc[3]	= {0.0,0.0,0.0};
//	short int	fingerVal[3]	= {0,0,0};
	short int	fingerSVal		= 0;
#ifdef VBM_PROGRAM	
	float velocityXraw		= 0.0;
	float velocityYraw		= 0.0;
	float velocityZraw		= 0.0;
	
	float thetaX			= 0.0;
	float thetaY			= 0.0;
	float thetaZ			= 0.0;
	
	float grasp				= 0.0;
	
	short int fingersTold	= 0;
#endif

#ifdef PCA_PROGRAM	
	float 		dotProduct[TOTAL_POSSIBLE_DOFS]	= {0.0};
	float 		sentValue[TOTAL_POSSIBLE_DOFS]	= {0.0};
#endif
	
	int goalSlot = -1;
	int occupiedSlot = -1;
	
	int goalIterator = 0;
	int goalNum = 0;
	bool wasJustOutsideGoal = false;
	bool againstBorder		= false;
//	short int cursorStatus = -1;
	struct timeval goalStartTime;
	struct timeval goalArrivalTime;
	
	
	
	
	
	
	
//	float programTime		= 0.0; //since start of streaming
	
//	float		currentLoc[3]	= {0.0,0.0,0.0};
//	float		currentRot[3]	= {0.0,0.0,0.0};
	
//	float		fingerSVal		= 0.0;
	
//	float		fingersTold		= 0.0;
	
	float		rawCartVel[3]	= {0.0,0.0,0.0};
	float		rawRotVel[3]	= {0.0,0.0,0.0};
	
	float		jointAngle[6]	= {0.0,0.0,0.0,0.0,0.0,0.0};
};
const float CenterX = 0.4;
const float CenterZ = 0.4;
const float fingerGain = 10.0;
const float StartY = 0.4;



void printDataPoint(const dataPoint *_DataPoint, std::ofstream &outputFile)
{
	int i;
	outputFile << 
		_DataPoint->programTime << "\t";
		
	for (i = 0; i<NUMBER_OF_IMUS*3; i++)
	{
		outputFile << _DataPoint->IMU_values[i] << "\t";
	}
	
	outputFile <<
		_DataPoint->currentLoc[0]	<< "\t" <<
		_DataPoint->currentLoc[1]	<< "\t" <<
		_DataPoint->currentLoc[2]	<< "\t" <<
		
		_DataPoint->currentRot[0]	<< "\t" <<
		_DataPoint->currentRot[1]	<< "\t" <<
		_DataPoint->currentRot[2]	<< "\t" <<
		
		_DataPoint->tipLEDLoc[0]	<< "\t" <<
		_DataPoint->tipLEDLoc[1]	<< "\t" <<
		_DataPoint->tipLEDLoc[2]	<< "\t" <<
		
		_DataPoint->fingerSVal		<< "\t" <<
		
		_DataPoint->goalIterator	<< "\t" <<
		
		_DataPoint->goalNum			<< "\t" <<
		
		_DataPoint->goalSlot		<< "\t" <<
		
		_DataPoint->occupiedSlot	<< "\t" <<
		
//		_DataPoint->cursorStatus	<< "\t" <<
		
		_DataPoint->againstBorder	<< "\t" <<
		
		
#ifdef VBM_PROGRAM
		_DataPoint->velocityXraw	<< "\t" <<
		_DataPoint->velocityYraw	<< "\t" <<
		_DataPoint->velocityZraw	<< "\t" <<
	
		_DataPoint->thetaX			<< "\t" <<
		_DataPoint->thetaY			<< "\t" <<
		_DataPoint->thetaZ			<< "\t" <<
	
		_DataPoint->grasp			<< "\t" <<
		_DataPoint->fingersTold		<< "\t" << //keeping this for consistency?
		
		std::endl;
		
#endif
#ifdef PCA_PROGRAM

	""; //because the main thing ends with a << hahahaha oh boy it's late
	for (i= 0; i<TOTAL_POSSIBLE_DOFS; i++)
	{
		outputFile << _DataPoint->dotProduct[i] << "\t";
	}
	
	for (i= 0; i<TOTAL_POSSIBLE_DOFS; i++)
	{
		outputFile << _DataPoint->sentValue[i] << "\t";
	}
	
		outputFile << std::endl;
#endif

		
		
	return;
}

#endif //DATA_STORAGE_H
