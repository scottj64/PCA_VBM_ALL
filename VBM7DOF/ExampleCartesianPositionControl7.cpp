
//#include <iostream>
#include <dlfcn.h>
#include "KinovaTypes.h"
#include "Kinova.API.UsbCommandLayerUbuntu.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define PI 3.14159265359

//g++ ExampleCartesianPositionControl7.cpp -ldl -o ExCartPosContr7
 

int main()
{
	void * APIhandle;

	bool canExecuteProgram = false;
	CartesianPosition data;

	int result;

	int (*MyInitAPI)();
	int (*MyCloseAPI)();
	int (*MyStartControlAPI)();

	//This function get the actual angular command of the robot.
	int (*MyGetQuickStatus)(QuickStatus &);
	int (*MySendBasicTrajectory)(TrajectoryPoint command);
	int (*MySendAdvanceTrajectory)(TrajectoryPoint command);
	int (*MyMoveHome)();
	int (*MyInitFingers)();
	int (*MyGetCartesianCommand)(CartesianPosition &Response);
	int (*MySetCartesianControl)();
	int (*MyGetCartesianPosition)(CartesianPosition &);

	//We load the library (Under Windows, use the function LoadLibrary)
	APIhandle = dlopen("Kinova.API.USBCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);

	if(APIhandle != NULL)
	{
		MyInitAPI = (int (*)()) dlsym(APIhandle,"InitAPI");
		MyCloseAPI = (int (*)()) dlsym(APIhandle,"CloseAPI");
		MyGetQuickStatus = (int (*)(QuickStatus &)) dlsym(APIhandle,"GetQuickStatus");
		MyStartControlAPI = (int (*)()) dlsym(APIhandle,"StartControlAPI");
		MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendBasicTrajectory");
		MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(APIhandle,"SendAdvanceTrajectory");
		MyMoveHome = (int (*)()) dlsym(APIhandle,"MoveHome");
		MyInitFingers = (int (*)()) dlsym(APIhandle,"InitFingers");
		MyGetCartesianCommand = (int (*)(CartesianPosition&)) dlsym(APIhandle,"GetCartesianCommand");
		MySetCartesianControl = (int (*)()) dlsym(APIhandle,"SetCartesianControl");
		MyGetCartesianPosition = (int (*)(CartesianPosition &)) dlsym(APIhandle,"GetCartesianPosition");

		//We validate that all the functions were loaded corectly.
		if((MyInitAPI == NULL) || (MyCloseAPI == NULL) || (MyGetQuickStatus == NULL) ||
		   (MyStartControlAPI == NULL) || (MySendBasicTrajectory == NULL) ||
		   (MySendBasicTrajectory == NULL))
		{
			printf("Can't load all the functions from the library.\n");
			return 1;
		}
		else
		{
			//Init the API
			result = (*MyInitAPI)();

			printf("result = %d\n",result);

			if(result == NO_ERROR_KINOVA)
			{
				canExecuteProgram = true;
			}
		}
	}
	else
	{
		printf("Unable to load the API.\n");
		return 1;
	}

	if(canExecuteProgram)
	{
		QuickStatus status;
		(*MyGetQuickStatus)(status);

		(*MyMoveHome)();
		(*MyInitFingers)();

		CartesianPosition actualPosition;
		(*MyGetCartesianCommand)(actualPosition);

		TrajectoryPoint pointToSend;
		TrajectoryPoint startPoint;

		pointToSend.InitStruct();
		
		
		pointToSend.Position.HandMode = POSITION_MODE;
		pointToSend.Position.Type = CARTESIAN_POSITION;
		
		pointToSend.Position.CartesianPosition.X = -0.28f;
		pointToSend.Position.CartesianPosition.Y = -0.36f;
		pointToSend.Position.CartesianPosition.Z = 0.540f;
		pointToSend.Position.Fingers.Finger1 = 6000.0f;
		pointToSend.Position.Fingers.Finger2 = 6000.0f;
		pointToSend.Position.Fingers.Finger3 = 6000.0f;
		
		pointToSend.Position.CartesianPosition.ThetaX = PI/2;
		pointToSend.Position.CartesianPosition.ThetaY = -PI/2+0.4f;
		pointToSend.Position.CartesianPosition.ThetaZ = PI/2+0.2f;
		
		memcpy(&startPoint, &pointToSend,sizeof(pointToSend));
		
		printf("Sending position and fingers.\n");
		for (int r = 0; r<10;r++){
			(*MySendAdvanceTrajectory)(pointToSend);
			usleep(250000);
		}
		
		FILE * pfile;
		pfile = fopen ("pose.txt","w");
		char c = '\0';
	
//		do{
//			c = getchar();
//					
//			switch(c)
//			{
//				case 'm': (*MyMoveHome)(); break;
//				case 'b': memcpy(&pointToSend, &startPoint,sizeof(pointToSend)); break;
//				case 'q': pointToSend.Position.CartesianPosition.ThetaX +=.1; break;
//				case 'a': pointToSend.Position.CartesianPosition.ThetaX -=.1; break;
//				case 'w': pointToSend.Position.CartesianPosition.ThetaY +=.1; break;
//				case 's': pointToSend.Position.CartesianPosition.ThetaY -=.1; break;
//				case 'e': pointToSend.Position.CartesianPosition.ThetaZ +=.1; break;
//				case 'd': pointToSend.Position.CartesianPosition.ThetaZ -=.1; break;
//				
//				case 'r': pointToSend.Position.CartesianPosition.X +=.01; break;
//				case 'f': pointToSend.Position.CartesianPosition.X -=.01; break;
//				case 't': pointToSend.Position.CartesianPosition.Y +=.01; break;
//				case 'g': pointToSend.Position.CartesianPosition.Y -=.01; break;
//				case 'u': pointToSend.Position.CartesianPosition.Z +=.01; break;
//				case 'j': pointToSend.Position.CartesianPosition.Z -=.01; break;
//				
//				case 'i': pointToSend.Position.Fingers.Finger1 +=100; break;
//				case 'k': pointToSend.Position.Fingers.Finger1 -=100; break;
//				case 'o': pointToSend.Position.Fingers.Finger2 +=100; break;
//				case 'l': pointToSend.Position.Fingers.Finger2 -=100; break;
//				case '[': pointToSend.Position.Fingers.Finger3 +=100; break;
//				case ';': pointToSend.Position.Fingers.Finger3 -=100; break;
//				
//				case 'p':
//					(*MyGetCartesianPosition)(data); 
//					printf("%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\n", data.Coordinates.ThetaX, data.Coordinates.ThetaY, data.Coordinates.ThetaZ, data.Coordinates.X, data.Coordinates.Y, data.Coordinates.Z, data.Fingers.Finger1, data.Fingers.Finger2, data.Fingers.Finger3);
//					fprintf(pfile,"%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f\n", data.Coordinates.ThetaX, data.Coordinates.ThetaY, data.Coordinates.ThetaZ, data.Coordinates.X, data.Coordinates.Y, data.Coordinates.Z, data.Fingers.Finger1, data.Fingers.Finger2, data.Fingers.Finger3);
//					fflush(pfile);
//					break;
//				default: break;
//			}
//			(*MySendAdvanceTrajectory)(pointToSend);
//	
//		}while(c!='x');
		int y = 0;
//		for (int alpha = 0; alpha <10; alpha++)
		while(c != 'd')
		{
			c = 'r';
			while (c != 'c')
			{
				printf("Please place foam between fingers and press 'c' to close fingers.\n");
				while( ( c=getchar() ) =='\n'){}
				//c = getchar();
			}
//			printf("1%c\n",c);
			pointToSend.Position.Fingers.Finger1 = 6250.0f;
			pointToSend.Position.Fingers.Finger2 = 6250.0f;
			pointToSend.Position.Fingers.Finger3 = 6250.0f;
		
			result = (*MySendBasicTrajectory)(pointToSend);
			printf("result = %d\t",result);
			y = 100;
			printf("Close.\n");
			for (int x = 0; x<y; x++)
			{
				usleep(10000);
				result = (*MySendBasicTrajectory)(pointToSend);
				
			}
			usleep(500000);
			c = 'r';
			while (!(c == 'd' || c == 'o'))
			{
				printf("If you are content with the grasp, press 'd' for 'done'.\nIf not, press 'o' to open the grasper and try again.\n");
				while( ( c=getchar() ) =='\n'){}
				//c = getchar();
			}
//			printf("2%c\n",c);
			if(c == 'd') continue;
			
			pointToSend.Position.Fingers.Finger1 = 3500.0f;
			pointToSend.Position.Fingers.Finger2 = 3500.0f;
			pointToSend.Position.Fingers.Finger3 = 3500.0f;
		
			result = (*MySendBasicTrajectory)(pointToSend);
			y = 100;
			printf("Open.\n");
			for (int x = 0; x<y; x++)
			{
				usleep(10000);
				result = (*MySendBasicTrajectory)(pointToSend);
			}
			usleep(500000);
		}
		
		fprintf(pfile,"\n");
		fclose (pfile);

		printf("End of example\n");
		(*MyCloseAPI)();
	}
	else
	{
		printf("Error detected, leaving the example...\n");
	}
	return 0;
}
