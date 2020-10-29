#ifndef YEI_THREESPACE_API_H_INCLUDED
#define YEI_THREESPACE_API_H_INCLUDED
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "checkSum_EndianSwap.h"

/********************************************//**
 * Macro for infinite duration for streaming setup.
 ***********************************************/
#define TSS_INFINITE_DURATION 0xffffffff


/********************************************//**
 * An enum expressing the command list of Streamable Commands.
 ***********************************************/
typedef enum Stream_Command_Enum
{
    TSS_GET_TARED_ORIENTATION_AS_QUATERNION                 = 0x00,
    TSS_GET_TARED_ORIENTATION_AS_EULER_ANGLES               = 0x01,
    TSS_GET_TARED_ORIENTATION_AS_ROTATION_MATRIX            = 0x02,
    TSS_GET_TARED_ORIENTATION_AS_AXIS_ANGLE                 = 0x03,
    TSS_GET_TARED_ORIENTATION_AS_TWO_VECTOR                 = 0x04,
    TSS_GET_DIFFERENCE_QUATERNION                           = 0x05,
    TSS_GET_UNTARED_ORIENTATION_AS_QUATERNION               = 0x06,
    TSS_GET_UNTARED_ORIENTATION_AS_EULER_ANGLES             = 0x07,
    TSS_GET_UNTARED_ORIENTATION_AS_ROTATION_MATRIX          = 0x08,
    TSS_GET_UNTARED_ORIENTATION_AS_AXIS_ANGLE               = 0x09,
    TSS_GET_UNTARED_ORIENTATION_AS_TWO_VECTOR               = 0x0a,
    TSS_GET_TARED_TWO_VECTOR_IN_SENSOR_FRAME                = 0x0b,
    TSS_GET_UNTARED_TWO_VECTOR_IN_SENSOR_FRAME              = 0x0c,
    TSS_GET_ALL_NORMALIZED_COMPONENT_SENSOR_DATA            = 0x20,
    TSS_GET_NORMALIZED_GYRO_RATE                            = 0x21,
    TSS_GET_NORMALIZED_ACCELEROMETER_VECTOR                 = 0x22,
    TSS_GET_NORMALIZED_COMPASS_VECTOR                       = 0x23,
    TSS_GET_ALL_CORRECTED_COMPONENT_SENSOR_DATA             = 0x25,
    TSS_GET_CORRECTED_GYRO_RATE                             = 0x26,
    TSS_GET_CORRECTED_ACCELEROMETER_VECTOR                  = 0x27,
    TSS_GET_CORRECTED_COMPASS_VECTOR                        = 0x28,
    TSS_GET_CORRECTED_LINEAR_ACCELERATION_IN_GLOBAL_SPACE   = 0x29,
    TSS_GET_TEMPERATURE_C                                   = 0x2b,
    TSS_GET_TEMPERATURE_F                                   = 0x2c,
    TSS_GET_CONFIDENCE_FACTOR                               = 0x2d,
    TSS_GET_ALL_RAW_COMPONENT_SENSOR_DATA                   = 0x40,
    TSS_GET_RAW_GYROSCOPE_RATE                              = 0x41,
    TSS_GET_RAW_ACCELEROMETER_DATA                          = 0x42,
    TSS_GET_RAW_COMPASS_DATA                                = 0x43,
    TSS_GET_BATTERY_VOLTAGE                                 = 0xc9,
    TSS_GET_BATTERY_PERCENT_REMAINING                       = 0xca,
    TSS_GET_BATTERY_STATUS                                  = 0xcb,
    TSS_GET_BUTTON_STATE                                    = 0xfa,
    TSS_NULL                                                = 0xff
}Stream_Command_Enum;

/********************************************//**
 * YEI 3-Space API streamable command.
 ***********************************************/
typedef unsigned char TSS_Stream_Command;



//Here is an example of what well written reader+writer functions would look like
// Taken from: https://github.com/smd-ros-devel/yei_tss_usb/blob/groovy-devel/src/tss_usb.c

/*

enum tss_usb_error
{
	TSS_USB_SUCCESS = 0,
	TSS_USB_ERROR_IO = -1,
	TSS_USB_ERROR_NO_DEVICE = -4,
	TSS_USB_ERROR_NO_MEM = -11,
	TSS_USB_ERROR_OTHER = -13,
	TSS_USB_ERROR_TIMEOUT = -14
};


static int send_cmd( const int fd, const unsigned char *data, const size_t num_bytes )
{
	int bytes_sent = write( fd, data, num_bytes );
	if( bytes_sent < 0 )
		return TSS_USB_ERROR_IO;
	else if( bytes_sent == 0 )
		return TSS_USB_ERROR_TIMEOUT;
	else if( (unsigned)bytes_sent != num_bytes )
		return TSS_USB_ERROR_IO;
	return TSS_USB_SUCCESS;
}

static int read_data( const int fd, unsigned char *data, size_t num_bytes )
{
	int bytes_recv;
	while( num_bytes )
	{
		bytes_recv = read( fd, data, num_bytes );
		if( bytes_recv < 0 )
			return TSS_USB_ERROR_IO;
		if( bytes_recv == 0 )
			return TSS_USB_ERROR_TIMEOUT;
		num_bytes -= bytes_recv;
	}
	return TSS_USB_SUCCESS;
}
*/






// I still have to figure out how to set the timestamp mode to Sensor's timestamp
int openDevice(int port) // I could possibly accept the filename string
{
	//Here is a exerpt of what you need to open the file.
	
	int fd_;
	std::string device_ = "/dev/ttyACM"+std::to_string(port);
	const char* path = device_.c_str();
	fd_ = ::open(path, O_RDWR | O_NOCTTY );// | O_NDELAY); //read/write, no controlling terminal, assumed blocking access

	//printf("fd_ = %d\n",fd_);
	 
	if (fd_ < 0)
	{
	 printf("fd_ : %d\n",fd_);
	 throw std::runtime_error("Failed to open device : " + device_);
	}

	//make sure it is non-blocking - I want blocking
	/*if (fcntl(fd_, F_SETFL, FNONBLOCK) < 0)
	{
	 //disconnect();
	 ::close(fd_);
	 //throw io_error(strerror(errno));
	 throw std::runtime_error(strerror(errno));
	}*/

	struct termios toptions;

	if (tcgetattr(fd_, &toptions)<0)
	// blocking accesss) < 0)
	{
	 //disconnect();
	 ::close(fd_);
	 //throw io_error(strerror(errno));
	 throw std::runtime_error(strerror(errno));
	}

	//set default baud rate
	cfsetispeed(&toptions, (speed_t)B115200);
	cfsetospeed(&toptions, (speed_t)B115200);

	toptions.c_cflag &= ~PARENB; //no parity bit
	toptions.c_cflag &= ~CSTOPB; //disable 2 stop bits (only one used instead)
	toptions.c_cflag &= ~CSIZE;//disable any previous size settings
	toptions.c_cflag |= HUPCL; //enable modem disconnect
	toptions.c_cflag |= CS8; //bytes are 8 bits long

	toptions.c_cflag &= ~CRTSCTS; //no hardware RTS/CTS switch

	toptions.c_cflag |= CREAD | CLOCAL; //enable receiving of data, forbid control lines (CLOCAL)
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); //no software flow control
	toptions.c_iflag &= ~(INLCR | ICRNL); //disable NL->CR and CR->NL

	//disable the following
	//ICANON = input is read on a per-line basis
	//ECHO/ECHOE = echo enabled
	//ISIG = interrupt signals
	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	toptions.c_oflag &= ~OPOST; //disable pre-processing of input data
	toptions.c_oflag &= ~(ONLCR | OCRNL); //disable NL->CR and CR->NL

	toptions.c_cc[VMIN] = 0;//minimum number of bytes when reading = 1
	toptions.c_cc[VTIME] = 10; //1 sec time blocking

	//TCSAFLUSH = make change after flushing i/o buffers
	if (tcsetattr(fd_, TCSAFLUSH, &toptions) < 0)
	{
	 //disconnect();
	 ::close(fd_);
	 //throw io_error(strerror(errno));
	 throw std::runtime_error(strerror(errno));
	 }
	
	
	return fd_; // needs more error checking!
}








/********************************************//**
 * \brief Closes and frees the com port.
 *
 * Once the com port is freed, other programs can use the com port if necessary.
 * \param device The device ID for the 3-Space device.
 ***********************************************/
int closeDevice(int fd_)

//*/ this will close any file descriptors that you opened in the createTSDeviceStr function.

{
	return ::close(fd_); // 0 means no error
}











//  80(0x50) streaming commands type
int setStreamingSlots(int fd_, int deviceNum, const TSS_Stream_Command * slots8)
//Configures data output slots for streaming mode. Command accepts a list of eight bytes, where each byte corresponds to a different data command. Every streaming iteration, each command will be executed in order and the resulting data will be output in the specified slot. Valid commands are commands in the ranges 0x0 – 0x10, 0x20 – 0x30, 0x40 – 0x50, 0xC9 – 0xCA (for battery-powered sensors) and 0xFA. A slot value of 0xFF 'clears' the slot and prevents any data from being written in that position. This command can fail if there is an invalid command passed in as any of the parameters or if the total allotted size is exceeded. Upon failure, all slots will be reset to 0xFF. This setting can be saved to non-volatile flash memory using the Commit Settings command.
//return length = 0 bytes
//command data length = Commands (Byte x8)

//*/ This sets the sensor slots for the wireless packet (since you can have multiple imu devices for a single wireless dongle). This is done through another send packet.

// this will have to write to the file descritptor of the dongle (not a function argument) with the dongle_ID_number (argument: device). The dongle_fd is needed for the write() while the IMU_num is needed in the byte string that is sent to the dongle which relays it to the IMU. My thinking is that I add an argument to the function that gives the dongle_fd
// length of command being sent is 1+1+1+8+1 = 12 : wireless=1 + IMU_num=1 + command=1 + command_data=8 + checksum=1
{
	unsigned char msg[12];
	
	msg[0] = 0xf8; //f8 returns success and imu num and data length; fa doesn't work
	msg[1] = (unsigned char)(deviceNum); // I've read this is bad form but I will fix it later. I don't even know if it'll work correctly
	msg[2] = 0x50;
	
	for (int i = 0; i<8; i++)
	{
		msg[i+3] = slots8[i];
	}
	
	msg[11] = calcCheckSum ( & msg[1], 10); // I tested it and the calcCheckSum portion works
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
    
    unsigned char response[3];
        
    res = ::read( fd_, &response[0], sizeof(response) ); // read the response it should be 0 - deviceNum - 0
    if ( res < sizeof(response) )
        return -2;
	
	return 0; // no error
}




//  219(0xdb) dongle command type or streaming commands type
int setStreamingHeader(int fd_, int deviceNum)
//Set wireless response header bitfield.
//return length = 0 bytes
//command data length = 4 bytes

{
	unsigned char msg[8];
	
	msg[0] = 0xf8; //f8 returns failure, f7 doesn't even send....duh wrong format
	msg[1] = (unsigned char)(deviceNum); 
	msg[2] = 0xdb;
	msg[3] = 0x00;
	msg[4] = 0x00;
	msg[5] = 0x00;
	msg[6] = 	0x01	// (1 byte ) Success/Failure, with non-zero values representing failure.
			//+	0x02	// (4 bytes) Timestamp, in microseconds.
			+	0x04	// (1 byte ) Command echo—outputs the called command. Returns 0xFF for streamed data. 
			+	0x08	// (1 byte ) Additive checksum over returned data, but not including response header.
			+	0x10	// (1 byte ) Logical ID
			//+	0x20	// (4 bytes) Serial number
			+	0x40	// (1 byte ) Data length, returns the length of the requested data, not including response header.
			;
	msg[7] = calcCheckSum ( & msg[1], 6);
	
	printf("\nHeader values cmd start\n");
	for (int i = 0; i< 8; i++)
	{
		printf("%x\n", msg[i]);
	}
	printf("Header values cmd end\n\n");
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
	
	return 0; // no error
}



//  82(0x52) streaming commands type
int setStreamingTiming(int fd_, int deviceNum, unsigned int interval, unsigned int duration, unsigned int delay)
//Configures timing information for a streaming session. All parameters are specified in microseconds. The first parameter is the interval, which specifies how often data will be output. A value of 0 means that data will be output at the end of every filter loop. Aside from 0, values lower than 1000 will be clamped to 1000. The second parameter is the duration, which specifies the length of the streaming session. If this value is set to 0xFFFFFFFF, streaming will continue indefinitely until it is stopped via command 0x56. The third parameter isthe delay, which specifies a n amount of time the sensor will wait before outputting the first packet of streaming data. This setting can be saved to non-volatile flash memory using the Commit Settings command.
//return length = 0 bytes
//command data length = 12 bytes: interval (Unsigned int), Duration (Unsigned int), Delay (Unsigned int)

//*/ This is a write to the the device. the write sends the command packet with the desired streaming timing (rate).

//similar comments as above (copied+edited)
// this will have to write to the file descritptor of the dongle (not a function argument) with the dongle_ID_number (argument: device). The dongle_fd is needed for the write() while the IMU_num is needed in the byte string that is sent to the dongle which relays it to the IMU. My thinking is that I add an argument to the function that gives the dongle_fd
// length of command being sent is 1+1+1+12+1 = 16 : wireless=1 + IMU_num=1 + command=1 + command_data=12 + checksum=1
// get to deal with endian-ness

// taken from http://forum.yeitechnology.com/viewtopic.php?f=3&t=262
 
{
	endian_swap_32( /*(unsigned int *)*/ interval ); // Flip to big endian
    endian_swap_32( /*(unsigned int *)*/ duration );
    endian_swap_32( /*(unsigned int *)*/ delay );

    unsigned char set_streaming_timing[16]; // start byte + command + data(12) + checksum

    set_streaming_timing[0]  = 0xf8; //sending file through dongle to IMU; f8 works
    set_streaming_timing[1]  = (unsigned char)(deviceNum); //the IMU slot to send to
    set_streaming_timing[2]  = 0x52; //set streaming timing

    memcpy( &set_streaming_timing[3],  &interval, sizeof( unsigned int ) );
    memcpy( &set_streaming_timing[7],  &duration, sizeof( unsigned int ) );
    memcpy( &set_streaming_timing[11], &delay,    sizeof( unsigned int ) );

    set_streaming_timing[15] = calcCheckSum( &set_streaming_timing[1], 12 + 1 );

    // Write the bytes to the serial
    int res = ::write( fd_, set_streaming_timing, sizeof( set_streaming_timing ) ); // Send to device
    if ( res < sizeof( set_streaming_timing ) )
        return -1;

    endian_swap_32( /*(unsigned int *)*/ interval ); // Flip to little endian
    endian_swap_32( /*(unsigned int *)*/ duration );
    endian_swap_32( /*(unsigned int *)*/ delay );


	unsigned char response[3];
        
    res = ::read( fd_, &response[0], sizeof(response) ); // read the response it should be 0 - deviceNum - 0
    if ( res < sizeof(response) )
        return -2;

	return 0;
}





//  85(0x55) dongle command type or streaming commands type
int startStreaming(int fd_, int deviceNum)
//Start a streaming session using the current slot and timing configuration.
//return length = 0 bytes
//command data length = 0 bytes

//*/ this will send the command to start the imu streaming data, and if you are threading it will start the read thread

{
	unsigned char msg[4];
	
	msg[0] = 0xf8;
	msg[1] = (unsigned char)(deviceNum); 
	msg[2] = 0x55;
	msg[3] = calcCheckSum ( & msg[1], 2);
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
	
	
	unsigned char response[3];
        
    res = ::read( fd_, &response[0], sizeof(response) ); // read the response it should be 0 - deviceNum - 0
    if ( res < sizeof(response) )
        return -2;
        
        
	return 0; // no error
}



//  86(0x56) dongle command type or streaming commands type
int stopStreaming(int fd_, int deviceNum)
//Stop the current streaming session. 
//return length = 0 bytes
//command data length = 0 bytes

//*/ this sends a stop command to the imu if one exists. It will also stop and merge any threads that you started to read the imu.

{
	unsigned char msg[4];
	
	msg[0] = 0xf8;
	msg[1] = (unsigned char)(deviceNum); 
	msg[2] = 0x56;
	msg[3] = calcCheckSum ( & msg[1], 2);
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
	
	unsigned char response[3];
        
    res = ::read( fd_, &response[0], sizeof(response) ); // read the response it should be 0 - deviceNum - 0
    if ( res < sizeof(response) )
        return -2;
	
	return 0; // no error
}


//  95(0x5f) streaming commands type
int updateCurrentTimestamp(int fd_, unsigned int set_timestamp)
//Set the current internal timestamp to the specified value. 
//return length = 0 bytes 
//command data length = 4 bytes: Timestamp (Unsigned int)

//*/ This will most likely be called multiple times but first in the init. This has to do with knowing the exact time that you received data from the imu (since the process is asycronous meaning the clocks on the two devices are different.


// send 0x f7 5f set_timestamp_byte1 set_timestamp_byte2 set_timestamp_byte3 set_timestamp_byte4 checksum
// timestame needs to be in big endian order and it is being sent as a wired command
// seeing as the timestamp value is 0 this can be hardcoded

{
	unsigned char msg[] = {0xf7, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x5f}; //f7 gives no error while f8 does; returns nothing
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
	
	//no response
	
	return 0; // no error
}








//  96(0x60) configuration write command type
int tareWithCurrentOrientation(int fd_, int deviceNum)
//Sets the tare orientation to be the same as the current filtered orientation.
//return length = 0 bytes
//command data length = 0 bytes

//*/ This will read the first orientation packet and then, depending on the device) either store it to set as an orientation offset, or send a tare command to the device. Both ways try to make your current orientation 0 (or 1,0,0,0 if you are using quaternions)


//similar comments as above (copied+edited)
// this will have to write to the file descritptor of the dongle (not a function argument) with the dongle_ID_number (argument: device). The dongle_fd is needed for the write() while the IMU_num is needed in the byte string that is sent to the dongle which relays it to the IMU. My thinking is that I add an argument to the function that gives the dongle_fd
// length of command being sent is 1+1+1+0+1 = 4 : wireless=1 + IMU_num=1 + command=1 + command_data=0 + checksum=1
{
	unsigned char msg[4];
	
	msg[0] = 0xf8;
	msg[1] = (unsigned char)(deviceNum); 
	msg[2] = 0x60;
	msg[3] = calcCheckSum ( & msg[1], 2);
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
	
	unsigned char response[3];
        
    res = ::read( fd_, &response[0], sizeof(response) ); // read the response it should be 0 - deviceNum - 0
    if ( res < sizeof(response) )
        return -2;
	
	return 0; // no error //making powerful assumptions :/
}





// 182(0xb6) dongle command type
int broadcastSynchronizationPulse(int fd_)
//Sends out a timestamp synchronization broadcast message to all wireless sensors that are listening on the same channel and PanID as the dongle. The message will essentially set each receiving sensor's timestamp to the same timestamp as stored in the dongle. 
//return length = 0 bytes
//command data length = 0 bytes

//*/ this is a pulse to synchonise the clocks between the computers and the device. Most people don't bother too much with this unless you have hard constraints on things like checking stale data, or hard clocked state updates (which I am guessing you are doing neither). Instead they let the packet timeout handle the timeout of the device communication.

// this will have to write to the file descritptor of the dongle
// is the format a wired command? I am treating it as such
// length of command being sent is 1+1+0+1 = 3 : wired?=1 + command=1 + command_data=0 + checksum=1

{
	unsigned char msg[] = {0xf8, 0xb6, 0xb6};
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
	
	//no response
	
	return 0; // no error
}


//0xca
int getBatteryLevel(int fd_, int deviceNum)
{
	unsigned char msg[4];
	
	msg[0] = 0xf8;
	msg[1] = (unsigned char)(deviceNum); 
	msg[2] = 0xca;
	msg[3] = calcCheckSum ( & msg[1], 2);
	
	int res = ::write( fd_, &msg[0], sizeof(msg) ); // Send to device
    if ( res < sizeof(msg) )
        return -1;
	
	unsigned char response[4];
        
    res = ::read( fd_, &response[0], sizeof(response) ); // read the response it should be 0 - deviceNum - 0
    if ( res < sizeof(response) )
        return -2;
    
	
	return (unsigned int)response[3];; 
};\



#endif // YEI_THREESPACE_API_H_INCLUDE
