#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include "tnt/tnt_array1d.h"
#include "tnt/tnt_array2d.h"

#include "jama/jama_eig.h"


using namespace std;
using namespace TNT;
using namespace JAMA;

namespace PCA {
	bool debug = false;
	
	template < class T>

	void adjust_data(Array2D<double>& d, Array1D<double>& means) {
	   for (int i=0; i<d.dim2(); ++i) { 
		   double mean = 0;
		   for (int j=0; j<d.dim1(); ++j) {
			   mean += d[j][i];
		   }

		   mean /= d.dim1();

		   // store the mean
		   means[i] = mean;

		   // subtract the mean
		   for (int j=0; j<d.dim1(); ++j) {
			   d[j][i] -= mean;
		   }
	   }
	}

	double compute_covariance(const Array2D<double>& d, int i, int j) {
	   double cov = 0;
	   for (int k=0; k<d.dim1(); ++k) {
		   cov += d[k][i] * d[k][j];
	   }

	   return cov / (d.dim1() - 1);
	}

	void compute_covariance_matrix(const Array2D<double> & d, Array2D<double> & covar_matrix) {
		int dim = d.dim2();
		assert(dim == covar_matrix.dim1());
		assert(dim == covar_matrix.dim2());
		for (int i=0; i<dim; ++i) {
			for (int j=i; j<dim; ++j) {
				covar_matrix[i][j] = compute_covariance(d, i, j);
			}
		}


		// fill the Left triangular matrix
		for (int i=1; i<dim; i++) {
			for (int j=0; j<i; ++j) {
				covar_matrix[i][j] = covar_matrix[j][i];
			}
		}

	}

	// Calculate the eigenvectors and eigenvalues of the covariance
	// matrix
	void eigen(const Array2D<double> & covar_matrix, Array2D<double>& eigenvector, Array2D<double>& eigenvalue) {
		Eigenvalue<double> eig(covar_matrix);
		eig.getV(eigenvector);
		eig.getD(eigenvalue);
	}
}









//a bunch of random includes from Driver0_2.cpp
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <time.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h> //  stre`rror
#include <stdexcept> // for exception, runtime_error, out_of_range

//g++ -std=c++11 -pthread StreamIMUInfo6IMU.cpp -ldl -o StreamIMUInfo6IMU -lboost_iostreams -lboost_system -lboost_filesystem

//#define ROBOT_IS_PLUGGED_IN true

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fstream>
#include <sys/stat.h>

#include "yei_threespace_api4.h"

bool keepGoing = true; //the loops will check this value
int fd_ = 0; // file handle for dongle stream. must be done globally for threading
const int INITIAL_IMU = 0;
const int NUMBER_OF_IMUS = 5;

const int TOTAL_COLLECTION_TIME = 60; //seconds
const int LOOP_FREQUENCY = 100; //cycles per second
const int TOTAL_LOOPS = TOTAL_COLLECTION_TIME*LOOP_FREQUENCY;


struct orientation
{
	float pitch = 0;
	float yaw = 0;
	float roll = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	//int batteryLevel = 100;
}orient[NUMBER_OF_IMUS]; //must be instantiated globally for threading

const int LOOP_FREQUENCY = 100; //cycles per second
//no point going faster than 100Hz as that is how fast the Jaco arm controller refreshes

//threading
#include <pthread.h>

#include "readerthreadWithYaw.h"


// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_handler(int signum)
{
	printf("Caught signal %d\n",signum);
	keepGoing = false;
}

int main(int argc, char * argv[])
{
   	
   	signal(SIGINT, signal_handler);
   	
   	char c;
   	
  
//	dataPoint currDP; //currentDataPoint
//	double IMU_Values[TOTAL_LOOPS][NUMBER_OF_IMUS*3] = {0.0};


	fd_ = openDevice();
	if (( fd_ ) < 0 ) 
	{
		printf ("Unable to open file");
		return 1;
	}

	
	

	
	int result; //used for error checking
	

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
	
	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
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
	
	//usleep(2500000); // allow the user to see the battery levels but realize that the IMUs have already been tared.
	printf("Program will tare IMUs and start streaming. Make sure user is in base position and then ");
	do{
		printf("enter 'y' to continue/'q' to quit.\n");
		c = getchar(); //this is included from way up there
		if (c == 'q')
		{
			//Turn off Kinova API
			
			result = closeDevice (fd_);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_: %d\n",fd_);
			
			
			return 1;;
		}
	}while(c != 'y');
	
	
	
	//start the IMUs streaming
	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
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
			
			result = closeDevice (fd_);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_: %d\n",fd_);
			
			
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
			
			result = closeDevice (fd_);
			if ( result != 0 ) 
			{
				printf ("Unable to close device"); 
			}
			printf ("Device closed. fd_: %d\n",fd_);
			
			
			return 1;
		}
		k=0;
		printf ("Streaming started for IMU #%d\n", device_iter);
	}
	
	
	//start the new thread with the reader from the dongle
	pthread_t readerThread;
	int threadCreateReturn;
	
	threadCreateReturn = pthread_create( &readerThread, NULL, readerFunction, NULL);
	
	struct timeval startOfProgramTime, currentTime;
	currDP.programTime = 0.0;

	int counter = 0;
	unsigned long int sleepTime = 0;
	
	Array2D<double> IMU_values( TOTAL_LOOPS,  NUMBER_OF_IMUS*3);
	usleep(1000000);
	
	gettimeofday(&startOfProgramTime, NULL);
	
	while(counter<TOTAL_LOOPS) //errors should break out of the loop and set keepGoing to false for the reader thread
   	{
		for (int i = INITIAL_IMU; i< INITIAL_IMU + NUMBER_OF_IMUS; i++)
		{
			pthread_mutex_lock( &orient[i].mutex );
			IMU_values[counter][i*3]   = orient[i].roll;
			IMU_values[counter][i*3+1] = orient[i].yaw;
			IMU_values[counter][i*3+2] = orient[i].pitch;
			pthread_mutex_unlock( &orient[i].mutex );	
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
  	}
   	keepGoing = false;
   	
   	
   	//Putting the IMU stop_streaming portion hear for hope of error fix, the reasoning being unknown
   	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
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

   	
   	//wait for the readerThread to join; it should have quit once it got saw keepGoing == false
   	pthread_join( readerThread, NULL);
   	
   	//IMUS
   	for (int device_iter = INITIAL_IMU; device_iter< INITIAL_IMU + NUMBER_OF_IMUS ; device_iter++)
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
	
	printf("Printing calibData to file \"calibData.txt\".\n");
	std::ofstream outputFile ("calibData.txt");
	for (counter = 0; counter < TOTAL_LOOPS; counter++)
	{
		for (int i = 0; i < NUMBER_OF_IMUS*3;i++)
		{
			outputfile << IMU_VALUES[counter][i]) << "\t";
		}
		outputfile << endl;
	}
	outputFile.close();
		
	printf("Calculating PCAs.\n");

	Array1D<double> means( NUMBER_OF_IMUS*3);
	adjust_data(IMU_values, means);

	Array2D<double> covar_matrix( NUMBER_OF_IMUS*3,  NUMBER_OF_IMUS*3);
	compute_covariance_matrix(IMU_values, covar_matrix);

	// get the eigenvectors
	Array2D<double> eigenvector(NUMBER_OF_IMUS*3, NUMBER_OF_IMUS*3);

	// get the eigenvalues
	Array2D<double> eigenvalue(NUMBER_OF_IMUS*3, NUMBER_OF_IMUS*3);
	eigen(covar_matrix, eigenvector, eigenvalue);
	
	
	printf("Printing calibData to file \"PCA_Data.txt\".\n");
	std::ofstream outputFile ("PCA_Data.txt");
	for (int i=NUMBER_OF_IMUS*3-1; i >=0; i--)
	{
		for (int j=0; j < NUMBER_OF_IMUS*3; j++)
		{
			outputfile << eigenvector[j][i] << "\t";
		}
		outputfile << eigenvalue[i][i] << endl;
	}
	outputFile.close();
	printf ("Exiting program\n");
   	
   	return EXIT_SUCCESS;
}

