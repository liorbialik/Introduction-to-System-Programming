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

#define MAXCOUNT 100 // TODO: Need to check if even necessary a supremum

int main(int argc, char *argv[]) {
	FILE *fileInput = NULL, *runTime_logFileOutput = NULL;
	char *filePath = NULL; char outputFilesDirectoryPath = NULL; char *fileName = NULL; char *runTime_logFileName = { "runtime_logfile.txt" };
	char *fileToTestPnt = NULL; char *fileToTest = NULL;

	filePath = argv[1];
	outputFilesDirectoryPath = argv[2];
	fileName = getFileName(filePath);


	// read 'filesToTests' input file
	fileInput = fopen(fileName, "r");
	if (fileInput == NULL) {
		printf("Could not open file, error %ul\n", GetLastError()); //TODO: need to add an error handling function
		exit(1);
	}

	// create runTime_log File,
	// TODO: need to check how to creat a txt file inside a new directory creation
	runTime_logFileOutput = fopen(runTime_logFileName, "w");
	if (runTime_logFileOutput == NULL) {
		printf("openning run Time log file has failed!");
		exit(1);
	}

	// get the names of the files to be tested
	while (fileInput != EOF) {
		fgets(fileToTest, MAXCOUNT, fileInput);
		fileToTestPnt = fileToTest;
		// send 'file to test' name as a process to TestFile program
		printf("file name is: %s\n", fileToTest);
		getchar();
		// fileToTest++;
	}




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
// 7. 



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


char *getFileName(char *path) {
	/*
	inspired by http://stackoverflow.com/questions/5901624/extract-file-name-from-full-path-in-c-using-msvs2005
	*/

	Sleep(10);

	char *filename = strrchr(path, '\\');
	if (filename == NULL)
		filename = path;
	else
		filename++;
	return filename;
}