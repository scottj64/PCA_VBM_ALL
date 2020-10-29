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

int goalNumArray[8] = {0,0,0,0,0,0,0,0};
//float goalLocationsX[9] = {0.35,0.15,0.55,0.55,0.15,0.15,0.55,0.55,0.15};
//float goalLocationsY[9] = {0.2,0.4,0.4,0.0,0.0,0.4,0.4,0.0,0.0};
float goalLocationsZ[6] = {0.25,0.25,0.25,0.25,0.4,0.55};
//using these until I can get goal 4&8 removed
float goalLocationsX[6] = {0.35,0.15,0.55,0.55,0.35,0.65};
float goalLocationsY[6] = {0.2,0.4,0.4,0.0,0.5,0.2};


const float timeRequiredToBeInGoal = 0.5; //s
const float radiusOfGoal = 0.025; //m

int totalNumberOfGoals = 8;

void initializeGoalOrder(const int NUM_OF_DOFs)
{

	switch(NUM_OF_DOFs)
	{
		case 1:
		{
			for (int i = 0;i<totalNumberOfGoals;i++)
			{
				goalNumArray[i] = i%2+1;
			}
			std::random_shuffle(&goalNumArray[0],&goalNumArray[totalNumberOfGoals]);
			goalLocationsY[0] = 0.4;
			break;
		}
		case 2:
		{
			for (int i = 0;i<totalNumberOfGoals;i++)
			{
				goalNumArray[i] = i%3+1;
			}
			std::random_shuffle(&goalNumArray[0],&goalNumArray[totalNumberOfGoals]);
			break;
		}
		case 3:
		{
			for (int i = 0;i<totalNumberOfGoals;i++)
			{
				goalNumArray[i] = i%5+1;
			}
			std::random_shuffle(&goalNumArray[0],&goalNumArray[totalNumberOfGoals]);
			break;
		}
		case 4:
		{
			
			break;
		}
		case 5:
		{
			
			break;
		}
		case 6:
		{
			
			break;
		}
		case 7:
		{
			
			break;
		}
	}
}

void goalCheck3D(dataPoint *currDP, timeval currentTime, int fd_teensy)
{
	//are we inside goal area?
/*	float dist2goal = sqrt( //this is straight distance in meters
					  (currDP->currentX-goalLocationsX[goalNumArray[currDP->goalIterator]])*(currDP->currentX-goalLocationsX[goalNumArray[currDP->goalIterator]])
					+ (currDP->currentY-goalLocationsY[goalNumArray[currDP->goalIterator]])*(currDP->currentY-goalLocationsY[goalNumArray[currDP->goalIterator]])
					+ (currDP->currentZ-goalLocationsZ[goalNumArray[currDP->goalIterator]])*(currDP->currentZ-goalLocationsZ[goalNumArray[currDP->goalIterator]]));
*/					
	float dist2goal = sqrt( //this is straight distance in meters
					  (currDP->currentLoc[0]-goalLocationsX[currDP->goalNum])*(currDP->currentLoc[0]-goalLocationsX[currDP->goalNum])
					+ (currDP->currentLoc[1]-goalLocationsY[currDP->goalNum])*(currDP->currentLoc[1]-goalLocationsY[currDP->goalNum])
					+ (currDP->currentLoc[2]-goalLocationsZ[currDP->goalNum])*(currDP->currentLoc[2]-goalLocationsZ[currDP->goalNum]));
	  
	
	if(dist2goal <= radiusOfGoal ) //rad = 0.05m; 
    {
		if (currDP->wasJustOutsideGoal)
		{
			if (currDP->goalNum == 0)
			{
				currDP->cursorStatus = 1; //used for color changes...... is it?
			}
			else
			{
				currDP->cursorStatus = 2;
			}
		}
		else {currDP->cursorStatus = -2;} //this is a terrible way of doing things.....
		currDP->wasJustOutsideGoal = false;
				
		if ( ( (currentTime.tv_sec - currDP->goalArrivalTime.tv_sec)*1.0 + (currentTime.tv_usec - currDP->goalArrivalTime.tv_usec)/1000000.0 ) >= timeRequiredToBeInGoal)
		{
			if (currDP->goalNum == 0) //we are using cursorStatus differently
			//if (currDP->cursorStatus == 1) //we set the value up there, might as well use it
			{		
				currDP->goalIterator++;
				if ( currDP->goalIterator >= totalNumberOfGoals )
				{
					keepGoing = false; //we are all done
					return;
				}
				currDP->goalNum = goalNumArray[currDP->goalIterator];
				
				gettimeofday(&currDP->goalStartTime,NULL); //this is used for changing the color of the goal
			}
			else
			{
				currDP->goalNum = 0;
			}
			//SEND THE NEW SIGNAL TO THE TEENSY TO CHANGE TO currDP->goalNum
			const char ledNUM[] = {(char)currDP->goalNum};
			write( fd_teensy, ledNUM, sizeof(ledNUM) ); // Send to device
			printf("Go to goal %d\n",currDP->goalNum);
		}
		else return;
	}
	else
	{
		gettimeofday(&currDP->goalArrivalTime,NULL); //you haven't reached it yet
		currDP->wasJustOutsideGoal = true;
	}
	
	return;
}
#endif //PROJECTION6

