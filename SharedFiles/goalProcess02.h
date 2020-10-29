#ifndef GOAL_SYSTEM
#define GOAL_SYSTEM

//#include <map>
//#include <vector>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include <chrono>
#include <algorithm>

#include <unistd.h>
#include <sys/time.h>

#include "DataStorage_7DOF_01.h"


//#include "globalVariablesAndConstants4.h"

int *goalNumArray = NULL;


const float ledLocationsX[6] = {0.27,0.57,0.27,0.57,0.57,0.57};
//const float ledLocationsY[6] = {0.50,0.50,0.50,0.50,0.20,0.20};
const float ledLocationsY[6] = {0.47,0.47,0.47,0.47,0.17,0.17};
const float ledLocationsZ[6] = {0.25,0.25,0.50,0.50,0.25,0.50};

//LED posts {left bottom, center bottom, left top, center top, right bottom, right top}
// 1DOF: left bottom, center bottom
// 2DOF: 1DOF + left top, center top
// 3DOF: 2DOF + right bottom, right top

// Slots {0.right top, 1.bottom center, 2.back top, 3.bottom front, 4.right center, 5.back center}
// 4 DOFa: starting empty from random position, grab from slot 0 and deactivate reed switch
// 4 DOFb: starting with object clasped and in correct orientation, place in slot 0 to activate reed switch and release, moving back a certain amount from wall
// 5 DOF: same as 4 DOFa and 4 DOFb except incorporate PITCH and slot 1, vary starting orientation (about pitch)
// 6 DOF: same as 5 DOF except incorporate YAW and slot 2 (and 3?), vary starting orientation (about pitch and yaw)
// 7 DOF: same as 6 DOF except incorporate ROLL and slots 3-5, vary starting orientation



const float timeRequiredToBeInGoal = 0.5; //s
const float radiusOfGoal = 0.025; //m

int totalNumberOfGoals = 8; //Subject to change for each DOF

//int slotGoal = -1;

void initializeGoalOrder3D(const int NUM_OF_DOFs)
{

	switch(NUM_OF_DOFs)
	{
		case 1:
		{
//			totalNumberOfGoals = 8; //Do we want a different amount for this
			goalNumArray = new int[totalNumberOfGoals];
			for (int i = 0;i<totalNumberOfGoals;i++)
			{
				goalNumArray[i] = (i+1)%2;
			}
//			std::random_shuffle(&goalNumArray[0],&goalNumArray[totalNumberOfGoals]);
//			ledLocationsY[0] = 0.4;
			break;
		}
		case 2:
		{
			totalNumberOfGoals = 10; //Do we want a different amount for this
			goalNumArray = new int[totalNumberOfGoals];
//			for (int i = 0;i<totalNumberOfGoals;i++)
//			{
//				goalNumArray[i] = i%4;
//			}
//			while(goalNumArray[0]==0)
//			{
//				std::random_shuffle(&goalNumArray[0],&goalNumArray[totalNumberOfGoals]);
//			}
			// 1DOF: left bottom, center bottom
			// 2DOF: 1DOF + left top, center top
			goalNumArray[0] = 2;
			goalNumArray[1] = 1;
			goalNumArray[2] = 3;
			goalNumArray[3] = 0;
			goalNumArray[4] = 1;
			goalNumArray[5] = 2;
			goalNumArray[6] = 0;
			goalNumArray[7] = 3;
			goalNumArray[8] = 1;
			goalNumArray[9] = 0;
//			goalNumArray[10] = ;
//			goalNumArray[11] = ;
			break;
		}
		case 3:
		{
		// 1DOF: left bottom, center bottom
		// 2DOF: 1DOF + left top, center top
		// 3DOF: 2DOF + right bottom, right top
			totalNumberOfGoals = 14; //Do we want a different amount for this
			goalNumArray = new int[totalNumberOfGoals];
//			for (int i = 0;i<totalNumberOfGoals;i++)
//			{
//				goalNumArray[i] = i%6;
//			}
//			while(goalNumArray[0]==0)
//			{
//				std::random_shuffle(&goalNumArray[0],&goalNumArray[totalNumberOfGoals]);
//			}
			goalNumArray[0] = 1;
			goalNumArray[1] = 4;
			goalNumArray[2] = 5;
			goalNumArray[3] = 3;
			goalNumArray[4] = 5;
			goalNumArray[5] = 4;
			goalNumArray[6] = 1;
			goalNumArray[7] = 5;
			goalNumArray[8] = 3;
			goalNumArray[9] = 4;
			goalNumArray[10] = 2;
			goalNumArray[11] = 5;
			goalNumArray[12] = 0;
			goalNumArray[13] = 4;
			break;
		}
		case 4:
		case 5:
		case 6:
		case 7:
		default:
		{
			printf("How did we get here??? Don't send NUM_OF_DOFs > 3 to initializeGoalOrder3D\n");
			break;
		}
	}
}

int goalSlotPicker(const int NUM_OF_DOFs)
{
	switch(NUM_OF_DOFs)
	{
		case 1:
		case 2:
		case 3:
		{
			printf("How did we get here??? Don't send NUM_OF_DOFs < 4 to goalSlotPicker\n");
			return -1;
		}
		case 4:
		{
			return 0; //right top
		}
		case 5:
		{
			return 1; //back top
		}
		case 6:
		{
			return 2; //bottom
		}
		case 7:
		{
			return 3; //a twisted one
		}
		default:
		{
			printf("How did we get here??? Don't send NUM_OF_DOFs > 7 to goalSlotPicker\n");
			return -1;
		}
	}
}

void currPose2TipLEDLoc(dataPoint *currDP)
{
	float rotX = currDP->thetaX;
	float rotY = currDP->thetaY;
	float rotZ = currDP->thetaZ;
	
	float tipLEDOffsetX = baseTipLEDOffsetZ*sin(rotY) + baseTipLEDOffsetX*cos(rotY)*cos(rotZ) - baseTipLEDOffsetY*cos(rotY)*sin(rotZ);
	float tipLEDOffsetY = baseTipLEDOffsetX*(cos(rotX)*sin(rotZ) + cos(rotZ)*sin(rotX)*sin(rotY)) + baseTipLEDOffsetY*(cos(rotX)*cos(rotZ) - sin(rotX)*sin(rotY)*sin(rotZ)) - baseTipLEDOffsetZ*cos(rotY)*sin(rotX);
	float tipLEDOffsetZ = baseTipLEDOffsetX*(sin(rotX)*sin(rotZ) - cos(rotX)*cos(rotZ)*sin(rotY)) + baseTipLEDOffsetY*(cos(rotZ)*sin(rotX) + cos(rotX)*sin(rotY)*sin(rotZ)) + baseTipLEDOffsetZ*cos(rotX)*cos(rotY);
	
	currDP->tipLEDLoc[0] = currDP->currentLoc[0] + tipLEDOffsetX;
	currDP->tipLEDLoc[1] = currDP->currentLoc[1] + tipLEDOffsetY;
	currDP->tipLEDLoc[2] = currDP->currentLoc[2] - tipLEDOffsetZ;
	
	return;
}

const unsigned char startBlinking[] = {0xfa};
const unsigned char  stopBlinking[] = {0xfb};

int greg = 0;
void goalCheck3D(dataPoint *currDP, timeval currentTime, int fd_teensy)
{
	//are we inside goal area?
/*	float dist2goal = sqrt( //this is straight distance in meters
					  (currDP->currentX-ledLocationsX[goalNumArray[currDP->goalIterator]])*(currDP->currentX-ledLocationsX[goalNumArray[currDP->goalIterator]])
					+ (currDP->currentY-ledLocationsY[goalNumArray[currDP->goalIterator]])*(currDP->currentY-ledLocationsY[goalNumArray[currDP->goalIterator]])
					+ (currDP->currentZ-ledLocationsZ[goalNumArray[currDP->goalIterator]])*(currDP->currentZ-ledLocationsZ[goalNumArray[currDP->goalIterator]]));
*/					
	float dist2goal = sqrt( //this is straight distance in meters
					  (currDP->tipLEDLoc[0]-ledLocationsX[currDP->goalNum])*(currDP->tipLEDLoc[0]-ledLocationsX[currDP->goalNum])
					+ (currDP->tipLEDLoc[1]-ledLocationsY[currDP->goalNum])*(currDP->tipLEDLoc[1]-ledLocationsY[currDP->goalNum])
					+ (currDP->tipLEDLoc[2]-ledLocationsZ[currDP->goalNum])*(currDP->tipLEDLoc[2]-ledLocationsZ[currDP->goalNum]));
	if(++greg % 100 == 0)
	{
		printf("%f\t%f\t%f\n",currDP->tipLEDLoc[0]-ledLocationsX[currDP->goalNum],currDP->tipLEDLoc[1]-ledLocationsY[currDP->goalNum],currDP->tipLEDLoc[2]-ledLocationsZ[currDP->goalNum]);
	}
	
	if(dist2goal <= radiusOfGoal ) //rad = 0.05m; 
    {
//		if (currDP->wasJustOutsideGoal)
//		{
//			if (currDP->goalNum == 0)
//			{
//				currDP->cursorStatus = 1; //used for color changes...... is it?
//			}
//			else
//			{
//				currDP->cursorStatus = 2;
//			}
//		}
//		else {currDP->cursorStatus = -2;} //this is a terrible way of doing things.....
//		currDP->wasJustOutsideGoal = false;
		
		//const char startBlinking[] = {0xfa};
		write( fd_teensy, startBlinking, sizeof(startBlinking) ); // Send to device
		
		if ( ( (currentTime.tv_sec - currDP->goalArrivalTime.tv_sec)*1.0 + (currentTime.tv_usec - currDP->goalArrivalTime.tv_usec)/1000000.0 ) >= timeRequiredToBeInGoal)
		{
//			if (currDP->goalNum == 0) //we are using cursorStatus differently
			//if (currDP->cursorStatus == 1) //we set the value up there, might as well use it
//			{		
				currDP->goalIterator++;
				if ( currDP->goalIterator >= totalNumberOfGoals )
				{
					keepGoing = false; //we are all done
					return;
				}
				currDP->goalNum = goalNumArray[currDP->goalIterator];
				
				gettimeofday(&currDP->goalStartTime,NULL); //this is used for changing the color of the goal
//			}
//			else
//	activatedSlot		{
//				currDP->goalNum = 0;
//			}
			//SEND THE NEW SIGNAL TO THE TEENSY TO CHANGE TO currDP->goalNum
			const char ledNUM[] = {(char)(currDP->goalNum+6)}; //The teensy has these listed as the 6 slots then 6 posts
			write( fd_teensy, ledNUM, sizeof(ledNUM) ); // Send to device
			printf("Go to goal %d\n",currDP->goalNum);
		}
		else return;
	}
	else
	{
		gettimeofday(&currDP->goalArrivalTime,NULL); //you haven't reached it yet
		write( fd_teensy, stopBlinking, sizeof(stopBlinking) ); // Send to device
//		currDP->wasJustOutsideGoal = true;
	}
	
	return;
}


#endif //GOAL_SYSTEM

