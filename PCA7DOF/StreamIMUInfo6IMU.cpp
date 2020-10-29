//bool keepGoing = true;


//a bunch of random includes from Driver0_2.cpp
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h> //  stre`rror
#include <stdexcept> // for exception, runtime_error, out_of_range

//#include "DataStorage.h"

//g++ -std=c++11 -pthread StreamIMUInfo6IMU.cpp -ldl -o StreamIMUInfo6IMU -lboost_iostreams -lboost_system -lboost_filesystem

//#define ROBOT_IS_PLUGGED_IN true

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "yei_threespace_api4.h"
#include <fstream>
#include <sys/stat.h>


bool keepGoing = true; //the loops will check this value
int fd_ = 0; // file handle for dongle stream. must be done globally for threading
const int INITIAL_IMU = 4;
const int NUMBER_OF_IMUS = 1;


struct orientation
{
	float pitch = 0;
	float yaw = 0;
	float roll = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}orient[NUMBER_OF_IMUS]; //must be instantiated globally for threading

const int LOOP_FREQUENCY = 10; //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes









//threading
#include <pthread.h>

#include "readerthreadWithYaw.h"





struct dataPoint
{
	//output
	float programTime		= 0.0; //since start of streaming
	float IMU_values[18]	= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	
/*	float currentX			= 0.0;
	float currentY			= 0.0;
//	float currentZ			= 0.0;
	int   goalNum			= 0;
	int   epochType			= 0;	//whether this is a test or practice and which number it is
									//1-8:practice; 11:pre-test; 12:mid-test; 13:post-test
	short cursorStatus		= -1;	//-1: not moving; 0: the cursor is moving; 1: at a center goal; 2: at an outer goal; 
									//this stuff seriously needs to be fixed
	//internal
	bool wasJustOutsideGoal	= false;
	bool usingNewGoalColor	= true;
	
	
	float velocityX			= 0.0;
	float velocityY			= 0.0;
//	float velocityZ			= 0.0;
	float velocityXraw		= 0.0;
	float velocityYraw		= 0.0;
//	float velocityZraw		= 0.0;


	//internal
	struct timeval goalStartTime; //since current goal timer began
	struct timeval goalArrivalTime; //since cursor arrived in goal area
	int   goalIterator		= -1;   //the number of goals that have been reached
	
	float goalX				= 0.0;
	float goalY				= 0.0;
//	float goalZ				= 0.0;
	
	int score				= 0.0;
*/
};




void printDataHeader(std::ofstream &outputFile)
{
	outputFile << "programTime\tIMU0R\tIMU0Y\tIMU0P\tIMU1R\tIMU1Y\tIMU1P\tIMU2R\tIMU2Y\tIMU2P\tIMU3R\tIMU3Y\tIMU3P\tIMU4R\tIMU4Y\tIMU4P\tIMU5R\tIMU5Y\tIMU5P\t\n";
	return;
}


void printDataPoint(const dataPoint *_DataPoint, std::ofstream &outputFile)
{
	outputFile << 
		_DataPoint->programTime			<< "\t" <<
		_DataPoint->IMU_values[0]		<< "\t" <<
		_DataPoint->IMU_values[1]		<< "\t" <<
		_DataPoint->IMU_values[2]		<< "\t" <<
		_DataPoint->IMU_values[3]		<< "\t" <<
		_DataPoint->IMU_values[4]		<< "\t" <<
		_DataPoint->IMU_values[5]		<< "\t" <<
		_DataPoint->IMU_values[6]		<< "\t" <<
		_DataPoint->IMU_values[7]		<< "\t" <<
		_DataPoint->IMU_values[8]		<< "\t" <<
		_DataPoint->IMU_values[9]		<< "\t" <<
		_DataPoint->IMU_values[10]		<< "\t" <<
		_DataPoint->IMU_values[11]		<< "\t" <<
		_DataPoint->IMU_values[12]		<< "\t" <<
		_DataPoint->IMU_values[13]		<< "\t" <<
		_DataPoint->IMU_values[14]		<< "\t" <<
		_DataPoint->IMU_values[15]		<< "\t" <<
		_DataPoint->IMU_values[16]		<< "\t" <<
		_DataPoint->IMU_values[17]		<< //"\t" <<

		
		std::endl;
	return;
}











// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_handler(int signum)
{
	printf("Caught signal %d\n",signum);
	keepGoing = false;
}









int main(int argc, char * argv[])
{
   	
   	signal(SIGINT, signal_handler);
   	
   	char c;
   	
  
	dataPoint currDP; //currentDataPoint

	fd_ = openDevice();
	if (( fd_ ) < 0 ) 
	{
		printf ("Unable to open file");
		return 1;
	}
	//printf("fd_ = %d\n",fd_);



	//std::vector<dataPoint> dataStorageVector;
	
	
	
	
   	

	std::ofstream outputFile ("output.txt");
	

	
	int result; //used for error checking
	

	int k = 0;
	do{
		result = updateCurrentTimestamp(fd_, 0);
		if ( result < 0 ) 
		{
			printf ("Unable to update current timestamp, error: %d\n", result);
			k++;
			usleep(10000);
		}
	}while(result<0 && k<5);
	k=0;
	printf ("Current timestamp updated\n");
	
	
	const TSS_Stream_Command stream_slots[8] = {TSS_GET_TARED_ORIENTATION_AS_EULER_ANGLES,
                                      TSS_NULL,//GET_BATTERY_PERCENT_REMAINING might be something to include
                                      TSS_NULL,
									  TSS_NULL,
									  TSS_NULL,
									  TSS_NULL,
									  TSS_NULL,
									  TSS_NULL};
	
	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
	{
		do{
			result = setStreamingTiming(fd_, device_iter, 1000000/LOOP_FREQUENCY, TSS_INFINITE_DURATION, 0); // (microseconds/second) / (cycles/second) = (microseconds/cycle)
			if ( result < 0 ) 
			{
				printf ("Unable to set streaming timing for IMU #%d, error: %d\n", device_iter, result);
				k++;
				usleep(10000);
			}
			
		}while(result<0 && k<5);
		if (k==5) return 1;
		k=0;
		printf ("Streaming timing set for IMU #%d\n", device_iter);
		
		do{
			result = setStreamingSlots(fd_, device_iter, stream_slots);
			if ( result < 0 ) 
			{
				printf ("Unable to set streaming slots for IMU #%d, error: %d\n", device_iter, result);
				k++;
				usleep(10000);
			}
		}while(result<0 && k<5);
		if (k==5) return 1;
		k=0;
		printf ("Streaming slots set for IMU #%d\n", device_iter);
		
		/*	result = setStreamingHeader(fd_, device_iter);
			if ( result < 0 ) 
			{
				printf ("Unable to set streaming header for IMU #%d\n", device_iter);
				return 1;
			}
			printf ("Streaming header set for IMU #%d, error: %d\n", device_iter, result);
		*/	
		
		
		do{
			result = getBatteryLevel(fd_, device_iter);
			if ( result < 0 ) 
			{
				printf ("Unable to get battery level of IMU #%d, error: %d\n", device_iter, result);
				k++;
				usleep(10000);
			}
		}while(result<0 && k<5);
		if (k==5) return 1;
		k=0;
		printf ("\t\t\tbattery level of IMU #%d = %d\n", device_iter, result);
	}
	
	


	

	



	//usleep(2500000); // allow the user to see the battery levels but realize that the IMUs have already been tared.
	printf("Program will tare IMUs and start streaming. Make sure user is in base position and then ");
	do{
		printf("enter 'y' to continue/'q' to quit.\n");
		c = getchar(); //this is included from way up there
		if (c == 'q')
		{
			//Turn off Kinova API
			
			result = closeDevice (fd_);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_: %d\n",fd_);
			
			
			return 1;;
		}
	}while(c != 'y');
	
	
	
	//start the IMUs streaming
	
	
	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
	{
		do{
			result = tareWithCurrentOrientation(fd_, device_iter);
			if ( result < 0 ) 
			{
				printf ("Unable to tare orientation of IMU #%d, error: %d\n", device_iter, result);
				k++;
				usleep(10000);
			}
		}while(result<0 && k<5);
		if (k==5) 
		{
			
			result = closeDevice (fd_);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_: %d\n",fd_);
			
			
			return 1;
		}
		k=0;
		printf ("Orientation tared for IMU #%d\n", device_iter);	
		
		do{
			result = startStreaming(fd_, device_iter);
			if ( result < 0 ) 
			{
				printf ("Unable to start streaming for IMU #%d, error: %d\n", device_iter, result); 
				k++;
				usleep(10000);
			}
		}while(result<0 && k<5);
		if (k==5) 
		{
			
			result = closeDevice (fd_);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_: %d\n",fd_);
			
			
			return 1;
		}
		k=0;
		printf ("Streaming started for IMU #%d\n", device_iter);
	}
	
	
	//start the new thread with the reader from the dongle
	pthread_t readerThread;
	int threadCreateReturn;
	
	threadCreateReturn = pthread_create( &readerThread, NULL, readerFunction, NULL);
	
	struct timeval startOfProgramTime, currentTime;
	currDP.programTime = 0.0;

	unsigned long int counter = 0;
	unsigned long int sleepTime = 0;
	
	gettimeofday(&startOfProgramTime, NULL);
	
	while(keepGoing) //errors should break out of the loop and set keepGoing to false for the reader thread
   	{
		gettimeofday(&currentTime, NULL);
		currDP.programTime  = (currentTime.tv_sec - startOfProgramTime.tv_sec)*1.0 + (currentTime.tv_usec - startOfProgramTime.tv_usec)/1000000.0;
		
/*		c = getchar();
		if (c == '\n')
			c = getchar();
			
		
		if (c=='c')
		{
*/			for (int i = INITIAL_IMU; i< INITIAL_IMU + NUMBER_OF_IMUS; i++)
			{
				pthread_mutex_lock( &orient[i].mutex );
				currDP.IMU_values[i*3]   = orient[i].roll;
				currDP.IMU_values[i*3+1] = orient[i].yaw;
				currDP.IMU_values[i*3+2] = orient[i].pitch;
				pthread_mutex_unlock( &orient[i].mutex );	
			
			printf("%f\t%f\t%f\n",orient[i].roll,orient[i].yaw,orient[i].pitch);
			}
			
			printDataPoint(&currDP, outputFile); //output to file
/*		}
		else if (c=='q')
		{
			keepGoing = false;
		}
*/		
		counter++;

		gettimeofday(&currentTime, NULL);
		
		sleepTime = (startOfProgramTime.tv_sec - currentTime.tv_sec)*1000000
				  + (startOfProgramTime.tv_usec - currentTime.tv_usec)
				  +	(1000000/LOOP_FREQUENCY)*counter;
		
		if ( (sleepTime>0) && (sleepTime<1000000/LOOP_FREQUENCY) )
		{
			usleep(	sleepTime );
		}
  	}
   	
   	
   	
   	//Putting the IMU stop_streaming portion hear for hope of error fix, the reasoning being unknown
   	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
	{
		do{
			result = stopStreaming(fd_, device_iter);
			if ( result < 0 ) 
			{
				printf ("Unable to stop streaming for IMU #%d\n", device_iter);
			}
			k++;
			usleep(10000);
		}while(result<0 && k<10);
		if (k==10) printf ("\t\t\t\t\tRun ./stop_streaming in parent directory");
		k=0;
		printf ("Streaming stopped for IMU #%d\n", device_iter);
   	}

   	
   	//wait for the readerThread to join; it should have quit once it got saw keepGoing == false
   	pthread_join( readerThread, NULL);
   	
   	//IMUS
   	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
	{
		do{
			result = getBatteryLevel(fd_, device_iter);
			if ( result < 0 ) 
			{
				printf ("Unable to get battery level of IMU #%d, error: %d\n", device_iter, result);
			}
			k++;
			usleep(10000);
		}while(result<0 && k<10);
		if (k==10) printf ("\t\t\t\t\tRun ./stop_streaming in parent directory");
		k=0;
		printf ("battery level of IMU #%d = %d\n", device_iter, result);
	}
	
	
   	
   	
	do{
		result = closeDevice (fd_);
		if ( result < 0 ) 
		{
			printf ("Unable to close device; error: %d\n", result);
			k++;
			usleep(10000);
		}
	}while(result<0 && k<10);
	k=0;
	printf ("Device closed. fd_: %d\n",fd_);
	
	printf ("Exiting program\n");
   	
   	return EXIT_SUCCESS;
}

