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


#include "DataStorage.h"

//g++ -std=c++11 -pthread Attempt1_7.cpp -ldl -o Attempt1_7 -lboost_iostreams -lboost_system -lboost_filesystem


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
#include "projection12.h"

#include "configFileReader.h"

#include "globalVariablesAndConstants4.h"
//#define NUMBER_OF_IMUS 4
//#define PI 3.14159265359


//#define LOOP_FREQUENCY = 50 //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes


//Global Variables
//bool keepGoing = true; //the loops will check this value
//int fd_ = 0; // file handle for dongle stream. must be done globally for threading


// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_handler(int signum)
{
	printf("Caught signal %d\n",signum);
	keepGoing = false;
}





#include "readerthread3.h"



#include <unistd.h>
#include <sys/time.h>


int main(int argc, char * argv[])
{
   	// Register signal and signal handler
   	signal(SIGINT, signal_handler);
   	
   	//char c;
   	
   	//initialize everything
	//Read in Matrix from a file or I guess we can just hard code it for now
	//std::string ifilename;
    //std::cout << "Please enter name of file containing matrix: \n";
    //std::getline(std::cin, ifilename);
	
	for (int i = 0; i<9; i++)
	{
		octagonCorners.push_back(std::make_pair(octagonCornerX[i], octagonCornerY[i]));
	}
	
	
	
	
	if (argc !=2)
	{
		printf("Correct program call is \"./ProgramName xyz\" where \"xyz\" are participant initials\n");
		return 1;
	}
	
	const char configFileName[] = "ConfigFileFullMain.txt";
	
	configFileReader( configFileName );
	
	
	
	const float FREQUENCY_RATIO = (float)(LOOP_FREQUENCY/GRAPHING_FREQUENCY);
	
	radiusOfGoal *= goalSizeMult;
	
	
/*	
	if (argc > 3 || argc < 2)
	{
		printf("Correct program call is \"./ProgramName xyz [goalSizeMult]\" where \"xyz\" are participant initials and [goalSizeMult] is an optional scaling factor for the goalSize\n\n");
		return 1;
	}
	
	float goalSizeMult = 1.0;
	if (argc == 3 )
	{
		goalSizeMult = atof(argv[2]);
		if (goalSizeMult <= 0.0)
		{
			printf("Invalid goalSizeMult. It must be a number that is greater than 0.0\n\n");
			return 1;
		}
		radiusOfGoal *= goalSizeMult;
	}
	else
	{
		printf("Use \"./ProgramName xyz [goalSizeMult]\" with [goalSizeMult] as an optional scaling factor to edit the goalSize\n\n");
	}
*/	
	
	
	
	
	
	
	
	char folder[128];
	char matrixFilename[128];
	char outputFilename[128];
	char c;
	
	strcpy(folder,"/home/aspelun1/Documents/BOMI_Data/"); 
	strcat(folder,argv[1]);
	
	
	struct stat sb;

	if (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		printf("\n\nFolder exists.\nChecking for \"Matrix_%s.txt\"\n",argv[1]);
		strcpy(matrixFilename,folder); 
		strcat(matrixFilename,"/Matrix_");
		strcat(matrixFilename,argv[1]);
		strcat(matrixFilename,".txt");
		
		if (stat(matrixFilename, &sb) != 0)
		{
			printf("File \"Matrix_%s.txt\" does not exist. Please copy it into folder.\nQuitting.\n", argv[1]);
			return 1;
		}
		else
		{
			printf("File \"Matrix_%s.txt\" found.\nChecking for latest result file.\n", argv[1]);
			
			//at this point I am going to break out of this insane if-tree as all other branches end in a return statement.
		}
	}
	else
	{
		printf("Folder \"%s\" does not exist. Would you like for it to be created? -y\n", argv[1]);
		c = getchar();
		if (c == 'y' || c == 'Y')
		{
			mkdir(folder, 0777);
			printf("Folder \"%s\" created. Please copy matrix file into it.\n", folder);
			return 1;
		}
		else
		{
			printf("Did not input \"y\". Quitting.\n");
        	return 1;
        }
		return 1; //just in case?
	}
	
	const char* epochTypes[] =	{
								"Test11",
								"Practice1",
								"Practice2",
								"Practice3",
								"Practice4",
								"Test12",
								"Practice5",
								"Practice6",
								"Practice7",
								"Practice8",
								"Test13"
								};
	
	int q;
	for (q = 0; q<11; q++)
	{
		strcpy(outputFilename,folder); 
		strcat(outputFilename,"/");
		strcat(outputFilename,epochTypes[q]);
		strcat(outputFilename,".txt");
		
		if (stat(outputFilename, &sb) != 0)
		{
			if (q == 0)
			{
				printf("No previous epoch result files found.\n");
			}
			else
			{
				printf("Epoch result file \"%s.txt\" is last available file.\n", epochTypes[q-1]);
			}
			break;
		}
	}
	
	if (q==11)
	{
		printf("Result files for all epochs found.\n");
		return 1;
	}
	
	printf("Will create file \"%s.txt\" for next test.\nDo you wish to continue? -y\n", epochTypes[q]);
	
	c = getchar();
	if (!(c == 'y' || c == 'Y'))
	{
		printf("See you next time!\n");
		return 1;
	}
	
	dataPoint currDP; //currentDataPoint
		
	currDP.epochType = 1; //this will get changed later to be dependent on input
	switch(q)
	{
		case 0:	currDP.epochType = 11; break;
		
		case 1: currDP.epochType = 1; break;
		case 2: currDP.epochType = 2; break;
		case 3: currDP.epochType = 3; break;
		case 4: currDP.epochType = 4; break;
		
		case 5: currDP.epochType = 12; break;
		
		case 6: currDP.epochType = 5; break;
		case 7: currDP.epochType = 6; break;
		case 8: currDP.epochType = 7; break;
		case 9: currDP.epochType = 8; break;
		
		case 10: currDP.epochType = 13; break;
		
		default: printf("Got to the default of the epochType switch statement.....\n"); return 1;
	}
	
	printf("epochType: %d\n",currDP.epochType);
	
	
	
	
	
	
	//this is simply put here so that the show gets on the road asap by thinking
	//that the first goal was entered a long time ago and therefore "immediately"
	//making a new one show up
	gettimeofday(&currDP.goalArrivalTime,NULL);
	
	
	
	
	
	
	
	double multMatrix[2][8];
	double variance[2];
	
	//std::ifstream infile(ifilename);
	std::ifstream infile(matrixFilename);
	
	
	//double value;
	for (int col = 0; col <8; col ++)
	{
		for (int row = 0; row <2; row ++)
		{	
			infile >> multMatrix[row][col];
		}
	}
	
	infile >> variance[0];
	infile >> variance[1];
	
	variance[0] = sqrt(variance[0]);
	variance[1] = sqrt(variance[1]);
	
	infile.close();
		
	// I will want to input this file name as argv[1]. See the following file for code:
	//  /home/aspelun1/Documents/Testing_functions/initializeMatrixFromTextFile/initializeMatrix.cpp




	fd_ = openDevice();
	if (( fd_ ) < 0 ) 
	{
		printf ("Unable to open file");
		return 1;
	}
	//printf("fd_ = %d\n",fd_);



	//std::vector<dataPoint> dataStorageVector;
	
	
	
	
   	
   	Gnuplot gp;
   	///graphing setup
	srand (static_cast <unsigned> (time(0)));
	//double Xcurrent, Ycurrent, Xgoal, Ygoal, Xdelta, Ydelta;
	//currentX = currentY = Xdelta = Ydelta = 0;
	
	initializeGoalOrder(&currDP);
		
	currDP.goalX = centerX;
	currDP.goalY = centerY;
	
	
	
	//std::string ofilename;
	//std::cout << "Please enter desired output data file name: \n";
	//ofilename = "output.txt";
	
//	std::getline(std::cin, ofilename);
//	std::ofstream outputFile( ofilename );
	
//	std::ofstream outputFile ("output.txt");
	
/*	if (outputFile.is_open())
	{
		outputFile << "GoalTime\tXcurrent\tYcurrent\tXvel\tYvel" << "\n";
	}
*/ 
//	outputFile.close();
	
	//projection2.h
	//graph ( gp, 0, 0, Xgoal, Ygoal, 0, 0, outputFile, &startOfProgramTime, &startOfCurrentGoalTime);
	
	//projection.h
	//graph ( gp, 0,0, Xgoal, Ygoal, 0,0);
	
	//projection3.h
	graphSetup( gp);
	graph ( gp, &currDP);
	
	
	
	
	//Time_Stamp(ofilename);
	
	int result; //used for error checking
	
	//store the orientation values of the IMUs
	//orientation orient[NUMBER_OF_IMUS];
	
	
	//float maxSpeed = 0.30f; //never used
	
	
	
	
	
	
	
	
	//initialize IMUs
	//int fd_; // file handle for dongle stream must be done globally
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
		pointToSend.Limitations.speedParameter1 = 0.5;
		pointToSend.Limitations.speedParameter2 = 0.7;
		
		
		//This is the starting position
		pointToSend.Position.CartesianPosition.X = -currDP.goalX + END_EFFECTOR_OFFSET_X; //offset for hand orientation
		pointToSend.Position.CartesianPosition.Y = Y_PLANE_VALUE;
		pointToSend.Position.CartesianPosition.Z = currDP.goalY - END_EFFECTOR_OFFSET_Z;
		pointToSend.Position.CartesianPosition.ThetaX = PI/2;
		pointToSend.Position.CartesianPosition.ThetaY = -PI/2+0.4f;
		pointToSend.Position.CartesianPosition.ThetaZ = PI/2+0.2f;
		
		//For some reason these values work because the status.RobotType = NULL rather than 0 for JACO
		//If this extends fingers too far or too little, change these values to orders of magintude smaller
		pointToSend.Position.Fingers.Finger1 = 6000.0f;
		pointToSend.Position.Fingers.Finger2 = 6000.0f;
		pointToSend.Position.Fingers.Finger3 = 6000.0f; //does this go more?
		
		
		printf( "Sending starting position.\n");
		
		printf("\n\nPlease position graph in correct place in next 4+ seconds.\n\n");
		
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
		
		result = (*MySendBasicTrajectory)(pointToSend);
	}
	else
	{
		printf( "Error detected, leaving the example...\n" );
		return 1;
	}
	



	

	


	graph ( gp, &currDP);
	//usleep(2500000); // allow the user to see the battery levels but realize that the IMUs have already been tared.
	printf("Program will tare IMUs and start streaming. Make sure user is in base position and then enter 'y' to continue/'q' to quit.\n");
	do{
		c = getchar(); //this is included from way up there
		if (c == 'q')
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
			
			gp << "quit" << std::endl;
			
			return 1;;
		}
	}while(c != 'y');
	
	
	
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
			
			gp << "quit" << std::endl;
			
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
			
			gp << "quit" << std::endl;
			
			return 1;
		}
		k=0;
		printf ("Streaming started for IMU #%d\n", device_iter);
	}
	
	
	//start the new thread with the reader from the dongle
	pthread_t readerThread;
	int threadCreateReturn;
	
	threadCreateReturn = pthread_create( &readerThread, NULL, readerFunction, NULL);
	
	
	
	
	float xVel, zVel, magnitude;//, tempVelocityX;
	magnitude = 1.0;
	//float scaledxVel, scaledzVel;
	
	struct timeval startOfProgramTime, currentTime;
	//float programTime;
	currDP.programTime = 0.0;
	
	unsigned long int counter = 0;
	unsigned long int sleepTime = 0;
	
	const float inverseVariance1 = 1.0/variance[0];
	const float inverseVariance2 = 1.0/variance[1];
	
	usleep(150000); //to avoid the E+17 values that randomly appear
	
	std::ofstream outputFile( outputFilename );
//	printDataHeader(outputFile); //removed as it makes later data processing more difficult
//	outputFile.close();
	
	gettimeofday(&startOfProgramTime, NULL);
	// loop everything:
	// get current position (takes ~3 milliseconds....) !
	// graph the initial points (this can do the dist(goal,current) checking)
		// set the initial point to (-0.12,0.5) so the initial check cause a random point to start
	// get the values from the dongle reader ! not applicable to this section !
	// multiply the matrix by the applicable values to get the two velocities !
	// send the velocities to the Kinova Arm !
	while(keepGoing) //errors should break out of the loop and set keepGoing to false for the reader thread
   	{
		gettimeofday(&currentTime, NULL);
		currDP.programTime  = (currentTime.tv_sec - startOfProgramTime.tv_sec)*1.0 + (currentTime.tv_usec - startOfProgramTime.tv_usec)/1000000.0;
		
		result = (*MyGetCartesianPosition)(data); //about 2ms
		
		
		//graph ( gp, -1.0*data.Coordinates.X, data.Coordinates.Z, Xgoal, Ygoal, -0.0625*xVel/magnitude, 0.0625*zVel/magnitude);
		
		currDP.currentX = -1.0*data.Coordinates.X + END_EFFECTOR_OFFSET_X; //offset for hand orientation
		currDP.currentY =  1.0*data.Coordinates.Z + END_EFFECTOR_OFFSET_Z; //offset for hand orientation
		
		
		//error correction for moving from the Y Plane
		pointToSend.Position.CartesianPosition.Y = (Y_PLANE_VALUE - data.Coordinates.Y)*10.0;
		
		//have I reached the goal, if so, change
		goalCheck3(&currDP, currentTime); //still have to decide when this should happen
		
		if (fmod(counter,FREQUENCY_RATIO) == 0.0)
		{
			graph (gp, &currDP);
		}
		
		
		xVel = zVel = 0.0;
		for (int i = 0; i < NUMBER_OF_IMUS; i++)
		{
			pthread_mutex_lock( &orient[i].mutex );
			//printf("id:%d\troll:%f\tpitch%f\n",i,orient[i].roll,orient[i].pitch);
			currDP.IMU_values[i*2]   = orient[i].roll*-1.0;
			currDP.IMU_values[i*2+1] = orient[i].pitch;
			pthread_mutex_unlock( &orient[i].mutex );
			
			xVel += (float)(multMatrix[0][i*2]*currDP.IMU_values[i*2] + multMatrix[0][i*2+1]*currDP.IMU_values[i*2+1]);
			zVel += (float)(multMatrix[1][i*2]*currDP.IMU_values[i*2] + multMatrix[1][i*2+1]*currDP.IMU_values[i*2+1]);
		}
		
		//printf("raw\txVel: %7.4f\tzVel: %7.4f\n", xVel,zVel);
		currDP.velocityXraw = xVel;
		currDP.velocityYraw = zVel;
		
		//Here is where I need to scale the values according to the variances and a factor
		xVel = xVel * inverseVariance1 * velocityScaleFactor;
		zVel = zVel * inverseVariance2 * velocityScaleFactor;
		
		if ((abs(xVel) > 50.0) || isinf(xVel) || isnan(xVel))
		{
			printf("xVel: %f",xVel);
			xVel = 0.0;
		}
		if ((abs(zVel) > 50.0) || isinf(zVel) || isnan(zVel))
		{
			printf("zVel: %f",zVel);
			zVel = 0.0;
		}
		
		
		
		
		
		magnitude = sqrt(xVel*xVel + zVel*zVel);
		//printf("mag: %7.4f\t", magnitude);
		
		//printf("xVel:%7.4f\tzVel:%7.4f\tmag:%7.4f\n",xVel, zVel, magnitude);
		if ( magnitude < bufferSize)
		{
			currDP.velocityX = 0.0f; //these values are the result of the matrix
			currDP.velocityY = 0.0f;
			//printf("xVel: %7.4f\tzVel: %7.4f but sending 0s\n", xVel, zVel);
			if (currDP.cursorStatus < 1 ) // so that it doesn't overwrite a 1 or 2
				currDP.cursorStatus = -1; 
		}
		else
		{
			currDP.velocityX = -1.0*(xVel-bufferSize*xVel/magnitude); //can't divide by zero as magnitude would be less than bufferSize 
			currDP.velocityY = zVel-bufferSize*zVel/magnitude; 
			if (currDP.cursorStatus < 1 ) // so that it doesn't overwrite a 1 or 2
				currDP.cursorStatus = 0;
			//printf("xVel: %7.4f\tzVel: %7.4f\n", xVel, zVel);
		//} gonna include all this other stuff in this else statement
		
		
			currDP.againstBorder = false;
		
		
		
		
		
			//Don't allow robot to move outside of boundaries as defined in globalVariablesAndConstants.h
			//Can't be more than one non-adjacent boundary so why test twice
			//The idea is to remove the component of the velocity that is heading outwards 
			//	using basic grahm-schmidt with the normal unit vector to the edge 
			//If the robot is heading in that direction(dot product with normal vector>=0). Location && intent
		
			//we could have fun marginally improving the program by figuring out which sides are come in contact with most and test for those first but that's essentially a waste of human time and the computer doesn't care
		
		
			
		
						//**********	VERSION 1 ***************//
/*			//Start with the top right corner
			if (currDP.currentY >= ( INTERCEPT_TR - currDP.currentX ) && (currDP.velocityX + currDP.velocityY) >= 0.0)
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
			//then the bottom right corner
			else if (currDP.currentY <= ( INTERCEPT_BR + currDP.currentX ) && (currDP.velocityX - currDP.velocityY) >= 0.0)
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
			//then the bottom left corner
			else if (currDP.currentY <= ( INTERCEPT_BL - currDP.currentX ) && (currDP.velocityX + currDP.velocityY) <= 0.0) //notice the flipped sign on the velocity comparison
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
			//then the top left corner
			else if (currDP.currentY >= ( INTERCEPT_TL + currDP.currentX ) && (-currDP.velocityX + currDP.velocityY) >= 0.0)
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
			
			
			
			//Now with left side
			 if (currDP.currentX <= X_ROBOT_MIN && currDP.velocityX <= 0.0)
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
			//then the right side
			else if (currDP.currentX >= X_ROBOT_MAX && currDP.velocityX >= 0.0)
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
			//then the bottom
			else if (currDP.currentY <= Y_ROBOT_MIN && currDP.velocityY <= 0.0)
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
			//then the top
			else if (currDP.currentY >= Y_ROBOT_MAX && currDP.velocityY >= 0.0)
			{
				currDP.velocityY = 0.0;
				currDP.velocityX = 0.0;
				currDP.againstBorder = true;
			}
*/			
			
			
			//**********	VERSION 2 ***************//
			//Start with the top right corner
			if (currDP.currentY >= ( INTERCEPT_TR - currDP.currentX ) )
			{
				currDP.againstBorder = true;
				if(currDP.velocityX + currDP.velocityY >= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			//then the bottom right corner
			else if (currDP.currentY <= ( INTERCEPT_BR + currDP.currentX ) )
			{
				currDP.againstBorder = true;
				if(currDP.velocityX - currDP.velocityY >= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			//then the bottom left corner
			else if (currDP.currentY <= ( INTERCEPT_BL - currDP.currentX ) )
			{
				currDP.againstBorder = true;
				if(-currDP.velocityX - currDP.velocityY >= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			//then the top left corner
			else if (currDP.currentY >= ( INTERCEPT_TL + currDP.currentX ) )
			{
				currDP.againstBorder = true;
				if(-currDP.velocityX + currDP.velocityY >= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			
			
			
			//Now with left side
			 if (currDP.currentX <= X_ROBOT_MIN )
			{
				currDP.againstBorder = true;
				if(currDP.velocityX <= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			//then the right side
			else if (currDP.currentX >= X_ROBOT_MAX )
			{
				currDP.againstBorder = true;
				if(currDP.velocityX >= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			//then the bottom
			else if (currDP.currentY <= Y_ROBOT_MIN )
			{
				currDP.againstBorder = true;
				if(currDP.velocityY <= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			//then the top
			else if (currDP.currentY >= Y_ROBOT_MAX )
			{
				currDP.againstBorder = true;
				if(currDP.velocityY >= 0.0)
				{
					currDP.velocityY = 0.0;
					currDP.velocityX = 0.0;
				}
			}
			
			
			
		
		}
		
		pointToSend.Position.CartesianPosition.X = -currDP.velocityX;	
		pointToSend.Position.CartesianPosition.Z =  currDP.velocityY;
		
		printDataPoint(&currDP, outputFile); //output to file
		
		(*MySendBasicTrajectory)(pointToSend); //send velocity to robot; how long of a process is this?
		
		counter++;
		
		if (counter%LOOP_FREQUENCY == 0)
		{
			printf("%ld",counter);
			for (int i = 0; i < NUMBER_OF_IMUS; i++)
			{
				if (currDP.IMU_values[i*2] == 0.0 && currDP.IMU_values[i*2+1] == 0.0)
				{
					printf("\t\tIMU %d reported 0s",i);
					//Here is an idea to try restarting the streaming if the user is very close to home position
					if (sqrt(currDP.velocityXraw * currDP.velocityXraw + currDP.velocityYraw * currDP.velocityYraw) <= .05)
					{
						result = startStreaming(fd_, i);
						printf("\t\tSent restart signal to IMU %d",i);
					}
				}
			}
			printf("\n");
		}
		
		
		
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
		
  	}
   	
   	
   	pointToSend.Position.CartesianPosition.X = 0.0f; //make the robot stop moving
	pointToSend.Position.CartesianPosition.Z = 0.0f;
	(*MySendBasicTrajectory)(pointToSend);
	//printf("xVel: 0\tzVel: 0\n");
   	
   	
   	//Putting the IMU stop_streaming portion hear for hope of error fix, the reasoning being unknown
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

   	// move the hand backwards just slightly so it doesn't hit the screen on reset. Still need to figure out time
   	pointToSend.Position.CartesianPosition.Y = 0.5f;
   	for (int w = 0; w<200; w++)
   	{
   		usleep(5000);
   		(*MySendBasicTrajectory)(pointToSend);
	}

   	
   	//close everything down
   	
   	//graph
   	gp << "quit" << std::endl;
   	
   	
   	//Turn off Kinova API
	(*MyCloseAPI)();
	printf ("Kinova API closed\n");
   	
   	//wait for the readerThread to join; it should have quit once it got saw keepGoing == false
   	pthread_join( readerThread, NULL);
   	
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
	
	
//	std::ofstream outputFile( ofilename );
//	printData2(dataStorageVector, outputFile);
//	result = seems like that's not valid or something
	outputFile.close();
/*	do{
		result = closeDevice (fd_);
		if ( result < 0 ) 
		{
			printf ("Unable to close output file; error: %d\n", result);
			k++;
			usleep(10000);
		}
	}while(result<0 && k<10);
	//k=0;
*/
	printf ("Output file \"%s.txt\" closed.\n",epochTypes[q]);
	
	printf ("Exiting program\n");
   	
   	return EXIT_SUCCESS;
}

