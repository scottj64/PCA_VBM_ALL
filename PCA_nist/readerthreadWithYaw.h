// This is going to be the reader thread's function that just keeps looping as long as keepGoing is true
// 

// Plan of attack as I can't change the header, we have to assume eerything works fine and dandy
/*
read in two chars
	if the first char is 0 then the packet was successful
	else start back at the top
the second char signifies the logicID -> store it
the third char signifies returnedDataLength
read in returnedDataLength number of chars (12)

lock orient[logicID].mutex
cast first four bytes read in data as float (is this part necessary?) and set/copy to orient[logicID].pitch
cast last  four bytes read in data as float (is this part necessary?) and set/copy to orient[logicID].roll
unlock orient[logicID].mutex

start back at top
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


//for that final wait
#include <unistd.h>
#include <sys/time.h>


//#include "globalVariablesAndConstants.h"
/*
struct orientation
{
	float pitch = 0;
	//float yaw = 0;
	float roll = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}orient[NUMBER_OF_IMUS]; //must be instantiated globally for threading
*/


#include "checkSum_EndianSwap.h"

void *readerFunction(void*)
{
//	int fd_ = *(int*)p;
	unsigned char success_failure_logicID[2] = {'\0','\0'};
	unsigned char dataLengthChar = '\0';
	unsigned char returnedData[12];
	memset(returnedData, '\0', sizeof returnedData);
	
	unsigned int logicID, dataLengthInt;
	
	ssize_t readReturn = 0; //to get it to stop bugging me about ignored return values
	
	printf("IMUReaderThread started!\n");
	
	while(keepGoing)
	{
		//printf("fd_, keepGoing = %d, %s\n",fd_,keepGoing ? "true" : "false");
		readReturn = read( fd_, &success_failure_logicID, 2 );
		if ( success_failure_logicID[0] != '\0' )	continue;
		
		//implied else
		logicID = (unsigned int) success_failure_logicID[1];
		
		readReturn = read( fd_, &dataLengthChar, 1 );
		dataLengthInt = (unsigned int) dataLengthChar;
		
		readReturn = read(fd_, &returnedData, dataLengthInt ); //pitch,yaw,roll
		
		endian_swap_32( (unsigned int&) returnedData[0] ); //pitch
		endian_swap_32( (unsigned int&) returnedData[4] ); //yaw
		endian_swap_32( (unsigned int&) returnedData[8] ); //roll
		
		pthread_mutex_lock( &orient[logicID].mutex );
		
		memcpy(&orient[logicID].pitch, &returnedData[0], sizeof( float ) );
		memcpy(&orient[logicID].yaw  , &returnedData[4], sizeof( float ) );
		memcpy(&orient[logicID].roll , &returnedData[8], sizeof( float ) );
		
		pthread_mutex_unlock( &orient[logicID].mutex );
		
		//printf("%d\t%f\t%f\n",logicID, orient[logicID].pitch, orient[logicID].roll);
	}
	
	printf("IMUReaderThread loop ended!\n");
//	long int returnedLength;
	usleep(500000);
	//I am considering adding a bit that will simply read for a while until everything is empty
	//Here's to hoping it doesn't break everything....
	while (read(fd_, &returnedData, 1 ) > 0){};

}
