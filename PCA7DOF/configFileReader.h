#ifndef ConfigReaderFile
#define ConfigReaderFile

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>		// atof
#include <climits>

//g++ -std=c++11 ConfigFileReader.cpp -o ConfigFileReader
#include "globalVariablesAndConstants4.h"


bool store_line( std::string key, std::string value)
{
//	printf("store_line\n");
	if (key == "IMUoffsetValue")
	{
		printf("IMUoffsetValue:\t\tfrom\t%d\tto\t",IMUoffsetValue);
		IMUoffsetValue = stoi(value);
		printf("%d\n",IMUoffsetValue);
		return true;
	}
/*	else if (key == "velocityScaleFactor")
	{
		printf("velocityScaleFactor:\tfrom\t%g\tto\t",velocityScaleFactor);
		velocityScaleFactor = stof(value);
		printf("%g\n",velocityScaleFactor);
		return true;
	}
*/	else if (key == "bufferSize")
	{
		printf("bufferSize:\t\tfrom\t%g\tto\t",bufferSize);
		bufferSize = stof(value);
		printf("%g\n",bufferSize);
		return true;
	}
	else if (key == "LOOP_FREQUENCY")
	{
		printf("LOOP_FREQUENCY:\t\tfrom\t%d\tto\t",LOOP_FREQUENCY);
		LOOP_FREQUENCY = stoi(value);
		printf("%d\n",LOOP_FREQUENCY);
		return true;
	}
	else if (key == "totalNumPCAVectors")
	{
		printf("totalNumPCAVectors:\t\tfrom\t%d\tto\t",totalNumPCAVectors);
		totalNumPCAVectors = stoi(value);
		printf("%d\n",totalNumPCAVectors);
		return true;
	}
	else if (key == "F1Gain")
	{
		printf("F1Gain:\tfrom\t%f\tto\t",F1Gain);
		F1Gain = stod(value);
		printf("%f\n",F1Gain);
		return true;
	}
	else if (key == "F2Gain")
	{
		printf("F2Gain:\tfrom\t%f\tto\t",F2Gain);
		F2Gain = stod(value);
		printf("%f\n",F2Gain);
		return true;
	}
	else if (key == "F3Gain")
	{
		printf("F3Gain:\tfrom\t%f\tto\t",F3Gain);
		F3Gain = stod(value);
		printf("%f\n",F3Gain);
		return true;
	}
	else if (key == "F4Gain")
	{
		printf("F4Gain:\tfrom\t%f\tto\t",F4Gain);
		F4Gain = stod(value);
		printf("%f\n",F4Gain);
		return true;
	}
	else if (key == "F5Gain")
	{
		printf("F5Gain:\tfrom\t%f\tto\t",F5Gain);
		F5Gain = stod(value);
		printf("%f\n",F5Gain);
		return true;
	}
	else if (key == "F6Gain")
	{
		printf("F6Gain:\tfrom\t%f\tto\t",F6Gain);
		F6Gain = stod(value);
		printf("%f\n",F6Gain);
		return true;
	}	
	else if (key == "F7Gain")
	{
		printf("F7Gain:\tfrom\t%f\tto\t",F7Gain);
		F7Gain = stod(value);
		printf("%f\n",F7Gain);
		return true;
	}
	else if (key == "CartControlOn")
	{
		printf("CartControlOn:\tfrom\t%s\tto\t", CartControlOn ? "true" : "false");
		if (value == "true")
		{
			CartControlOn = true;
			printf("true\n");
			CartXOn = CartYOn = CartZOn = true;
			return true;
		}
		else if (value == "false")
		{
			CartControlOn = false;
			printf("false\n");
			CartXOn = CartYOn = CartZOn = false;
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "RotControlOn")
	{
		printf("RotControlOn:\tfrom\t%s\tto\t", RotControlOn ? "true" : "false");
		if (value == "true")
		{
			RotControlOn = true;
			RotXOn = RotYOn = RotZOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			RotControlOn = false;
			RotXOn = RotYOn = RotZOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "CartXOn")
	{
		printf("CartXOn:\tfrom\t%s\tto\t", CartXOn ? "true" : "false");
		if (value == "true")
		{
			CartXOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			CartXOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "CartYOn")
	{
		printf("CartYOn:\tfrom\t%s\tto\t", CartYOn ? "true" : "false");
		if (value == "true")
		{
			CartYOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			CartYOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "CartZOn")
	{
		printf("CartZOn:\tfrom\t%s\tto\t", CartZOn ? "true" : "false");
		if (value == "true")
		{
			CartZOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			CartZOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "RotXOn")
	{
		printf("RotXOn:\tfrom\t%s\tto\t", RotXOn ? "true" : "false");
		if (value == "true")
		{
			RotXOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			RotXOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "RotYOn")
	{
		printf("RotYOn:\tfrom\t%s\tto\t", RotYOn ? "true" : "false");
		if (value == "true")
		{
			RotYOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			RotYOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "RotZOn")
	{
		printf("RotZOn:\tfrom\t%s\tto\t", RotZOn ? "true" : "false");
		if (value == "true")
		{
			RotZOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			RotZOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else if (key == "GrasperOn")
	{
		printf("GrasperOn:\tfrom\t%s\tto\t", GrasperOn ? "true" : "false");
		if (value == "true")
		{
			GrasperOn = true;
			printf("true\n");
			return true;
		}
		else if (value == "false")
		{
			GrasperOn = false;
			printf("false\n");
			return true;
		}
		else
		{
			printf("!!!bad input!!!\n");
			return false;
		}
		return false;
	}
	else
	{
		return false;
	}
	return false;
}


//int main(int argc, char ** argv)
bool configFileReader( const char * configFileName)
{
	//std::ifstream file("ConfigFileFullMain.txt");
	std::ifstream file(configFileName);
	
	std::stringstream is_file;
	
	is_file << file.rdbuf();
	file.close();
	
	
	std::string line;

//	printf("\n");
	
	while( std::getline(is_file, line) )
	{
//		printf("while %s\n", line.c_str());
		std::istringstream is_line(line);
		std::string is_line2;
		std::string key;
//		if( 
		std::getline(is_line, is_line2, '#') ;//)
//		printf("is_line = '%s'\n", is_line2.c_str());
//		printf("is_line2.length() = %ld\n", is_line2.length());
		
		if( is_line2.length() == 0 )
		{
			is_line.ignore( INT_MAX, '\n');
		}
		else 
		{
			std::istringstream is_line3(is_line2);
			if( std::getline(is_line3, key, '=') )
			{
	//			printf("if1 %s\n", key.c_str());
				std::string value;
				if( std::getline(is_line3, value) ) 
				{
	//				printf("if2%s\n", value.c_str());
					if(!store_line(key, value))
					{
						printf("Failed to change key=%s to value=%s\t\tExiting Program\n",key.c_str(),value.c_str());
						return 1;
					}
				}
			}
		}
		
	}
	
//	printf("goalSizeMult:\t%f\n",goalSizeMult);
//	printf("IMUOffset:\t%d\n",IMUOffset);

	
	return 0;
}

void printParameters(std::ofstream &parameterFile)
{
	parameterFile << 	"LOOP_FREQUENCY\t"		<< LOOP_FREQUENCY	<< 
						"\nF1Gain\t\t\t"		<< F1Gain			<< 
						"\nF2Gain\t\t\t" 		<< F2Gain			<< 
						"\nF3Gain\t\t\t"		<< F3Gain			<< 
						"\nF4Gain\t\t\t"		<< F4Gain			<<
						"\nF5Gain\t\t\t"		<< F5Gain			<< 
						"\nF6Gain\t\t\t"		<< F6Gain			<< 
						"\nF7Gain\t\t\t"		<< F7Gain			<<
						"\nCartControlOn\t"		<< (CartControlOn ? "true":"false")	<<
						"\nRotControlOn\t"		<< (RotControlOn ? "true":"false")	<<
						"\nCartXOn\t\t\t"		<< (CartXOn ? "true":"false")		<<
						"\nCartYOn\t\t\t"		<< (CartYOn ? "true":"false")		<<
						"\nCartZOn\t\t\t"		<< (CartZOn ? "true":"false")		<<
						"\nRotXOn\t\t\t"		<< (RotXOn ? "true":"false")		<<
						"\nRotYOn\t\t\t"		<< (RotYOn ? "true":"false")		<<
						"\nRotZOn\t\t\t"		<< (RotZOn ? "true":"false")		<<
						"\nGrasperOn\t\t"		<< (GrasperOn ? "true":"false")		<<
						
					std::endl;
	return;
}

#endif //ConfigReaderFile
