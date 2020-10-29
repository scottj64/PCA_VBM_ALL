#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <vector>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/time.h>

struct dataPoint
{
	//output
	float programTime		= 0.0; //since start of streaming
	float IMU_values[NUMBER_OF_IMUS*3]	= {0.0};
	
	float		currentLoc[3]	= {0.0,0.0,0.0};
	float		currentRot[3]	= {0.0,0.0,0.0};
//	short int	fingerVal[3]	= {0,0,0};
	short int	fingerSVal		= 0;
	
	float velocityXraw		= 0.0;
	float velocityYraw		= 0.0;
	float velocityZraw		= 0.0;
	
	float thetaX			= 0.0;
	float thetaY			= 0.0;
	float thetaZ			= 0.0;
	
	float grasp				= 0.0;
	
	short int fingersTold	= 0;
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
		
		_DataPoint->fingerSVal		<< "\t" <<

		_DataPoint->velocityXraw	<< "\t" <<
		_DataPoint->velocityYraw	<< "\t" <<
		_DataPoint->velocityZraw	<< "\t" <<
	
//		_DataPoint->twistX			<< "\t" <<
//		_DataPoint->twistY			<< "\t" <<
//		_DataPoint->twistZ			<< "\t" <<
		_DataPoint->thetaX			<< "\t" <<
		_DataPoint->thetaY			<< "\t" <<
		_DataPoint->thetaZ			<< "\t" <<
	
		_DataPoint->grasp			<< "\t" <<
		_DataPoint->fingersTold		<< //"\t" <<
	//	_DataPoint->finger1told		<< "\t" <<
	//	_DataPoint->finger2told		<< "\t" <<
	//	_DataPoint->finger3told		<< "\t" <<
		
		std::endl;
	return;
}

#endif //DATA_STORAGE_H
