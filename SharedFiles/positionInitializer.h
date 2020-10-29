#include "goalProcess02.h"

float CONSTRAINT_X = ledLocationsX[0]; //THIS NEEDS TO BE CONFIGURED CORRECTLY
float CONSTRAINT_Y = ledLocationsY[0]; //THIS NEEDS TO BE CONFIGURED CORRECTLY
float CONSTRAINT_Z = ledLocationsZ[0]; //THIS NEEDS TO BE CONFIGURED CORRECTLY

//bool constraintXActive = true;
//bool constraintYActive = true;
//bool constraintZActive = true;

void positionDOFInitializer(int numberOfDOFs)
{
	CartXOn		= false;
	CartYOn		= false;
	CartZOn		= false;
	RotXOn		= false;
	RotYOn		= false;
	RotZOn		= false;
	GrasperOn	= false;
	
	START_X		= ledLocationsX[0] - ( 0.0522);
	START_Y		= ledLocationsY[0] - ( 0.0677);
	START_Z		= ledLocationsZ[0] - (-0.0519);
//	START_ROT_X	= PI/2;
//	START_ROT_Y	= -PI/2;
//	START_ROT_Z	= PI;
	START_ROT_X	= 2.22516;
	START_ROT_Y	= -0.548839;
	START_ROT_Z	= -2.34759;
	

	
	
	switch(numberOfDOFs)
	{
		case 1: //left right		//start at farther goal0 location
		{
			CartXOn = true;
//			ledLocationsY[0] = 0.4;
	//		START_X		= goalLocationsX[0];
	//		START_Y		= goalLocationsY[0];
	//		START_Z		= goalLocationsZ[0];
	//		START_ROT_X	= PI/2;
	//		START_ROT_Y	= -PI/2;
	//		START_ROT_Z	= PI;
			break;
		}
		case 2: //forward backward	//start at middle goal0 location
		{
			CartXOn = CartZOn = true;
	//		START_X		= goalLocationsX[0];
	//		START_Y		= goalLocationsY[0];
	//		START_Z		= goalLocationsZ[0];
	//		START_ROT_X	= PI/2;
	//		START_ROT_Y	= -PI/2;
	//		START_ROT_Z	= PI;
			break;
		}
		case 3: //up down			//start at middle goal0 location
		{
			CartXOn = CartYOn = CartZOn = true;
	//		START_X		= goalLocationsX[0];
	//		START_Y		= goalLocationsY[0];
	//		START_Z		= goalLocationsZ[0];
	//		START_ROT_X	= PI/2;
	//		START_ROT_Y	= -PI/2;
	//		START_ROT_Z	= PI;
			break;
		}
		case 4:	//grasper
		{
			CartXOn = CartYOn = CartZOn = RotYOn = true;
			START_X		= X_ROBOT_MIN;
			START_Y		= Y_ROBOT_MIN + (Y_ROBOT_MAX - Y_ROBOT_MIN)/2;
			START_Z		= Z_ROBOT_MIN + (Z_ROBOT_MAX - Z_ROBOT_MIN)/2;
			START_ROT_X	= PI/2;
			START_ROT_Y	= -PI/2;
			START_ROT_Z	= PI;
			break;
		}
		case 5: //yaw
		{
			CartXOn = CartYOn = CartZOn = RotYOn = RotXOn = true;
			START_X		= X_ROBOT_MIN;
			START_Y		= Y_ROBOT_MIN + (Y_ROBOT_MAX - Y_ROBOT_MIN)/2;
			START_Z		= Z_ROBOT_MIN + (Z_ROBOT_MAX - Z_ROBOT_MIN)/2;
			START_ROT_X	= PI/2;
			START_ROT_Y	= -PI/2;
			START_ROT_Z	= PI;
			break;
		}
		case 6: //pitch
		{
			CartXOn = CartYOn = CartZOn = RotYOn = RotXOn = RotZOn = true;
			START_X		= X_ROBOT_MIN;
			START_Y		= Y_ROBOT_MIN + (Y_ROBOT_MAX - Y_ROBOT_MIN)/2;
			START_Z		= Z_ROBOT_MIN + (Z_ROBOT_MAX - Z_ROBOT_MIN)/2;
			START_ROT_X	= PI/2;
			START_ROT_Y	= -PI/2;
			START_ROT_Z	= PI;
			break;
		}
		case 7: //roll
		{
			CartXOn = CartYOn = CartZOn = RotYOn = RotXOn = RotZOn = GrasperOn = true;
			START_X		= X_ROBOT_MIN;
			START_Y		= Y_ROBOT_MIN + (Y_ROBOT_MAX - Y_ROBOT_MIN)/2;
			START_Z		= Z_ROBOT_MIN + (Z_ROBOT_MAX - Z_ROBOT_MIN)/2;
			START_ROT_X	= PI/2;
			START_ROT_Y	= -PI/2;
			START_ROT_Z	= PI;
			break;
		}
		default:
		{
			break;
		}
	}	
}
