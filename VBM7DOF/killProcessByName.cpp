// Code slightly adapted from:
// https://proswdev.blogspot.com/2012/02/get-process-id-by-name-in-linux-using-c.html?showComment=1467049723643#c2985745019452068063



#include <dirent.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

// g++ -std=c++11 killProcessByName.cpp -o killProcessByName

using namespace std;

int getProcIdByName(string procName)
{
    int pid = -1;

    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != NULL)
    {
        // Enumerate all entries in directory until process found
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp)))
        {
            // Skip non-numeric entries
            int id = atoi(dirp->d_name);
            if (id > 0)
            {
                // Read contents of virtual /proc/{pid}/cmdline file
                string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
                ifstream cmdFile(cmdPath.c_str());
                string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(pos + 1);
                    // Compare against requested process name
                    if (procName == cmdLine)
                        pid = id;
                }
            }
        }
    }

    closedir(dp);

    return pid;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
    	printf("Usage is \"./killProcessByName <name of process to be killed>\"\n");
    	return 1;
    }
    
    // Fancy command line processing skipped for brevity
    int pid = getProcIdByName(argv[1]);
    if (pid != -1)
    {
        kill(pid,10);
        printf("Sent signal 10 to process of name \"%s\"\n",argv[1]);
    }
    else
    {
    	printf("Process of name \"%s\" not found\n",argv[1]);	
    }
    return 0;
}
