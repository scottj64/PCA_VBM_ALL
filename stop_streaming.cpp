//extern "C" { //this procuces a bunch of linking errors
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <unistd.h> //  close
#include <string.h> //  strerror

// added by Sanders
#include <stdio.h> // printf
#include <stdexcept> // for exception, runtime_error, out_of_range
#include <stdlib.h> // exit

#include "SharedFiles/yei_threespace_api5.h"
#include <signal.h>
#include <stdlib.h>

//}

/*
g++ -std=c++11 stop_streaming.cpp -o stop_streaming
fuser -k /dev/ttyACM*

./stop_streaming [ NUMBER_OF_IMUS [ portNum [ IMUoffsetValue ] ] ]
*/


bool keepGoing = true; //the loops will check this value
void signal_handler(int signum)
{
	keepGoing = false;
	printf("\nCaught signal %d\n",signum);
}

ssize_t amt;






int main(int argc, char ** argv)
{
	signal(SIGINT, signal_handler);
	
	int NUMBER_OF_IMUS = 5;
	int IMUoffsetValue = 4;
	int portNum = 0;
	
	if (argc == 2)
	{
		NUMBER_OF_IMUS = atoi(argv[1]);
	}
	else if (argc == 3)
	{
		NUMBER_OF_IMUS = atoi(argv[1]);
		portNum = atoi(argv[2]);
	}
	else if (argc == 4)
	{
		NUMBER_OF_IMUS = atoi(argv[1]);
		portNum = atoi(argv[2]);
		IMUoffsetValue = atoi(argv[3]);
	}
	
	printf("NumIMUs = %d\nPortNum = %d\nIMUOffset = %d\n",NUMBER_OF_IMUS,portNum,IMUoffsetValue);
	
	int fd_, result;
	fd_ = openDevice(portNum);
	if (( fd_ ) < 0 ) 
	{
		printf ("Unable to open file /dev/ttyACM%d\n",portNum);
		return 1;
	}

	printf("fd_ = %d\n",fd_);
	
	
	for (int device_iter = 0 + IMUoffsetValue; device_iter< NUMBER_OF_IMUS + IMUoffsetValue; device_iter++)
	//for (int device_iter = 1; device_iter< 2 ; device_iter++)
	{
		//if (device_iter ==2) continue;
		
		result = getBatteryLevel(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to get battery level of IMU #%d, error: %d\n", device_iter, result);
			//return 1;
		}
		printf ("battery level of IMU #%d = %d\n", device_iter, result);
		
		
		result = stopStreaming(fd_, device_iter);
		if ( result < 0 ) 
		{
			printf ("Unable to stop streaming for IMU #%d\n", device_iter);
			//return 1;
		}
		
		printf ("Streaming stopped for IMU #%d\n", device_iter);
		
	}
	::close(fd_);


	return 0;
}
