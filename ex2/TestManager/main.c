/*
Main.c: TestManager.exe
- This program manages all runnings of TestFiles.
- Each file will be sent by TestManager to a set of tests, as a different process.
- The TestManager also outputs a status log file by sampling the processes every time set, noted by 'processStatusCheckFrequency'
- The testManager finishes its work only when all processes are finished running.
Tomer Shahar 301359410, Lior Bialik 301535316
*/

#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <Strsafe.h>
#include <tchar.h>
#include <conio.h>
#include <process.h>
#include <sys/stat.h>
#include "TestFile.h"


//char *getFileName(char *path); 
char *outputLogFileArgumentCreation(char*, char*);

int main(int argc, char *argv[]) {
/*	FILE *fileInput = NULL, *runTime_logFileOutput = NULL;
	char *filePath = NULL; char *outputFilesDirectoryPath = NULL; char *fileName = NULL; char *runTime_logFileNameEnding = { "\\runtime_logfile.txt" }; char *runTime_logFileName = NULL;
	char *dirName = NULL; char *dirPath = NULL; char *outputDir = NULL;
	char *fileToTestPnt = NULL; char *fileToTest = NULL;
*/
	FILE *fileInput = NULL; FILE *runTime_logFileOutput = NULL;
	char *fileName = NULL; char *outputFileName = NULL;
	char *dirName = NULL; char *dirNameEnding = NULL; char* dirNameBeginning = NULL;
	char *dirPath = "c:\\Users\\tomershahar11\\Source\\Repos\\TAU-ITSP20162\\ex2\\TestManager\\OutputFilesDirectory" ; char *runTime_logFileNameEnding = "\\runtime_logfile.txt" ; char *runTime_logFileName = NULL;
	char *outputLogFile = NULL; char *FileToTestName = NULL;
	fileName = argv[1];
	dirName = argv[2];


	// open the InputFile by getting the file path as an argument
	fileInput = fopen(fileName, "r");
	if (fileInput == NULL) {
		printf("Could not open file, error %ul\n", GetLastError()); //TODO: need to add an error handling function
		exit(1);
	}

	//TODO: Need to concatenate beggining of path to end of path as an argument argv[2]
	LPCWSTR dirfullName = L"c:\\Users\\tomershahar11\\Source\\Repos\\TAU-ITSP20162\\ex2\\TestManager\\OutputFilesDirectory";
	HANDLE hFile;
/*	dirNameEnding = argv[2];
	dirName = (char *)malloc(1 + strlen(dirNameBeginning) + strlen(dirNameEnding));
	strcpy(dirName, dirNameBeginning);
	strcat(dirName, dirNameEnding);
*/
	// Check if output directory exist. if not create one
	if (!CreateDirectory(dirfullName, NULL)) {
		//printf("Couldn't create %S directory\nerror %ul\n", dirfullName, GetLastError());
	}

	
	// create runTime_log File inside 'OutputFilesDirectory' directory
	runTime_logFileName = (char *)malloc(1 + strlen(dirPath) + strlen(runTime_logFileNameEnding));
	if (runTime_logFileName == NULL) {
		printf("runTime_logFileName allocation failed/n");
		exit(1);
	}
	strcpy(runTime_logFileName, dirPath);
	strcat(runTime_logFileName, runTime_logFileNameEnding);
	runTime_logFileOutput = fopen(runTime_logFileName, "w");
	if (runTime_logFileOutput == NULL) {
		printf("Could not open file, error %ul\n", GetLastError()); //TODO: need to add an error handling function
		exit(1);
	}
	fclose(runTime_logFileOutput);


	// open FilesToTest and for each file create a process

	// fgetc() is inspired by http://stackoverflow.com/questions/12733105/c-function-that-counts-lines-in-file
	// go over 'FilesToTest' file and count all files to be tested
	// TODO: Need to check if every file must assign '\n' in end of every line
	char ch = NULL; int TotalNumberOfFiles = 0;
	while (!feof(fileInput)) {
		ch = fgetc(fileInput);
		if (ch == '\n') {
			TotalNumberOfFiles++;
		}
	}
	// Resetting pointer to the start of file
	rewind(fileInput);	

	// Assign FilesToTestArray in size of TotalNumberOfFiles
	int *FilesToTestLengthArray = NULL; int i;
	FilesToTestLengthArray = (int *)malloc(TotalNumberOfFiles * sizeof(int));
	for (i = 0; i < TotalNumberOfFiles; i++) {
		FilesToTestLengthArray[i] = 0;	// initialize FilesToTestArray
	}


	// go over FilesToTest and save each file's length
	i = 0; ch = NULL;
	while (i<TotalNumberOfFiles) {
		while (fgetc(fileInput) != '\n') {
			FilesToTestLengthArray[i]++;
		}
		i++;
	}		
	// Resetting pointer to the start of file
	rewind(fileInput);

	


	if (fileInput) {
		while (fscanf(fileInput, "%s", FileToTestName) != EOF) {
			outputLogFile = outputLogFileArgumentCreation(dirName, FileToTestName);
			//call TestFiles.exe
			//TestFile(*outputLogFile, *FileToTestName);
		}
			getchar();
	}
		fclose(fileInput);
	

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
	fclose(fileInput);

	// free all allocated memories
	free(FilesToTestLengthArray);
	free(runTime_logFileName);

	return 0;
}

char *outputLogFileArgumentCreation(char* dirName, char* fileToTestName) {
	// prepare arguments for calling 'TestFiles' program
	char *outputLogFileNameEnding = { "_log.txt" }; char *outputLogFileName = NULL; size_t outputLogFileNameLength = 0;
	
	outputLogFileNameLength = strlen(dirName) + strlen(fileToTestName) + strlen(outputLogFileNameEnding) - 4 + 2 + 1;
	// '-4' due to deduction of '.txt' ending since it appearse twice, '-2' due to addition of '\\', '+1' due to '\0' ending
	outputLogFileName = malloc(sizeof(char) * outputLogFileNameLength); 
	if (outputLogFileName == NULL) {
		printf("outputLogFileName allocation failed/n");
		exit(1);
	}
	strcpy(outputLogFileName, dirName);
	strcat(outputLogFileName, "\\");
	strcat(outputLogFileName, fileToTestName);
	outputLogFileName[strlen(outputLogFileName) - 4] = '\0'; // mark the end of the precious string before the '.txt' ending
	strcat(outputLogFileName, outputLogFileNameEnding); // add the correct ending

	return outputLogFileName;
}

//char *getFileName(char *path) {
//	/*
//	inspired by http://stackoverflow.com/questions/5901624/extract-file-name-from-full-path-in-c-using-msvs2005
//	*/
//
//	Sleep(10);
//
//	char *filename = strrchr(path, '\\');
//	if (filename == NULL)
//		filename = path;
//	else
//		filename++;
//	return filename;
//}


// TestManager algorithm flow:
// 1. gets a 'fileToTest' file name and opens it.
// 2. for every fileTest in the content file:
//		a. going over the line, reads it and allocates a pointer for the specific line (Dynamic allocation). The program will hold array of pointers that its size will be defined dynamically.
//		b. check who is opening s process? testfile.exe or testmanager.exe when getting a fileToTestName
//		c. call for TestFile.exe for every line (process). The program should send two arguments: <FileTestName>.txt, <OutputFilesDirectory>\\<FileTestName>_log.txt.
//		d. End condition will be the char EOF. by then we will count all '\n' values and that will deter the array size.

// 3. As an assumption that each ProcessCall creates a logFile with the specific test results for every thread, 
//    stores this file inside 'Output Files Directory'.
// 4. Create a 'runTime_logFile.txt as well inside 'Output Files Directory'. The file will include the following:
//		a. Process ID
//		b. Process Status: succeed / failed / still running.
//		c. Finally, when all processes have finished running successfully, plot an appropriate message followed by exit code.

// 5. The TestManager samples every x milisecond each process, and update its progress.
// 6. TODO: Dealing with error handles
// 7. 
