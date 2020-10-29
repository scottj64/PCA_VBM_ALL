/* compile with
	g++ -std=c++11 FEMControl_7DOF_twirlyWhirly.cpp -ldl -o FEMControl_7DOF_twirlyWhirly
*/

/* run with
	./FEMControl_7DOF_twirlyWhirly
*/

/*
	close with Ctrl-C, if that somehow fails, Ctrl-Z
*/

// When I run the robot, I have it to my left with the plugs facing backwards.
// +X is to the right, +Y is forward, and +Z is up


//Kinova
#include <dlfcn.h>
#include <vector>

#include <math.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <csignal>
#include <cstring>
#include <ctime>
#include <fstream>
#include <unistd.h>


#include "supplementalVariablesFunctions.h"

#include "KinovaTypes.h"
#include "Kinova.API.UsbCommandLayerUbuntu.h"
#include "Kinova.API.CommLayerUbuntu.h"


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

	char outputFilename[128];
	char c;

	
	strcpy(outputFilename,"./BOMI_Data/swa/");
	strcat(outputFilename,timeBuffer);
	strcat(outputFilename,"_Session.txt");
	
	printf("Will create file %s_Session.txt\" for next test.\nDo you wish to continue? -y\n", timeBuffer);
	
	c = getchar();
	if (!(c == 'y' || c == 'Y'))
	{
		printf("See you next time!\n");
		return 1;
	}
	
	dataPoint currDP; //currentDataPoint
		

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
		pointToSend.Position.CartesianPosition.X = -StartX; //offset for hand orientation
		pointToSend.Position.CartesianPosition.Y = -StartY;
		pointToSend.Position.CartesianPosition.Z =  StartZ;
		pointToSend.Position.CartesianPosition.ThetaX = StartRotX;//PI/2;
		pointToSend.Position.CartesianPosition.ThetaY = StartRotY;//-PI/2;//+0.4f;
		pointToSend.Position.CartesianPosition.ThetaZ = StartRotZ;//PI;//+0.2f;
		
		//For some reason these values work because the status.RobotType = NULL rather than 0 for JACO
		//If this extends fingers too far or too little, change these values to orders of magintude smaller
		pointToSend.Position.Fingers.Finger1 = 3000.0f;
		pointToSend.Position.Fingers.Finger2 = 3000.0f;
		pointToSend.Position.Fingers.Finger3 = 3000.0f; //does this go more?
		
		
		printf( "Sending starting position over next 6 seconds.\n\n");
		
		result = (*MySendBasicTrajectory)(pointToSend);
		
		int y= 6*100; 
		for (int x = 0; x<y; x++)
		{
			usleep(10000);
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
	
	
	printf("Enter 'y' to continue/'q' to quit.\n");
	do{
		c = getchar(); //this is included from way up there
		if (c == 'q')
		{
			keepGoing = false;
			//Turn off Kinova API
			(*MyCloseAPI)();
			printf ("Kinova API closed\n");
		
			return 1;;
		}
	}while(c != 'y');
	
	
	struct timeval startOfProgramTime, currentTime;
	currDP.programTime = 0.0;
	
	unsigned long int counter = 0;
	unsigned long int sleepTime = 0;
	
	std::ofstream outputFile( outputFilename );
//	printDataHeader(outputFile); //removed as it makes later data processing more difficult

	
//	usleep(250000); //to avoid the E+17 values that randomly appear
	
	printf(".\n");
	printf("Starting loop.\n");
	gettimeofday(&startOfProgramTime, NULL);


	while(keepGoing) //errors should break out of the loop and set keepGoing to false for the reader thread
	{
		gettimeofday(&currentTime, NULL);
		currDP.programTime  = (currentTime.tv_sec - startOfProgramTime.tv_sec)*1.0 + (currentTime.tv_usec - startOfProgramTime.tv_usec)/1000000.0;
		
		result = (*MyGetCartesianPosition)(data); //about 2ms
		if (result != 1)
		{
			printf("GetPosition:\t%d\n", result);
		}
		
		currDP.currentLoc[0] = -data.Coordinates.X;
		currDP.currentLoc[1] = -data.Coordinates.Y;
		currDP.currentLoc[2] =	data.Coordinates.Z;
		
		currDP.currentRot[0] =	data.Coordinates.ThetaX;
		currDP.currentRot[1] =  data.Coordinates.ThetaY;
		currDP.currentRot[2] =  data.Coordinates.ThetaZ;

		currDP.fingerSVal = (data.Fingers.Finger1 + data.Fingers.Finger2 + data.Fingers.Finger3) / 3;
		

		
//		follow a circle
		currDP.rawCartVel[0] = (CenterX + 0.1*cos(currDP.programTime*0.5) - currDP.currentLoc[0]);
		currDP.rawCartVel[1] = (StartY - currDP.currentLoc[1]);
		currDP.rawCartVel[2] = (CenterZ + 0.1*sin(currDP.programTime*0.5) - currDP.currentLoc[2]);

//		twist the wrist
		currDP.rawRotVel[0] = 0;
		currDP.rawRotVel[1] = 0;
		currDP.rawRotVel[2] = -0.2;
		
//		open and close the fingers
		currDP.fingersTold = (3000.0 + 2000.0*sin(currDP.programTime*0.5) - currDP.fingerSVal) * fingerGain;
		
		
		//Don't go past boundaries
		if((currDP.currentLoc[1] - Y_ROBOT_MIN) <= 0.0 && currDP.rawCartVel[1] <= 0.0)
		{
			currDP.rawCartVel[1] = 0.0;
		}
		if((currDP.currentLoc[2] - Z_ROBOT_MIN) <= 0.0 && currDP.rawCartVel[2] <= 0.0)
		{
			currDP.rawCartVel[2] = 0.0;
		}
		
		//If it does go past boundaries, come back
		if((currDP.currentLoc[1] - Y_ROBOT_MIN) <= -0.025)
		{
			currDP.rawCartVel[1] = Y_ROBOT_MIN - currDP.currentLoc[1];
		}
		if((currDP.currentLoc[2] - Z_ROBOT_MIN) <= -0.02)
		{
			currDP.rawCartVel[2] = Z_ROBOT_MIN - currDP.currentLoc[2];
		}
		
		pointToSend.Position.CartesianPosition.X = -currDP.rawCartVel[0];	
		pointToSend.Position.CartesianPosition.Y = -currDP.rawCartVel[1];
		pointToSend.Position.CartesianPosition.Z =  currDP.rawCartVel[2];
		
		
		pointToSend.Position.CartesianPosition.ThetaX = currDP.rawRotVel[0];
		pointToSend.Position.CartesianPosition.ThetaY = -currDP.rawRotVel[1];
		pointToSend.Position.CartesianPosition.ThetaZ = currDP.rawRotVel[2];
		
		pointToSend.Position.Fingers.Finger1 = 
		pointToSend.Position.Fingers.Finger2 = 
		pointToSend.Position.Fingers.Finger3 = currDP.fingersTold;
					
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
				  +	(1000000/LOOP_FREQUENCY)*counter;
		
		if ( (sleepTime>0) && (sleepTime<1000000/LOOP_FREQUENCY) )
		{
			usleep(	sleepTime );
		}
		
  	}//END MAIN LOOP

	pointToSend.Position.CartesianPosition.X = 0.0f; //make the robot stop moving
	pointToSend.Position.CartesianPosition.Y = 0.0f;
	pointToSend.Position.CartesianPosition.Z = 0.0f;
	
	pointToSend.Position.CartesianPosition.ThetaX = 0.0f;
	pointToSend.Position.CartesianPosition.ThetaY = 0.0f;
	pointToSend.Position.CartesianPosition.ThetaZ = 0.0f;
	
	pointToSend.Position.Fingers.Finger1 = pointToSend.Position.Fingers.Finger2 = pointToSend.Position.Fingers.Finger3 = 0.0;
	
	(*MySendBasicTrajectory)(pointToSend);

	usleep(500000);
	
	//Turn off Kinova API
	(*MyCloseAPI)();
	printf ("Kinova API closed\n");

	outputFile.close();

	printf ("Output file %s_Session.txt\" closed.\n", timeBuffer);

	printf ("Exiting program\n");
	
	return EXIT_SUCCESS;
}

