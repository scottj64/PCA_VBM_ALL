#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


//for that final wait
#include <unistd.h>
#include <sys/time.h>

#include "checkSum_EndianSwap.h"

void *teensyReaderFunction(void*)
{
//	int fd_teensy = *(int*)p;
	char returnedValues[1];
	memset(returnedValues, '\0', sizeof returnedValues);
	
	ssize_t readReturn = 0; //to get it to stop bugging me about ignored return values
	
//	char startLoop = 0x04;
//	char rebootTeensy = 0x05;
	
//	write(fd_teensy, &startLoop, 1);
	printf("TeensyReaderThread started!\n");
	
	
	while (keepGoing)
	{
		//return structure is {0x05, controlState=0,1,2, closing, opening, requestTare, requestHome}
		
		
		memset(returnedValues, '\0', sizeof returnedValues);
		read(fd_teensy,returnedValues, sizeof returnedValues);
		
		if (returnedValues[0] != 0xff)
		{
			pthread_mutex_lock( &slot_mutex );
			currentlyOccupiedSlot_GLOBAL = (int)returnedValues[0];
			pthread_mutex_unlock( &slot_mutex );
		}
		else
		{
			pthread_mutex_lock( &slot_mutex );
			currentlyOccupiedSlot_GLOBAL = -1;
			pthread_mutex_unlock( &slot_mutex );
		}
		
		//usleep(1000000/(LOOP_FREQUENCY*2));
	}
	
	printf("TeensyReaderThread loop ended!\n");
//	write(fd_teensy, &rebootTeensy, 1);
	
//	unsigned int returnedLength;
/*	usleep(1500000);
	//I am considering adding a bit that will simply read for a while until everything is empty
	//Here's to hoping it doesn't break everything....
	do
	{
//		write(fd_teensy, &pauseLoop, 1);
//		returnedLength = read(fd_teensy, &returnedValues, 1 );
		//printf("Read Return length = %ld\n",returnedLength);
	}while (read(fd_teensy, &returnedValues, 1 ) > 0);
*/
}
