//a bunch of random includes from Driver0_2.cpp
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h> //  strerror
#include <stdexcept> // for exception, runtime_error, out_of_range



//g++ -std=c++11 -pthread Attempt0_5_3D.cpp -ldl -o Attempt0_5_3D -lboost_iostreams -lboost_system -lboost_filesystem


//#define ROBOT_IS_PLUGGED_IN true

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "yei_threespace_api4.h"
#include <fstream>

//Kinova
#include <dlfcn.h>
#include <vector>
#include "KinovaTypes.h"
#include "Kinova.API.UsbCommandLayerUbuntu.h"
#include "Kinova.API.CommLayerUbuntu.h"

//threading
#include <pthread.h>

//graphing
#include "projection2.h"


//#include <math.h>       /* sqrt */

#define NUMBER_OF_IMUS 4
#define PI 3.14159265359

//Global Variables

bool keepGoing = true; //the loops will check this value

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_handler(int signum)
{
	printf("Caught signal %d\n",signum);
	keepGoing = false;
}

int fd_ = 0; // file handle for dongle stream. must be done globally for threading

struct orientation
{
	float pitch = 0;
	//float yaw = 0;
	float roll = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}orient[NUMBER_OF_IMUS]; //must be instantiated globally for threading

#include "readerthread.h"






int main(int argc, char ** argv)
{
   	//initialize everything
   	
   	Gnuplot gp;
   	///graphing setup
	srand (static_cast <unsigned> (time(0)));
	double Xcurrent, Ycurrent, Xgoal, Ygoal, Xdelta, Ydelta;
	Xcurrent = Ycurrent = Xdelta = Ydelta = 0;
		
	Xgoal = 0.1;
	Ygoal = 0.5;
	
	struct timeval startOfProgramTime, startOfCurrentGoalTime;
	gettimeofday(&startOfProgramTime, NULL);
	
   	
   	
   	// Register signal and signal handler
   	signal(SIGINT, signal_handler);
	
	std::string ofilename;
    std::cout << "please enter desired output data file name: \n";
    std::getline(std::cin, ofilename);
	std::ofstream outputFile ( ofilename , std::ios::app);
	
	if (outputFile.is_open())
	{
		outputFile << "GoalTime\tXcurrent\tYcurrent\tXvel\tYvel" << "\n";
	}
		
	graph ( gp, 0, 0, Xgoal, Ygoal, 0, 0, outputFile, startOfProgramTime, startOfCurrentGoalTime);
	
	//Time_Stamp(ofilename);
	
	int result; //used for error checking
	
	//store the orientation values of the IMUs
	//orientation orient[NUMBER_OF_IMUS];
	
	//Read in Matrix from a file or I guess we can just hard code it for now
	std::string ifilename;
    std::cout << "please enter name of file containing matrix: \n";
    std::getline(std::cin, ifilename);
	
	double multMatrix[2][8];
	
	std::ifstream infile(ifilename);
	
	//double value;
	for (int col = 0; col <8; col ++)
	{
		for (int row = 0; row <3; row ++)
		{	
			infile >> multMatrix[row][col];
		}
	}
	// I will want to input this file name as argv[1]. See the following file for code:
	//  /home/aspelun1/Documents/Testing_functions/initializeMatrixFromTextFile/initializeMatrix.cpp
	
	float maxSpeed = 0.1f;
	
	
	
	
	//Initialize Kinova Arm API ONLY VELOCITY CONTROL SO FAR STILL NEED CARTESIAN POSITION AND ORIGINAL LOCATION
	
	
	
	void * APIhandle;

	bool canExecuteProgram = false;
	CartesianPosition data;

	int (*MyInitAPI)();
	int (*MyCloseAPI)();
	int (*MyStartControlAPI)();

	//This function get the actual angular command of the robot.
	int (*MyGetQuickStatus)(QuickStatus &);
	int (*MySendBasicTrajectory)(TrajectoryPoint command); // command is not used again
	int (*MyGetCartesianCommand)(CartesianPosition &Response); //not used again
	int (*MyMoveHome)();
	int (*MyInitFingers)();
	int (*MySetCartesianControl)();
	
	int (*MyGetCartesianPosition)(CartesianPosition &);
	
	printf("\nAddress items for Kinova created\n");
	
	APIhandle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);

	if(APIhandle != NULL)
	{
		MyInitAPI = (int (*)()) dlsym(APIhandle,"InitAPI");
		MyCloseAPI = (int (*)()) dlsym(APIhandle,"CloseAPI");
		MyGetQuickStatus = (int (*)(QuickStatus &)) dlsym(APIhandle,"GetQuickStatus");
		MyStartControlAPI = (int (*)()) dlsym(APIhandle,"StartControlAPI");
		MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendBasicTrajectory");
		MyGetCartesianCommand = (int (*)(CartesianPosition&)) dlsym(APIhandle,"GetCartesianCommand");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");
		MyInitFingers = (int (*)()) dlsym(APIhandle,"InitFingers");
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MyGetCartesianPosition = (int (*)(CartesianPosition &)) dlsym(APIhandle,"GetCartesianPosition");

		//We validate that all the functions were loaded corectly.
		if( (MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MyGetQuickStatus == NULL) || 
		   (MyStartControlAPI == NULL) || (MySendBasicTrajectory == NULL) || 
		   (MyGetCartesianCommand == NULL) || (MyMoveHome == NULL) || 
		   (MyInitFingers == NULL) || (MySetCartesianControl == NULL) || 
		   (MyGetCartesianPosition == NULL) )
		{
			printf( "Can't load all the functions from the library.\n");
			return 1;
		}
		else
		{
			printf( "All the functions from the library have been loaded.\n");
			
			//Init the API
			result = (*MyInitAPI)();

			//printf( "result = %d\n", result);

			if(result == NO_ERROR_KINOVA)
			{
				printf( "The API has been initialized.\n");
				canExecuteProgram = true;
			}
			else
			{
				printf( "Cannot initializes the API.\n");
				return 1;
			}
		}
	}
	else
	{
		printf( "Unable to load the API.\n");
		return 1;
	}
	
	TrajectoryPoint pointToSend;
	
	if(canExecuteProgram)
	{
		(*MyMoveHome)(); //can comment out this line to make the process go faster if there is a break in the IMU initialization
		(*MyInitFingers)();
		
		QuickStatus status;
		(*MyGetQuickStatus)(status);
		
		CartesianPosition actualPosition;
		(*MyGetCartesianCommand)(actualPosition);
		
		
		//initialize to starting position
		pointToSend.Position.HandMode = POSITION_MODE; 
		pointToSend.Position.Type = CARTESIAN_POSITION;

		// I don't know what these are for
		pointToSend.LimitationsActive = 1;
		pointToSend.Limitations.speedParameter1 = maxSpeed;
		//pointToSend.Limitations.speedParameter2 = 0.7;
		
		
		//This is the starting position
		pointToSend.Position.CartesianPosition.X = -Xgoal;
		pointToSend.Position.CartesianPosition.Y = -0.46f;
		pointToSend.Position.CartesianPosition.Z = Ygoal;
		pointToSend.Position.CartesianPosition.ThetaX = PI/2;
		pointToSend.Position.CartesianPosition.ThetaY = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaZ = PI;
		
		//For some reason these values work because the status.RobotType = NULL rather than 0 for JACO
		//If this extends fingers too far or too little, change these values to orders of magintude smaller
		pointToSend.Position.Fingers.Finger1 = 6000.0f;
		pointToSend.Position.Fingers.Finger2 = 6000.0f;
		pointToSend.Position.Fingers.Finger3 = 0.0f;
		
		printf( "Sending starting position.\n");
		result = (*MySendBasicTrajectory)(pointToSend);
		
		usleep(4000000);
		
		// set the command type to velocity controls and set all values to zero
		pointToSend.Position.Type = CARTESIAN_VELOCITY;
		pointToSend.Position.CartesianPosition.X = 0.0f;
		pointToSend.Position.CartesianPosition.Y = 0.0f;
		pointToSend.Position.CartesianPosition.Z = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaX = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaY = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaZ = 0.0f;
		pointToSend.Position.Fingers.Finger1 = 0.0f;
		pointToSend.Position.Fingers.Finger2 = 0.0f;
		pointToSend.Position.Fingers.Finger3 = 0.0f;
	}
	else
	{
		printf( "Error detected, leaving the example...\n" );
		return 1;
	}
	











	//initialize IMUs
	//int fd_; // file handle for dongle stream must be done globally
	
	fd_ = openDevice();
	if (( fd_ ) < 0 ) 
	{
		printf ("Unable to open file");
		return 1;
	}
	printf("fd_ = %d\n",fd_);
	
	result = updateCurrentTimestamp(fd_, 0);
	if ( result < 0 ) 
	{
		printf ("Unable to update current timestamp, error: %d\n", result);
		return 1;
	}
	printf ("Current timestamp updated\n");
	
	const TSS_Stream_Command stream_slots[8] = {TSS_GET_TARED_ORIENTATION_AS_EULER_ANGLES,
                                      TSS_NULL,//GET_BATTERY_PERCENT_REMAINING might be something to include
                                      TSS_NULL,
									  TSS_NULL,
									  TSS_NULL,
									  TSS_NULL,
									  TSS_NULL,
									  TSS_NULL};
	
	for (int device_iter = 0; device_iter< NUMBER_OF_IMUS ; device_iter++)
	{
		result = setStreamingTiming(fd_, device_iter, 1000000/100, TSS_INFINITE_DURATION, 0); // (microseconds/second) / (cycles/second) = (microseconds/cycle)
		if ( result < 0 ) 
		{
			printf ("Unable to set streaming timing for IMU #%d, error: %d\n", device_iter, result);
			return 1;
		}
		printf ("Streaming timing set for IMU #%d\n", device_iter);
		
		result = setStreamingSlots(fd_, device_iter, stream_slots);
		if ( result < 0 ) 
		{
			printf ("Unable to set streaming slots for IMU #%d, error: %d\n", device_iter, result);
			return 1;
		}
		printf ("Streaming slots set for IMU #%d\n", device_iter);
		
	/*	result = setStreamingHeader(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to set streaming header for IMU #%d\n", device_iter);
			return 1;
		}
		printf ("Streaming header set for IMU #%d, error: %d\n", device_iter, result);
	*/	
		
		result = tareWithCurrentOrientation(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to tare orientation of IMU #%d, error: %d\n", device_iter, result);
			return 1;
		}
		printf ("Orientation tared for IMU #%d\n", device_iter);
		
		
		result = getBatteryLevel(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to get battery level of IMU #%d, error: %d\n", device_iter, result);
			return 1;
		}
		printf ("\t\t\tbattery level of IMU #%d = %d\n", device_iter, result);
	}
/*	
	// I don't know if this will ever be needed
	result = broadcastSynchronizationPulse(fd_);
	if ( result < 0 ) 
	{
		printf ("Unable to broadcast synchronization pulse, error: %d\n", result);
		return 1;
	}
	printf ("Synchronization pulse broadcasted\n");
*/	



	usleep(2500000); // allow the user to see the battery levels but realize that the IMUs have already been tared.
	
	
	
	
	//start the IMUs streaming
	
	
	for (int device_iter = 0; device_iter< NUMBER_OF_IMUS ; device_iter++)
	{
		result = startStreaming(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to start streaming for IMU #%d, error: %d\n", device_iter, result); //stop the streaming for all successful ones so far
			return 1;
		}
		printf ("Streaming started for IMU #%d\n", device_iter);
	}
	
	
	//start the new thread with the reader from the dongle
	pthread_t readerThread;
	int threadCreateReturn;
	
	threadCreateReturn = pthread_create( &readerThread, NULL, readerFunction, NULL);
	
	
	
	
	float xVel, zVel, magnitude, yVel;
	xVel = zVel = magnitude = yVel = 1.0;
	
	



	// loop everything:
	// get current position (takes ~3 milliseconds....) !
	// graph the initial points (this can do the dist(goal,current) checking)
		// set the initial point to (-0.12,0.5) so the initial check cause a random point to start
	// get the values from the dongle reader ! not applicable to this section !
	// multiply the matrix by the applicable values to get the two velocities !
	// send the velocities to the Kinova Arm !
	while(keepGoing) //errors should break out of the loop and set keepGoing to false for the reader thread
   	{
		result = (*MyGetCartesianPosition)(data); //about 2ms
		
		//graph ( gp, Xcurrent, Ycurrent, Xgoal, Ygoal, Xdelta, Ydelta,outputFile, startOfProgramTime, startOfCurrentGoalTime);
		//graph ( gp, -1.0*data.Coordinates.X, data.Coordinates.Z, Xgoal, Ygoal, -0.0625*xVel/magnitude, 0.0625*zVel/magnitude,
		//		outputFile, startOfProgramTime, startOfCurrentGoalTime);
		
	
		xVel = zVel = yVel = 0.0;
		for (int i = 0; i< NUMBER_OF_IMUS; i++)
		{
			pthread_mutex_lock( &orient[i].mutex );
			xVel += multMatrix[0][i*2]*orient[i].roll*-1 + multMatrix[0][i*2-1]*orient[i].pitch;
			zVel += multMatrix[1][i*2]*orient[i].roll*-1 + multMatrix[1][i*2-1]*orient[i].pitch;
			yVel += multMatrix[2][i*2]*orient[i].roll*-1 + multMatrix[2][i*2-1]*orient[i].pitch;
			pthread_mutex_unlock( &orient[i].mutex );
		}
		
		magnitude = sqrt(xVel*xVel + zVel*zVel + yVel*yVel);
		if ( magnitude < 5)
		{
			pointToSend.Position.CartesianPosition.X = 0.0f;
			pointToSend.Position.CartesianPosition.Z = 0.0f;
			pointToSend.Position.CartesianPosition.Y = 0.0f;
			//printf("xVel: %7.4f\tzVel: %7.4f but sending 0s\n", xVel, zVel);
		}
		else
		{
			pointToSend.Position.CartesianPosition.X = xVel/magnitude*maxSpeed; // /640.0; //these values are the result of the matrix
			pointToSend.Position.CartesianPosition.Z = zVel/magnitude*maxSpeed; // /384.0;
			pointToSend.Position.CartesianPosition.Y = yVel/magnitude*maxSpeed;
			//printf("xVel: %7.4f\tzVel: %7.4f\n", xVel/magnitude*maxSpeed, zVel/magnitude*maxSpeed);
		}
		
		
		(*MySendBasicTrajectory)(pointToSend); //how long of a process is this?
		
		usleep(10000); //no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes
   	}
   	
   	
   	pointToSend.Position.CartesianPosition.X = 0; //these values are the result of the matrix
	pointToSend.Position.CartesianPosition.Z = 0;
	pointToSend.Position.CartesianPosition.Y = 0;
	printf("xVel: 0\tzVel: 0\tyVel: 0\n");
   	
   	
   	
   	
   	//Turn off Kinova API
	(*MyCloseAPI)();
	printf ("Kinova API closed\n");
   	
   	
   	pthread_join( readerThread, NULL);
   	
   	
   	//close everything down
   	//IMUS
   	for (int device_iter = 0; device_iter< NUMBER_OF_IMUS ; device_iter++)
	{
		result = stopStreaming(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to stop streaming for IMU #%d\n", device_iter);
		}
		printf ("Streaming stopped for IMU #%d\n", device_iter);
		
		result = getBatteryLevel(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to get battery level of IMU #%d, error: %d\n", device_iter, result);
			return 1;
		}
		printf ("battery level of IMU #%d = %d\n", device_iter, result);
	}
   	
   	result = closeDevice (fd_);
	if ( result != 0 ) 
	{
		printf ("Unable to close device"); 
	}
	printf ("Device closed. fd_: %d\n",fd_);
	
	
	gp << "quit" << std::endl;
	
	
	printf ("Exiting program\n");
   	
   	return EXIT_SUCCESS;
}
