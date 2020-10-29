const float PI = 3.14159265359;

//pretty numbers <=100 for 1,000,000 / x = round number: x= 1, 2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 64, 80, 100
//all of them!!!!! 1, 2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 64, 80, 100, 125, 160, 200, 250, 320, 400, 500, 625, 800, 1000, 1250, 1600, 2000, 2500, 3125, 4000, 5000, 6250, 8000, 10000, 12500, 15625, 20000, 25000, 31250, 40000, 50000, 62500, 100000, 125000, 200000, 250000, 500000, 1000000
const int LOOP_FREQUENCY = 100; //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes

volatile bool keepGoing = true; //the loops will check this value


const float StartX = 0.3;
const float StartY = 0.5;
const float StartZ = 0.4;

const float CenterX = 0.2;
const float CenterZ = 0.4;

const float StartRotX = PI/2;
const float StartRotY = 0.0;
const float StartRotZ = PI;

const float Y_ROBOT_MIN = -0.10;
const float Z_ROBOT_MIN = 0.02;

const float MAX_FINGER_CLOSE  = 6000.0;
const float MIN_FINGER_OPEN = 0000.0;

const float fingerGain = 10.0;


struct dataPoint
{
	float programTime		= 0.0; //since start of streaming
	
	float		currentLoc[3]	= {0.0,0.0,0.0};
	float		currentRot[3]	= {0.0,0.0,0.0};
	
	float		fingerSVal		= 0.0;
	
	float		fingersTold		= 0.0;
	
	float		rawCartVel[3]	= {0.0,0.0,0.0};
	float		rawRotVel[3]	= {0.0,0.0,0.0};
	
	float		jointAngle[6]	= {0.0,0.0,0.0,0.0,0.0,0.0};
};

void printDataPoint(const dataPoint *_DataPoint, std::ofstream &outputFile)
{
	outputFile << 
		_DataPoint->programTime << "\t";
	
	outputFile <<
		_DataPoint->currentLoc[0]	<< "\t" <<
		_DataPoint->currentLoc[1]	<< "\t" <<
		_DataPoint->currentLoc[2]	<< "\t" <<
		
		_DataPoint->currentRot[0]	<< "\t" <<
		_DataPoint->currentRot[1]	<< "\t" <<
		_DataPoint->currentRot[2]	<< "\t" <<
		
		_DataPoint->fingerSVal		<< "\t" <<

		_DataPoint->fingersTold		<< "\t" <<
		
		_DataPoint->rawCartVel[0]	<< "\t" <<
		_DataPoint->rawCartVel[1]	<< "\t" <<
		_DataPoint->rawCartVel[2]	<< "\t" <<
		
		_DataPoint->rawRotVel[0]	<< "\t" <<
		_DataPoint->rawRotVel[1]	<< "\t" <<
		_DataPoint->rawRotVel[2]	<< "\t" <<
		
		_DataPoint->jointAngle[0]	<< "\t" <<
		_DataPoint->jointAngle[1]	<< "\t" <<
		_DataPoint->jointAngle[2]	<< "\t" <<
		_DataPoint->jointAngle[3]	<< "\t" <<
		_DataPoint->jointAngle[4]	<< "\t" <<
		_DataPoint->jointAngle[5]	<< "\t" <<
		
	std::endl;
	return;
}
