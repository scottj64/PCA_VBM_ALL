#define VBM_PROGRAM

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

// g++ -std=c++11 -pthread VBM_7DOF_04_noGrasp.cpp -ldl -o /home/aspelun1/Documents/IncrementalLearning/VBM_7DOF_04_noGrasp

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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
#include <sys/time.h>
#include <ctime>

#include "globalVarsAndConsts_7DOF_VBM_02.h"
//#include "DataStorage_7DOF_VBM_01.h"
//#include "configFileReader_7DOF_VBM_01.h"

#include "../SharedFiles/globalVarsAndConsts_7DOF_Shared_01.h"
#include "../SharedFiles/DataStorage_7DOF_01.h"
#include "../SharedFiles/yei_threespace_api5.h"
#include "../SharedFiles/readerthreadWithYaw.h"
#include "../SharedFiles/positionInitializer.h"
#include "../SharedFiles/goalProcess02.h"
#include "../SharedFiles/teensyReaderThread_01.h"

#include "matlabThread_7DOF_4.h"


// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_handler(int signum)
{
	printf("Caught signal %d\n",signum);
	keepGoing = false;
}


int main(int argc, char * argv[])
{
	int result; //used for error checking
	
	// Register signal and signal handler
	signal(SIGINT, signal_handler);
	signal(SIGUSR1, signal_handler);
	
	//start the new thread with the reader from the dongle
	pthread_t readerThread, matlabThread, teensyThread;
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
	
	if (argc !=3)
	{
		printf("Correct program call is \"./ProgramName xyz #\"\nwhere \"xyz\" are participant initials and # is the number of degrees of freedom being used.\n");
		return 1;
	}
	
	int NUM_OF_DOFs = atoi(argv[2]);
	
	
	char folder[128];
//	char parameterFilename[128];
	char outputFilename[128];
	char outputFilenameTemp[128];
	char c;
	
	strcpy(folder,"/home/aspelun1/Documents/IncrementalLearning/Data/"); 
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

	
//	const char configFileName[] = "VBM7DOF/ConfigFileVBM.txt";
	
//	configFileReader( configFileName );
	
	
	

	strcpy(outputFilename,folder); 
	strcat(outputFilename,"/");
	strcat(outputFilename,"VBM_Session_");
	const char numDof[1] = {'0' + NUM_OF_DOFs};
	strcat(outputFilename,numDof);
	strcat(outputFilename,"DOF_");
	strcpy(outputFilenameTemp,outputFilename);
	
	int q = 1;
	while(true)
	{
		strcpy(outputFilename,outputFilenameTemp);
		const char sessionNum[1] = {'0' + q};
		strcat(outputFilename,sessionNum);
		strcat(outputFilename,".txt");
		
		if (stat(outputFilename, &sb) != 0)
		{
			printf("Result file \"VBM_Session_%dDOF_%d.txt\" will be produced.\nDo you wish to continue? -y\n", NUM_OF_DOFs, q);
			break;
		}
		q++;
	}
	
	while( ( c=getchar() ) =='\n');
	if (!(c == 'y' || c == 'Y'))
	{
		printf("See you next time!\n");
		return 1;
	}
	
	int fd,k;//, fd_teensy, fd_dongle, n;
	fd = fd_dongle = fd_teensy = -1;
	unsigned char buf[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	/* open serial port */
	
	
	usleep(500000);
	fd = openDevice(0);

	/* Send byte to trigger Arduino to send string back */
	unsigned char msg[3];
	msg[0] = 0xf7;
	msg[1] = 0xd4;
	msg[2] = msg[1];
	
	write(fd, &msg[0], sizeof(msg));
	/* Receive string from Arduino */
	k = read(fd, buf, 8);
	printf("buf = %d %d %d %d %d %d %d %d\n",int(buf[0]),int(buf[1]),int(buf[2]),int(buf[3]),int(buf[4]),int(buf[5]),int(buf[6]),int(buf[7]));
	if (buf[0] == 1)
	{
		fd_teensy = fd;
		printf("teensy: ACM0\n");
	}
	else if (buf[0] == 3)
	{
		fd_dongle = fd;
		printf("dongle: ACM0\n");
	}
	for (int r = 0; r<8;r++)
	{
		buf[r] = 0x00;
	}
		
	fd = openDevice(1);
	
	write(fd, &msg[0], sizeof(msg));
	/* Receive string from Arduino */
	k = read(fd, buf, 8);
	printf("buf = %d %d %d %d %d %d %d %d\n",int(buf[0]),int(buf[1]),int(buf[2]),int(buf[3]),int(buf[4]),int(buf[5]),int(buf[6]),int(buf[7]));
	if (buf[0] == 1)
	{
		fd_teensy = fd;
		printf("teensy: ACM1\n");
	}
	else if (buf[0] == 3)
	{
		fd_dongle = fd;
		printf("dongle: ACM1\n");
	}
	
	printf("fd_dongle = %d\nfd_teensy = %d\n",fd_dongle,fd_teensy);

	if ( fd_dongle <= 0 ) 
	{
		printf ("Unable to open/access dongle\n");
		return 1;
	}
	if ( fd_teensy <= 0 ) 
	{
		printf ("Unable to open/access teensy\n");
		return 1;
	}
	
	dataPoint currDP; //currentDataPoint
	
	if (NUM_OF_DOFs <=3)
	{
		positionDOFInitializer(NUM_OF_DOFs);
		const char ledNUM[] = {(char)(currDP.goalNum+6)};
		write( fd_teensy, ledNUM, sizeof(ledNUM) ); // Send to device
		printf("Sent ledNUM %d to teensy\n",currDP.goalNum+6);
	}
	else //NUM_OF_DOFs >=4
	{
		currDP.occupiedSlot = currDP.goalSlot = goalSlotPicker(NUM_OF_DOFs);
		const char ledNUM[] = {(char)currDP.goalSlot};
		write( fd_teensy, ledNUM, sizeof(ledNUM) ); // Send to device
		printf("Sent ledNUM %d to teensy\n",currDP.goalSlot);
	}
	
	
	//initialize IMUs
	k = 0;
	do{
		result = updateCurrentTimestamp(fd_dongle, 0);
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
			result = setStreamingTiming(fd_dongle, device_iter, 1000000/IMU_FREQUENCY, TSS_INFINITE_DURATION, 0); // (microseconds/second) / (cycles/second) = (microseconds/cycle)
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
			result = setStreamingSlots(fd_dongle, device_iter, stream_slots);
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
		
		/*	result = setStreamingHeader(fd_dongle, device_iter);
			if ( result < 0 ) 
			{
				printf ("Unable to set streaming header for IMU #%d\n", device_iter);
				return 1;
			}
			printf ("Streaming header set for IMU #%d, error: %d\n", device_iter, result);
		*/	
		
		
		do{
			result = getBatteryLevel(fd_dongle, device_iter);
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
		
//		 	pthread_mutex_lock( &Matlab_initialized_mutex ); // only the thread that locks a mutex can unlock it
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
		pointToSend.Position.CartesianPosition.X = -START_X;
		pointToSend.Position.CartesianPosition.Y = -START_Y;
		pointToSend.Position.CartesianPosition.Z =	START_Z;
		pointToSend.Position.CartesianPosition.ThetaX = START_ROT_X;
		pointToSend.Position.CartesianPosition.ThetaY = START_ROT_Y;
		pointToSend.Position.CartesianPosition.ThetaZ = START_ROT_Z;

		
		//For some reason these values work because the status.RobotType = NULL rather than 0 for JACO
		//If this extends fingers too far or too little, change these values to orders of magintude smaller
		pointToSend.Position.Fingers.Finger1 = 
		pointToSend.Position.Fingers.Finger2 = 6500.0f;
		pointToSend.Position.Fingers.Finger3 = 3500.0f; //does this go more? It does, to 6250 at least
		
		printf( "Sending starting position.\n");
		
		result = (*MySendBasicTrajectory)(pointToSend);
		
		int y = 100*3;
		for (int x = 0; x<y; x++)
		{
			usleep(10000);
			result = (*MySendBasicTrajectory)(pointToSend);
		}
		
		
//		c = 'r'; //this is included from way up there
//		while(c != 'd')
//		{
//			c = 'r';
//			while (c != 'c')
//			{
//				printf("Please place foam between fingers and press 'c' to close fingers.\n");
//				while( ( c=getchar() ) =='\n'){}
//				//c = getchar();
//			}
////			printf("1%c\n",c);
//			pointToSend.Position.Fingers.Finger1 = 
//			pointToSend.Position.Fingers.Finger2 = 6250.0f;
//			pointToSend.Position.Fingers.Finger3 = 3500.0f;
//		
//			result = (*MySendBasicTrajectory)(pointToSend);
//			y = 125;
//			for (int x = 0; x<y; x++)
//			{
//				usleep(8000);
//				result = (*MySendBasicTrajectory)(pointToSend);
//			}
//			c = 'r';
//			while (!(c == 'd' || c == 'o'))
//			{
//				printf("If you are content with the grasp, press 'd' for 'done'.\nIf not, press 'o' to open the grasper and try again.\n");
//				while( ( c=getchar() ) =='\n'){}
//				//c = getchar();
//			}
////			printf("2%c\n",c);
//			if(c == 'd') continue;
//			
//			pointToSend.Position.Fingers.Finger1 = 
//			pointToSend.Position.Fingers.Finger2 = 5500.0f;
//			pointToSend.Position.Fingers.Finger3 = 3500.0f;
//		
//			result = (*MySendBasicTrajectory)(pointToSend);
//			y = 125;
//			for (int x = 0; x<y; x++)
//			{
//				usleep(8000);
//				result = (*MySendBasicTrajectory)(pointToSend);
//			}
//		}
		
		
		
		
		
		// set the command type to velocity controls and set all values to zero
		pointToSend.Position.Type = CARTESIAN_VELOCITY;
		pointToSend.Position.HandMode = VELOCITY_MODE;
		pointToSend.Position.CartesianPosition.X = 0.0f;
		pointToSend.Position.CartesianPosition.Y = 0.0f;
		pointToSend.Position.CartesianPosition.Z = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaX = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaY = 0.0f;
		pointToSend.Position.CartesianPosition.ThetaZ = 0.0f;
		pointToSend.Position.Fingers.Finger1 = 
		pointToSend.Position.Fingers.Finger2 = 
		pointToSend.Position.Fingers.Finger3 = 0.0f;
		
		result = (*MySendBasicTrajectory)(pointToSend);
	}
	else
	{
		printf( "Error detected, leaving the example...\n" );
		return 1;
	}
	
	
	initializeGoalOrder3D(NUM_OF_DOFs);
	
	printf("Program will tare IMUs and start streaming. Make sure user is in base position and then ");
	do{
		printf("enter 'y' to continue/'q' to quit.\n");
		while ((c = getchar()) == '\n'); //this is included from way up there
		if (c == 'q')
		{
			keepGoing = false;
			//Turn off Kinova API
			(*MyCloseAPI)();
			printf ("Kinova API closed\n");
			
			result = closeDevice (fd_dongle);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_dongle: %d\n",fd_dongle);
			
			result = closeDevice (fd_teensy);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_teensy: %d\n",fd_teensy);
			
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
			result = tareWithCurrentOrientation(fd_dongle, device_iter);
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
			
			result = closeDevice (fd_dongle);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_dongle: %d\n",fd_dongle);
			
			return 1;
		}
		k=0;
		printf ("Orientation tared for IMU #%d\n", device_iter);	
		
		do{
			result = startStreaming(fd_dongle, device_iter);
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
			
			result = closeDevice (fd_dongle);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_dongle: %d\n",fd_dongle);
			
			return 1;
		}
		k=0;
		printf ("Streaming started for IMU #%d\n", device_iter);
	}
	
	
	
	threadCreateReturn = pthread_create( &readerThread, NULL, readerFunction, NULL);
	threadCreateReturn = pthread_create( &teensyThread, NULL, teensyReaderFunction, NULL);
	usleep(250000); //to avoid the E+17 values that randomly appear
	
	
//	float xVel, zVel, yVel, xRot, yRot, zRot, magnitude;//, tempVelocityX;
//	magnitude = 1.0;
	//float scaledxVel, scaledzVel;
	
	struct timeval startOfProgramTime, currentTime;
	//float programTime;
	currDP.programTime = 0.0;
	
	unsigned long int counter = 0;
	unsigned long int sleepTime = 0;
	
	float sendX,sendY,sendZ;
	float currX,currY,currZ;
	
	std::ofstream outputFile( outputFilename );
//	printDataHeader(outputFile); //removed as it makes later data processing more difficult
//	outputFile.close();
	
	printf("%d\n",currDP.goalSlot);
	
	
	printf(".\n");
	printf("Starting loop.\n");
	gettimeofday(&startOfProgramTime, NULL);
	// loop everything:
	// get current position (takes ~3 milliseconds....) !
	// get the values from the dongle reader ! not applicable to this section !
	// multiply the matrix by the applicable values to get the two velocities !
	// send the velocities to the Kinova Arm !
	while(keepGoing) //errors should break out of the loop and set keepGoing to false for the reader thread
	{
		gettimeofday(&currentTime, NULL);
		currDP.programTime = (currentTime.tv_sec - startOfProgramTime.tv_sec)*1.0 + (currentTime.tv_usec - startOfProgramTime.tv_usec)/1000000.0;
		
		result = (*MyGetCartesianPosition)(data); //about 2ms
			
		currDP.currentLoc[0] = -data.Coordinates.X;
		currDP.currentLoc[1] = -data.Coordinates.Y;
		currDP.currentLoc[2] =	data.Coordinates.Z;
		
		currDP.currentRot[0] =	data.Coordinates.ThetaX;
		currDP.currentRot[1] =	data.Coordinates.ThetaY;
		currDP.currentRot[2] =	data.Coordinates.ThetaZ;
		
/*		currDP.fingerVal[0]	=	data.Fingers.Finger1;
		currDP.fingerVal[1]	=	data.Fingers.Finger2;
		currDP.fingerVal[2]	=	data.Fingers.Finger3;
		
		currDP.fingerSVal = (currDP.fingerVal[0] + currDP.fingerVal[1] + currDP.fingerVal[2]) / 3;
*/		currDP.fingerSVal = (data.Fingers.Finger1 + data.Fingers.Finger2 + data.Fingers.Finger3) / 3;
		
		for (int i = 0; i < NUMBER_OF_IMUS; i++)
		{
			pthread_mutex_lock( &orient[i].mutex );
			
			currDP.IMU_values[i*3  ] = orient[i].roll;
			currDP.IMU_values[i*3+1] = orient[i].yaw;
			currDP.IMU_values[i*3+2] = orient[i].pitch;
	
			pthread_mutex_unlock( &orient[i].mutex );	
		}
		
		if (NUM_OF_DOFs <= 3)
		{
			currPose2TipLEDLoc(&currDP);
			goalCheck3D(&currDP,currentTime,fd_teensy);
		}
		else //NUM_OF_DOFs >=4
		{
			/*
			which slot is the magnet in
			check that the occupied slot is equal to the intended
			when it is no longer, check position and consider success when it is back in the middle
			*/
			pthread_mutex_lock( &slot_mutex );
			currDP.occupiedSlot = currentlyOccupiedSlot_GLOBAL;
			pthread_mutex_unlock( &slot_mutex );
			
			if (counter%100==0)
			{
				printf("%d\n",currDP.occupiedSlot);
			}
			if ( currDP.occupiedSlot == currDP.goalSlot )
			{
				if (currDP.currentLoc[0]<=0.35 &&currDP.currentLoc[1]<=0.45 &&currDP.currentLoc[2]>=0.25)
				{
					keepGoing = false;
				}
			}
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

	
		if (currDP.grasp < 0.0 && currDP.fingerSVal > MIN_FINGER_OPEN)
		{
			currDP.fingersTold = -1000;//MAX_FINGER_OPEN - currDP.fingerSVal;
		}
		else if (currDP.grasp > 0.0 && currDP.fingerSVal < MAX_FINGER_CLOSE)
		{
			currDP.fingersTold = 1000;//MIN_FINGER_CLOSE - currDP.fingerSVal;
		}
		else
		{
			currDP.fingersTold = 0;
		}
		
		currDP.againstBorder = false;
		//Boundary Checkin				
		if((currDP.currentLoc[0] - X_ROBOT_MIN) <= 0.0 && currDP.velocityXraw <= 0.0)
		{
			currDP.velocityXraw = 0.0;
			currDP.againstBorder = true;
		}
		if((currDP.currentLoc[0] - X_ROBOT_MAX) >= 0.0 && currDP.velocityXraw >= 0.0)
		{
			currDP.velocityXraw = 0.0;
			currDP.againstBorder = true;
		}
		if((currDP.currentLoc[1] - Y_ROBOT_MIN) <= 0.0 && currDP.velocityYraw <= 0.0)
		{
			currDP.velocityYraw = 0.0;
			currDP.againstBorder = true;
		}
		if((currDP.currentLoc[1] - Y_ROBOT_MAX) >= 0.0 && currDP.velocityYraw >= 0.0)
		{
			currDP.velocityYraw = 0.0;
			currDP.againstBorder = true;
		}
		if((currDP.currentLoc[2] - Z_ROBOT_MIN) <= 0.0 && currDP.velocityZraw <= 0.0)
		{
			currDP.velocityZraw = 0.0;
			currDP.againstBorder = true;
		}
		
		
		
				
		if (CartXOn)
			pointToSend.Position.CartesianPosition.X = -currDP.velocityXraw;
		else
			pointToSend.Position.CartesianPosition.X = -(CONSTRAINT_X - currDP.tipLEDLoc[0]);
		if (CartYOn)
			pointToSend.Position.CartesianPosition.Y = -currDP.velocityYraw;
		else 
			pointToSend.Position.CartesianPosition.Y = -(CONSTRAINT_Y - currDP.tipLEDLoc[1]);
		if (CartZOn)
			pointToSend.Position.CartesianPosition.Z =	currDP.velocityZraw;
		else 
			pointToSend.Position.CartesianPosition.Z =  (CONSTRAINT_Z - currDP.tipLEDLoc[2]);
		
		
		
		if( !RotXOn || !RotYOn || !RotZOn)
		{
			currX = currDP.currentRot[0];
			currY = currDP.currentRot[1];
			currZ = currDP.currentRot[2];
			
			sendX = asin(cos(START_ROT_Y)*cos(START_ROT_Z)*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)) - cos(START_ROT_Y)*sin(START_ROT_Z)*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) + cos(currX)*cos(currY)*sin(START_ROT_Y))*(cos(currX)*sin(currZ) + cos(currZ)*sin(currX)*sin(currY)) + atan2(cos(START_ROT_Y)*sin(START_ROT_Z)*(cos(currX)*cos(currZ) - sin(currX)*sin(currY)*sin(currZ)) - cos(START_ROT_Y)*cos(START_ROT_Z)*(cos(currX)*sin(currZ) + cos(currZ)*sin(currX)*sin(currY)) + cos(currY)*sin(START_ROT_Y)*sin(currX), sin(START_ROT_Y)*sin(currY) + cos(START_ROT_Y)*cos(START_ROT_Z)*cos(currY)*cos(currZ) + cos(START_ROT_Y)*cos(currY)*sin(START_ROT_Z)*sin(currZ))*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)) + atan2(cos(START_ROT_Y)*cos(currX)*cos(currY)*sin(START_ROT_X) - (cos(START_ROT_X)*cos(START_ROT_Z) - sin(START_ROT_X)*sin(START_ROT_Y)*sin(START_ROT_Z))*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) - (cos(START_ROT_X)*sin(START_ROT_Z) + cos(START_ROT_Z)*sin(START_ROT_X)*sin(START_ROT_Y))*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)), (cos(START_ROT_Z)*sin(START_ROT_X) + cos(START_ROT_X)*sin(START_ROT_Y)*sin(START_ROT_Z))*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) + (sin(START_ROT_X)*sin(START_ROT_Z) - cos(START_ROT_X)*cos(START_ROT_Z)*sin(START_ROT_Y))*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)) + cos(START_ROT_X)*cos(START_ROT_Y)*cos(currX)*cos(currY))*cos(currY)*cos(currZ);
			
			sendY = asin(cos(START_ROT_Y)*cos(START_ROT_Z)*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)) - cos(START_ROT_Y)*sin(START_ROT_Z)*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) + cos(currX)*cos(currY)*sin(START_ROT_Y))*(cos(currX)*cos(currZ) - sin(currX)*sin(currY)*sin(currZ)) + atan2(cos(START_ROT_Y)*sin(START_ROT_Z)*(cos(currX)*cos(currZ) - sin(currX)*sin(currY)*sin(currZ)) - cos(START_ROT_Y)*cos(START_ROT_Z)*(cos(currX)*sin(currZ) + cos(currZ)*sin(currX)*sin(currY)) + cos(currY)*sin(START_ROT_Y)*sin(currX), sin(START_ROT_Y)*sin(currY) + cos(START_ROT_Y)*cos(START_ROT_Z)*cos(currY)*cos(currZ) + cos(START_ROT_Y)*cos(currY)*sin(START_ROT_Z)*sin(currZ))*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) - atan2(cos(START_ROT_Y)*cos(currX)*cos(currY)*sin(START_ROT_X) - (cos(START_ROT_X)*cos(START_ROT_Z) - sin(START_ROT_X)*sin(START_ROT_Y)*sin(START_ROT_Z))*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) - (cos(START_ROT_X)*sin(START_ROT_Z) + cos(START_ROT_Z)*sin(START_ROT_X)*sin(START_ROT_Y))*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)), (cos(START_ROT_Z)*sin(START_ROT_X) + cos(START_ROT_X)*sin(START_ROT_Y)*sin(START_ROT_Z))*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) + (sin(START_ROT_X)*sin(START_ROT_Z) - cos(START_ROT_X)*cos(START_ROT_Z)*sin(START_ROT_Y))*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)) + cos(START_ROT_X)*cos(START_ROT_Y)*cos(currX)*cos(currY))*cos(currY)*sin(currZ);
			
			sendZ = atan2(cos(START_ROT_Y)*cos(currX)*cos(currY)*sin(START_ROT_X) - (cos(START_ROT_X)*cos(START_ROT_Z) - sin(START_ROT_X)*sin(START_ROT_Y)*sin(START_ROT_Z))*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) - (cos(START_ROT_X)*sin(START_ROT_Z) + cos(START_ROT_Z)*sin(START_ROT_X)*sin(START_ROT_Y))*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)), (cos(START_ROT_Z)*sin(START_ROT_X) + cos(START_ROT_X)*sin(START_ROT_Y)*sin(START_ROT_Z))*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) + (sin(START_ROT_X)*sin(START_ROT_Z) - cos(START_ROT_X)*cos(START_ROT_Z)*sin(START_ROT_Y))*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)) + cos(START_ROT_X)*cos(START_ROT_Y)*cos(currX)*cos(currY))*sin(currY) - cos(currY)*asin(cos(START_ROT_Y)*cos(START_ROT_Z)*(sin(currX)*sin(currZ) - cos(currX)*cos(currZ)*sin(currY)) - cos(START_ROT_Y)*sin(START_ROT_Z)*(cos(currZ)*sin(currX) + cos(currX)*sin(currY)*sin(currZ)) + cos(currX)*cos(currY)*sin(START_ROT_Y))*sin(currX) + cos(currX)*cos(currY)*atan2(cos(START_ROT_Y)*sin(START_ROT_Z)*(cos(currX)*cos(currZ) - sin(currX)*sin(currY)*sin(currZ)) - cos(START_ROT_Y)*cos(START_ROT_Z)*(cos(currX)*sin(currZ) + cos(currZ)*sin(currX)*sin(currY)) + cos(currY)*sin(START_ROT_Y)*sin(currX), sin(START_ROT_Y)*sin(currY) + cos(START_ROT_Y)*cos(START_ROT_Z)*cos(currY)*cos(currZ) + cos(START_ROT_Y)*cos(currY)*sin(START_ROT_Z)*sin(currZ));
			
			sendX = sendX>0?sqrt(sendX):-sqrt(-sendX);
			sendY = sendY>0?sqrt(sendY):-sqrt(-sendY);
			sendZ = sendZ>0?sqrt(sendZ):-sqrt(-sendZ);
		}
		
		
		if (RotXOn)
			pointToSend.Position.CartesianPosition.ThetaX = currDP.thetaX;
		else
			pointToSend.Position.CartesianPosition.ThetaX = sendX;
		if (RotYOn)
			pointToSend.Position.CartesianPosition.ThetaY = currDP.thetaY;
		else
			pointToSend.Position.CartesianPosition.ThetaY = sendY;
		if (RotZOn)
			pointToSend.Position.CartesianPosition.ThetaZ = currDP.thetaZ;
		else 
			pointToSend.Position.CartesianPosition.ThetaZ = sendZ;
		
		if (GrasperOn)
		{
			pointToSend.Position.Fingers.Finger1 = 
			pointToSend.Position.Fingers.Finger2 = 
			pointToSend.Position.Fingers.Finger3 = currDP.fingersTold;
		}
		
		
		outputFile << -pointToSend.Position.CartesianPosition.X << "\t" << -pointToSend.Position.CartesianPosition.Y << "\t" << pointToSend.Position.CartesianPosition.Z << "\t";
		printDataPoint(&currDP, outputFile); //output to file
		
		result = (*MySendBasicTrajectory)(pointToSend); //send velocity to robot; how long of a process is this?
		if (result !=1)
		{
			printf("SendTrajectory:\t%d\n", result);
		}
		
		counter++;
		gettimeofday(&currentTime, NULL);
		
		sleepTime = (startOfProgramTime.tv_sec - currentTime.tv_sec)*1000000
					+ (startOfProgramTime.tv_usec - currentTime.tv_usec)
					+ (1000000/LOOP_FREQUENCY)*counter;
		
		if ( (sleepTime>0) && (sleepTime<1000000/LOOP_FREQUENCY) )
		{
			usleep(	sleepTime );
		}
	}//END MAIN LOOP
	
	keepGoing = false;
	
//	strcpy(parameterFilename,folder); 
//	strcat(parameterFilename,"/");
//	strcat(parameterFilename,timeBuffer);
//	strcat(parameterFilename,"_Parameters.txt");
//	std::ofstream parameterFile( parameterFilename );
//	printParameters(parameterFile); //output to file
//	parameterFile.close();
	
	//make the robot stop moving
	pointToSend.Position.CartesianPosition.X = 0.0f;
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
			result = stopStreaming(fd_dongle, device_iter);
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

	//close everything down
	
	//Turn off Kinova API
	(*MyCloseAPI)();
	printf ("Kinova API closed\n");
	
	//wait for the matlabThread to join; it should go pretty fast
	pthread_join( matlabThread, NULL);
 	printf ("Matlab thread joined\n");

	//wait for the readerThread to join; it should have quit once it got saw keepGoing == false
	pthread_join( readerThread, NULL);
	printf ("Reader thread joined\n");

	//wait for the teensyThread to join; it should have quit once it got saw keepGoing == false
	pthread_join( teensyThread, NULL);
	printf ("Teensy thread joined\n");
	
	//IMUS
	for (int device_iter = 0 + IMUoffsetValue; device_iter< NUMBER_OF_IMUS + IMUoffsetValue ; device_iter++)
	{
		do{
			result = getBatteryLevel(fd_dongle, device_iter);
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
		result = closeDevice (fd_dongle);
		if ( result < 0 ) 
		{
			printf ("Unable to close device; error: %d\n", result);
			k++;
			usleep(10000);
		}
	}while(result<0 && k<10);
	k=0;
	printf ("Device closed. fd_dongle: %d\n",fd_dongle);
	
	do{
		result = closeDevice (fd_teensy);
		if ( result < 0 ) 
		{
			printf ("Unable to close device; error: %d\n", result);
			k++;
			usleep(10000);
		}
	}while(result<0 && k<10);
	k=0;
	printf ("Device closed. fd_teensy: %d\n",fd_teensy);
	
	
	outputFile.close();

	printf ("Output file \"%s/VBM_Session_%dDOF_%d.txt\" closed.\n", argv[1], NUM_OF_DOFs, q);
	
	printf ("Exiting program\n");
	
	return EXIT_SUCCESS;
}

