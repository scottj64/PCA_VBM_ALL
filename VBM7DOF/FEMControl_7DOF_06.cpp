//a bunch of random includes from Driver0_2.cpp
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>		// strerror
#include <stdexcept>	// for exception, runtime_error, out_of_range
#include <stdlib.h>		// atof


#include "DataStorage_7DOF_1.h"

/*

g++ -std=c++11 -pthread FEMControl_7DOF_06.cpp -ldl -o FEMControl_7DOF6

*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "yei_threespace_api4.h"
#include <fstream>
#include <sys/stat.h>

//Kinova
#include <dlfcn.h>
#include <vector>
#include "KinovaTypes.h"
#include "Kinova.API.UsbCommandLayerUbuntu.h"
#include "Kinova.API.CommLayerUbuntu.h"

//threading
#include <pthread.h>

//graphing
//#include "projection12.h"

#include "configFileReader_7DOF_3.h"

#include "globalVariablesAndConstants_7DOF_2.h"

#include "matlabThread_7DOF_4.h"


// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_handler(int signum)
{
	printf("Caught signal %d\n",signum);
	keepGoing = false;
}





#include "readerthreadWithYaw.h"



#include <unistd.h>
#include <sys/time.h>
#include <ctime>


int main(int argc, char * argv[])
{
   	int result; //used for error checking
   	
   	
   	// Register signal and signal handler
   	signal(SIGUSR1, signal_handler);
   	signal(SIGINT, signal_handler);
   	
   	
   	//start the new thread with the reader from the dongle
	pthread_t readerThread, matlabThread;
	int threadCreateReturn;
   	
	time_t now = time(0);
	tm *ltm = localtime(&now);
	
	char timeBuffer [21];
	int length = sprintf(timeBuffer, "%04d_%02d_%02d_%02d_%02d_%02d", 
								1900 + ltm->tm_year,	//year
								1 + ltm->tm_mon,		//month
								ltm->tm_mday,			//day
								ltm->tm_hour,			//hour
								ltm->tm_min,			//minute
								ltm->tm_sec				//second
								);
	
	
	if (argc !=2)
	{
		printf("Correct program call is \"./ProgramName xyz\" where \"xyz\" are participant initials\n");
		return 1;
	}
	

	char folder[128];
	char parameterFilename[128];
	char outputFilename[128];
	char c;

	strcpy(folder,"/home/aspelun1/Documents/7DOF (copy)/BOMI_Data/"); 
	strcat(folder,argv[1]);
	
	
	struct stat sb;

	if (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		printf("\n\nFolder exists.\n");
	}
	else
	{
		printf("Folder \"%s\" does not exist. Would you like for it to be created? -y\n", argv[1]);
		while( ( c=getchar() ) !='\n')
				
			
		if (c == 'y' || c == 'Y')
		{
			mkdir(folder, 0777);
			printf("Folder \"%s\" created\n", folder);
//			return 1;
		}
		else
		{
			printf("Did not input \"y\". Quitting.\n");
        	return 1;
        }
//		return 1; //just in case?
	}
	
	

	
	const char configFileName[] = "ConfigFileFullMain_7DOF2.txt";
	
	configFileReader( configFileName );
	


	strcpy(outputFilename,folder); 
	strcat(outputFilename,"/");
	strcat(outputFilename,timeBuffer);
	strcat(outputFilename,"_Session.txt");
	
	printf("Will create file \"%s/%s_Session.txt\" for next test.\nDo you wish to continue? -y\n", argv[1], timeBuffer);
	
	c = getchar();
	if (!(c == 'y' || c == 'Y'))
	{
		printf("See you next time!\n");
		return 1;
	}
	
	
	
	
	dataPoint currDP; //currentDataPoint
		



	fd_ = openDevice();
	if (( fd_ ) < 0 ) 
	{
		printf ("Unable to open/access dongle\n");
		return 1;
	}

	
	
	
	
	//initialize IMUs

/*	
	fd_ = openDevice();
	if (( fd_ ) < 0 ) 
	{
		printf ("Unable to open file");
		return 1;
	}
	printf("fd_ = %d\n",fd_);
*/	
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
	
	for (int device_iter = 0 + IMUoffsetValue; device_iter< NUMBER_OF_IMUS + IMUoffsetValue ; device_iter++)
	{
		do{
			result = setStreamingTiming(fd_, device_iter, 1000000/IMU_FREQUENCY, TSS_INFINITE_DURATION, 0); // (microseconds/second) / (cycles/second) = (microseconds/cycle)
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
	
	

	
	
	
	
	
	
	
	//Initialize Kinova Arm API
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
	int (*MyGetGripperStatus)(Gripper &);
	
	
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
		
		MyGetGripperStatus = (int (*)(Gripper &)) dlsym(APIhandle,"GetGripperStatus");

		//We validate that all the functions were loaded corectly.
		if( (MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MyGetQuickStatus == NULL) || 
		   (MyStartControlAPI == NULL) || (MySendBasicTrajectory == NULL) || 
		   (MyGetCartesianCommand == NULL) || (MyMoveHome == NULL) || 
		   (MyInitFingers == NULL) || (MySetCartesianControl == NULL) || 
		   (MyGetCartesianPosition == NULL) || (MyGetGripperStatus == NULL))
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
		
	
	
	QuickStatus status;
	CartesianPosition actualPosition;
	Gripper gripperStatus;
	
	if(canExecuteProgram)
	{
		printf("Main locking main.\n");
		pthread_mutex_lock( &ready_to_loop_mutex );
		printf("Main locked main.\n");
		
//	   	pthread_mutex_lock( &Matlab_initialized_mutex ); // only the thread that locks a mutex can unlock it
	   	threadCreateReturn = pthread_create( &matlabThread, NULL, matlabFunction, NULL);
//		printf("yabadabadoo");
		
		
		
		(*MyMoveHome)(); //can comment out this line to make the process go faster if there is a break in the IMU initialization
		(*MyInitFingers)();
		
		
		(*MyGetQuickStatus)(status);
		
		
		(*MyGetCartesianCommand)(actualPosition);
		
		
		(*MyGetGripperStatus)(gripperStatus); //look at 1800 of KinovaTypes access gripperStatus.Fingers[i].stuff
		
		//initialize to starting position
		pointToSend.Position.HandMode = POSITION_MODE; 
		pointToSend.Position.Type = CARTESIAN_POSITION;

		// I don't know what these are for
		pointToSend.LimitationsActive = 1;
		pointToSend.Limitations.speedParameter1 = 0.5;
		pointToSend.Limitations.speedParameter2 = 0.7;
		
		
		//This is the starting position
		pointToSend.Position.CartesianPosition.X = -centerX; //offset for hand orientation
		pointToSend.Position.CartesianPosition.Y = -0.3;
		pointToSend.Position.CartesianPosition.Z = centerY;
		pointToSend.Position.CartesianPosition.ThetaX = PI/2;
		pointToSend.Position.CartesianPosition.ThetaY = -PI/2;//+0.4f;
		pointToSend.Position.CartesianPosition.ThetaZ = PI;//+0.2f;
		
		//For some reason these values work because the status.RobotType = NULL rather than 0 for JACO
		//If this extends fingers too far or too little, change these values to orders of magintude smaller
		pointToSend.Position.Fingers.Finger1 = 0000.0f;
		pointToSend.Position.Fingers.Finger2 = 0000.0f;
		pointToSend.Position.Fingers.Finger3 = 0000.0f; //does this go more?
		
		
		printf( "Sending starting position.\n");
		
		printf("\n\nPlease position graph in correct place in next 4+ seconds.\n\n");
		
		result = (*MySendBasicTrajectory)(pointToSend);
		
		int y= 20; 
		for (int x = 0; x<y; x++)
		{
			usleep(4000000/y);
			result = (*MySendBasicTrajectory)(pointToSend);
		}
				
		// set the command type to velocity controls and set all values to zero
		pointToSend.Position.Type = CARTESIAN_VELOCITY;
		pointToSend.Position.HandMode = VELOCITY_MODE;
		pointToSend.Position.CartesianPosition.X = 0.0f;
		pointToSend.Position.CartesianPosition.Y = 0.0f;
		pointToSend.Position.CartesianPosition.Z = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaX = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaY = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaZ = 0.0f;
		pointToSend.Position.Fingers.Finger1 = 0.0f;
		pointToSend.Position.Fingers.Finger2 = 0.0f;
		pointToSend.Position.Fingers.Finger3 = 0.0f;
		
		result = (*MySendBasicTrajectory)(pointToSend);
	}
	else
	{
		printf( "Error detected, leaving the example...\n" );
		return 1;
	}
	




	
	
	printf("Program will tare IMUs and start streaming. Make sure user is in base position and then enter 'y' to continue/'q' to quit.\n");
	do{
		c = getchar(); //this is included from way up there
		if (c == 'q')
		{
			keepGoing = false;
			//Turn off Kinova API
			(*MyCloseAPI)();
			printf ("Kinova API closed\n");
			
			result = closeDevice (fd_);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_: %d\n",fd_);
			
			
			return 1;;
		}
	}while(c != 'y');
	
	
	
	printf("Main unlocking ready.\n");
	//The program needs to tell matlab that the loop is ready
	pthread_mutex_unlock( &ready_to_loop_mutex );
	printf("Main unlocked ready.\n");
	
	
	printf("Main locking matlab.\n");
	//The program needs to wait for a signal to say that it should start the loop
	pthread_mutex_lock( &Matlab_initialized_mutex );
	//thread_mutex_unlock( &Matlab_initialized_mutex ); //is there any danger of leaving amutex that isn't going to be referenced again, locked? Memory loss?
	printf("Main locked matlab.\n");
	
	//start the IMUs streaming
	
	
	for (int device_iter = 0 + IMUoffsetValue; device_iter< NUMBER_OF_IMUS + IMUoffsetValue ; device_iter++)
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
			//Turn off Kinova API
			(*MyCloseAPI)();
			printf ("Kinova API closed\n");
			
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
			//Turn off Kinova API
			(*MyCloseAPI)();
			printf ("Kinova API closed\n");
			
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
	
	
	
	
	
	float xVel, zVel, yVel, xRot, yRot, zRot, magnitude;//, tempVelocityX;
	magnitude = 1.0;
	//float scaledxVel, scaledzVel;
	
	struct timeval startOfProgramTime, currentTime;
	//float programTime;
	currDP.programTime = 0.0;
	
	unsigned long int counter = 0;
	unsigned long int sleepTime = 0;
	
	
	std::ofstream outputFile( outputFilename );
//	printDataHeader(outputFile); //removed as it makes later data processing more difficult
//	outputFile.close();
	

	
	
	threadCreateReturn = pthread_create( &readerThread, NULL, readerFunction, NULL);
	usleep(250000); //to avoid the E+17 values that randomly appear
	
	//The program needs to tell matlab that the loop is ready
//	pthread_mutex_unlock( &ready_to_loop_mutex );
	
	
	
	
	//The program needs to wait for a signal to say that it should start the loop
//	pthread_mutex_lock( &Matlab_initialized_mutex );
	//thread_mutex_unlock( &Matlab_initialized_mutex ); //is there any danger of leaving a mutex that isn't going to be referenced again, locked? Memory loss?
	
	
//	threadCreateReturn = pthread_create( &readerThread, NULL, readerFunction, NULL);
//	usleep(250000); //to avoid the E+17 values that randomly appear
	
	printf(".\n");
	printf("Starting loop.\n");
	gettimeofday(&startOfProgramTime, NULL);
	// loop everything:
	// get current position (takes ~3 milliseconds....) !
	// graph the initial points (this can do the dist(goal,current) checking)
		// set the initial point to (-0.12,0.5) so the initial check cause a random point to start
	// get the values from the dongle reader ! not applicable to this section !
	// multiply the matrix by the applicable values to get the two velocities !
	// send the velocities to the Kinova Arm !
	while(keepGoing) //errors should break out of the loop and set keepGoing to false for the reader thread
//   	while(counter<100*GRAPHING_FREQUENCY && keepGoing)
   	{
		gettimeofday(&currentTime, NULL);
		currDP.programTime  = (currentTime.tv_sec - startOfProgramTime.tv_sec)*1.0 + (currentTime.tv_usec - startOfProgramTime.tv_usec)/1000000.0;
		
//		printf("Getting position.\n");
		result = (*MyGetCartesianPosition)(data); //about 2ms
		if (result !=1)
		{
			printf("GetPosition:\t%d\n", result);
		}
//		printf("Got position.\n");
		
//		printf("Getting gripper status.\n");
//		result = (*MyGetGripperStatus)(gripperStatus);
//		printf("Got gripper status.\n");
		
		
		currDP.currentXloc = -data.Coordinates.X;
		currDP.currentYloc = -data.Coordinates.Y;
		currDP.currentZloc =  data.Coordinates.Z;
		
		currDP.currentXrot =  data.Coordinates.ThetaX;
		currDP.currentYrot =  data.Coordinates.ThetaY;
		currDP.currentZrot =  data.Coordinates.ThetaZ;
		
/*		currDP.fingerVal[0]  =  data.Fingers.Finger1;
		currDP.fingerVal[1]  =  data.Fingers.Finger2;
		currDP.fingerVal[2]  =  data.Fingers.Finger3;
		
		currDP.fingerSVal = (currDP.fingerVal[0] + currDP.fingerVal[1] + currDP.fingerVal[2]) / 3;
*/		currDP.fingersval = (data.Fingers.Finger1 + data.Fingers.Finger2 + data.Fingers.Finger3) / 3;
		

		for (int i = 0; i < NUMBER_OF_IMUS; i++)
		{
			pthread_mutex_lock( &orient[i].mutex );
			
			currDP.IMU_values[i*3  ] = orient[i].roll;
			currDP.IMU_values[i*3+1] = orient[i].yaw;
			currDP.IMU_values[i*3+2] = orient[i].pitch;
	
			pthread_mutex_unlock( &orient[i].mutex );	
		}

		//xVel = zVel = yVel = xRot = yRot = zRot = 
		//currDP.grasp = 0.0;

		//lock rawVelocity output
		pthread_mutex_lock( &forces.mutex );
	
		currDP.velocityXraw = forces.P1;
		currDP.velocityYraw = forces.P2;
		currDP.velocityZraw = forces.P3;
		currDP.thetaX		= forces.P5;
		currDP.thetaY		= forces.P6;
		currDP.thetaZ		= forces.P4;
		currDP.grasp		= forces.P7;

		//unlock rawVelocity output
		pthread_mutex_unlock( &forces.mutex );

	
		if (currDP.grasp < 0.0 && currDP.fingersval > MIN_FINGER_OPEN)
		{
			currDP.fingersTold = -1000;//MAX_FINGER_OPEN - currDP.fingersval;
		}
		else if (currDP.grasp > 0.0 && currDP.fingersval < MAX_FINGER_CLOSE)
		{
			currDP.fingersTold = 1000;//MIN_FINGER_CLOSE - currDP.fingersval;
		}
		else
		{
			currDP.fingersTold = 0;
		}
				
		//boundary of the table
		if (currDP.currentZloc <= Z_ROBOT_MIN )
		{
//			currDP.againstBorder = true;
			if(currDP.velocityZraw <= 0.0)
			{
				currDP.velocityZraw = 0.0;
			}
		}
		
		if (currDP.currentYloc <= Y_ROBOT_MIN )
		{
//			currDP.againstBorder = true;
			if(currDP.velocityYraw <= 0.0)
			{
				currDP.velocityYraw = 0.0;
			}
		}

		
		if (CartYOn) pointToSend.Position.CartesianPosition.X = -currDP.velocityXraw;	
		if (CartXOn) pointToSend.Position.CartesianPosition.Y = -currDP.velocityYraw;
		if (CartZOn) pointToSend.Position.CartesianPosition.Z =  currDP.velocityZraw;
		
		if (RotXOn) pointToSend.Position.CartesianPosition.ThetaX = currDP.thetaX;
		if (RotYOn) pointToSend.Position.CartesianPosition.ThetaY = currDP.thetaY;
		if (RotZOn) pointToSend.Position.CartesianPosition.ThetaZ = currDP.thetaZ;
		
		if (GrasperOn)
		{
			pointToSend.Position.Fingers.Finger1 = 
			pointToSend.Position.Fingers.Finger2 = 
			pointToSend.Position.Fingers.Finger3 = currDP.fingersTold;
		}
		
		
		printDataPoint(&currDP, outputFile); //output to file
		
		result = (*MySendBasicTrajectory)(pointToSend); //send velocity to robot; how long of a process is this?
		if (result !=1)
		{
			printf("SendTrajectory:\t%d\n", result);
		}
		
		
		counter++;
		
/*		if (counter%LOOP_FREQUENCY == 0)
		{
			printf("%ld\n",counter);
		}
*/		
		
		
		gettimeofday(&currentTime, NULL);
		
		sleepTime = (startOfProgramTime.tv_sec - currentTime.tv_sec)*1000000
				  + (startOfProgramTime.tv_usec - currentTime.tv_usec)
				  +	(1000000/LOOP_FREQUENCY)*counter;
		
		if ( (sleepTime>0) && (sleepTime<1000000/LOOP_FREQUENCY) )
		{
			usleep(	sleepTime );
		}
		
		//see how fast it's running
		//printf("%ld\t%ld\n",counter,sleepTime);
		
		//(*MySendBasicTrajectory)(pointToSend); //sending it twice to see if it speeds up
		//usleep(1000000/2/LOOP_FREQUENCY);
		
		
		
		//this is for preliminary debugging runs
		//if (counter>50) keepGoing = false;
		
  	}//END MAIN LOOP
  	
  	

   	keepGoing = false;
   	
   	strcpy(parameterFilename,folder); 
	strcat(parameterFilename,"/");
	strcat(parameterFilename,timeBuffer);
	strcat(parameterFilename,"_Parameters.txt");
	std::ofstream parameterFile( parameterFilename );
	printParameters(parameterFile); //output to file
	parameterFile.close();
	
	
	
	
	

	pointToSend.Position.CartesianPosition.X = 0.0f; //make the robot stop moving
	pointToSend.Position.CartesianPosition.Y = 0.0f;
	pointToSend.Position.CartesianPosition.Z = 0.0f;
	
	pointToSend.Position.CartesianPosition.ThetaX = 0.0f;
	pointToSend.Position.CartesianPosition.ThetaY = 0.0f;
	pointToSend.Position.CartesianPosition.ThetaZ = 0.0f;
	
	pointToSend.Position.Fingers.Finger1 = pointToSend.Position.Fingers.Finger2 = pointToSend.Position.Fingers.Finger3 = 0.0;
	
	(*MySendBasicTrajectory)(pointToSend);

   	usleep(2000000);
   	
   	//Putting the IMU stop_streaming portion here for hope of error fix, the reasoning being unknown
   	for (int device_iter = 0 + IMUoffsetValue; device_iter< NUMBER_OF_IMUS + IMUoffsetValue ; device_iter++)
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

   	

   	
   	
   	//Turn off Kinova API
	(*MyCloseAPI)();
	printf ("Kinova API closed\n");

   	//wait for the matlabThread to join; it should go pretty fast
   	pthread_join( matlabThread, NULL);
   	printf ("Matlab thread joined\n");
	
   	//wait for the readerThread to join; it should have quit once it got saw keepGoing == false
   	pthread_join( readerThread, NULL);
   	printf ("Reader thread joined\n");

   	
   	
   	//IMUS
   	for (int device_iter = 0 + IMUoffsetValue; device_iter< NUMBER_OF_IMUS + IMUoffsetValue ; device_iter++)
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
	
	

	outputFile.close();

	printf ("Output file \"%s/%s_Session.txt\" closed.\n", argv[1], timeBuffer);

	printf ("Exiting program\n");
   	
   	return EXIT_SUCCESS;
}

