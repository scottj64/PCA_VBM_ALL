#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <vector>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/time.h>

#define TOTAL_POSSIBLE_DOFS 7 //I need to find a better way to do this everywhere

struct dataPoint
{
	//output
	float programTime		= 0.0; //since start of streaming
	float IMU_values[NUMBER_OF_IMUS*3]	= {0.0};
	
	float		currentLoc[3]	= {0.0,0.0,0.0};
	float		currentRot[3]	= {0.0,0.0,0.0};
//	short int	fingerVal[3]	= {0,0,0};
	short int	fingerSVal		= 0;
	
	float 		dotProduct[TOTAL_POSSIBLE_DOFS]	= {0.0};
	float 		sentValue[TOTAL_POSSIBLE_DOFS]	= {0.0};
	
};




void printDataPoint(const dataPoint *_DataPoint, std::ofstream &outputFile)
{
	int i;
	outputFile << 
		_DataPoint->programTime << "\t";
		
	for (i= 0; i<NUMBER_OF_IMUS*3; i++)
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
		
		_DataPoint->fingerSVal		<< "\t";

	for (i= 0; i<TOTAL_POSSIBLE_DOFS; i++)
	{
		outputFile << _DataPoint->dotProduct[i] << "\t";
	}
	
	for (i= 0; i<TOTAL_POSSIBLE_DOFS; i++)
	{
		outputFile << _DataPoint->sentValue[i] << "\t";
	}
		
		outputFile << std::endl;
	return;
}

#endif //DATA_STORAGE_H
