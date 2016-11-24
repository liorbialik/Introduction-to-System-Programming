/*
Main.c: TestManager.exe
- This program manages all runnings of TestFiles. 
- Each file will be sent by TestManager to a set of tests, as a different process.
- The TestManager also outputs a status log file by sampling the processes every time set, noted by 'processStatusCheckFrequency'
- The testManager finishes its work only when all processes are finished running.
Tomer Shahar 301359410, Lior Bialik 301535316
*/


#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

// TestManager algorithm stages:
// 1. gets a 'fileToTest' file name and opens it.
// 2. read the fileName content and call for TestFile.exe for every file inside, as an individual process.
// 3. As an assumption that each ProcessCall creates a logFile with the specific test results for every thread, 
//    stores this file inside 'Output Files Directory'.
// 4. Create a 'runTime_logFile.txt as well inside 'Output Files Directory'. The file will include the following:
	// a. Process ID
	// b. Process Status: succeed / failed / still running.
	// c. Finally, when all processes have finished running successfully, plot an appropriate message followed by exit code.

// 5. The TestManager samples every x milisecond each process, and update its progress.
// 6. TODO: Dealing with error handles

	//	char *filePath = NULL, *outputLogFilePath = NULL;
	//	HANDLE fileHandler;
	//	FILETIME fileCreationTime;
	//	SYSTEMTIME stUTC, stLocal;
	//	
	//	filePath = argv[1];
	//	outputLogFilePath = argv[2];
	//
	//	fileHandler = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//	if (fileHandler == INVALID_HANDLE_VALUE)
	//	{
	//		printf("Could not open file, error %ul\n", GetLastError());
	//		return -1;
	//	}
	// getchar();
	return 0;
}