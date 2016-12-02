
/* Libraries: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <Strsafe.h>
#include <tchar.h>


/* Constants: */
#define NUM_OF_THREADS 6

/* Types: */
typedef struct testResults
{
	char *inputFileName;
	char *outputFilePath;
} testResults;


/* Function Declarations: */
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);
int logFileName(testResults *newtestResults);
int logFileExtention(testResults *newtestResults);
int logFirstFiveCharsInFile(testResults *newtestResults);
int logFileSize(testResults *newtestResults);
int logFileCreationTime(testResults *newtestResults);
int logFileLastModifiedTime(testResults *newtestResults);
BOOL GetFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString);

void writeTestResultsToFile(FILE *outputFile, char *fileName, char *fileExtention, DWORD fileSize,
							TCHAR *creationTimeStringPtr, TCHAR *fileLastModifiedTimeString, char *firstFiveCharsPtr);



int main(int argc, char *argv[]) {
	FILE *outputFile = NULL;

	int i;
	HANDLE threadHandles[NUM_OF_THREADS] = { 0 }; /* An array of thread handles */
	DWORD threadIDs[NUM_OF_THREADS] = { 0 }; /* An array of threadIDs */
	DWORD exitCode;
	testResults newTestResults;
	

	newTestResults.inputFileName = argv[1];
	newTestResults.outputFilePath = argv[2];


	// creating the output file, and in case it exists, it will discarded and the file is treated as a new empty file.
	outputFile = fopen(newTestResults.outputFilePath, "w+");
	if (outputFile == NULL) {
		printf("Output log File creation failed!");
		exit(1);
	}
	fclose(outputFile);

	// running each test on the input file in a different thread: 

	threadHandles[0] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)logFileName,                   /*  thread function */
		&newTestResults,                                         /*  argument to thread function */
		&threadIDs[0]);                                        /*  returns the thread identifier */

	threadHandles[1] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)logFileExtention,              /*  thread function */
		&newTestResults,                                         /*  argument to thread function */
		&threadIDs[1]);                                        /*  returns the thread identifier */

	threadHandles[2] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)logFirstFiveCharsInFile,       /*  thread function */
		&newTestResults,                                         /*  argument to thread function */
		&threadIDs[2]);										   /*  returns the thread identifier */

	threadHandles[3] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)logFileSize,                   /*  thread function */
		&newTestResults,                                         /*  argument to thread function */
		&threadIDs[3]);                                        /*  returns the thread identifier */

	threadHandles[4] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)logFileCreationTime,           /*  thread function */
		&newTestResults,                                         /*  argument to thread function */
		&threadIDs[4]);                                        /*  returns the thread identifier */

	threadHandles[5] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)logFileLastModifiedTime,           /*  thread function */
		&newTestResults,                                         /*  argument to thread function */
		&threadIDs[5]);                                        /*  returns the thread identifier */

	// Wait for thread to finish
	WaitForMultipleObjects(
		NUM_OF_THREADS,
		threadHandles,
		TRUE,       /* wait until all threads finish */
		INFINITE);

	Sleep(10);

	// Safely close all threads and print their exit code:
	for (i = 0; i < NUM_OF_THREADS; i++)
	{
		GetExitCodeThread(threadHandles[i], &exitCode);
		printf("Thread number %d returned exit code %d\n", i, exitCode);
		CloseHandle(threadHandles[i]);
	}

	getchar();

	////getting file's creating time and last accessed time
	//GetFileTime( fileHandler, &fileCreationTime, NULL, &fileLastModifiedTime );
	//
	//if (GetFileTimeString(fileCreationTime, fileCreationTimeString)) {
	//	_tprintf(TEXT("file creation time is: %s\n"), fileCreationTimeString); // REMOVE - just for testing
	//	creationTimeStringPtr = fileCreationTimeString;
	//}

	//if (GetFileTimeString(fileLastModifiedTime, fileLastModifiedTimeString)) {
	//	_tprintf(TEXT("file last accessed time is: %s\n"), fileLastModifiedTimeString); // REMOVE - just for testing
	//	lastModifiedTimeStringPtr = fileLastModifiedTimeString;
	//}

	//writeTestResultsToFile(outputFile, fileName, fileExtention, fileSize, creationTimeStringPtr, fileLastModifiedTimeString, firstFiveCharsPtr);

	//free( outputLogFileName );
	//fclose(inputFile); // this closes both the FILE and the HANDLE of the input file
	return 0;
}


void writeTestResultsToFile(FILE *outputFile, char *fileName, char *fileExtention, DWORD fileSize,
							TCHAR *creationTimeStringPtr, TCHAR *fileLastModifiedTimeString, char *firstFiveCharsPtr) {
	
	
	//fprintf(outputFile, "%s\n", fileName);
	//fprintf(outputFile, "The file extension of the test file is \".%s\"\n", fileExtention);
	//fprintf(outputFile, "The test file size is %d bytes\n", fileSize);
	//fprintf(outputFile, "The file was created on %ws\n", creationTimeStringPtr);
	//fprintf(outputFile, "The file was last modified on %ws\n", fileLastModifiedTimeString);
	//fprintf(outputFile, "The files first 5 bytes are: %s\n", firstFiveCharsPtr);

	fclose(outputFile);
	return;
}


BOOL GetFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString){
	/* 
	inspired by https://msdn.microsoft.com/en-us/library/windows/desktop/ms724926(v=vs.85).aspx 
	*/

	DWORD timeInStringFormatSize = 21;
	SYSTEMTIME utcTime, localTime;
	DWORD timeToStringFormatResult;
	
	// Convert the last-write time to local time.
	FileTimeToSystemTime( &fileCreationTime, &utcTime );
	SystemTimeToTzSpecificLocalTime( NULL, &utcTime, &localTime );

	// Build a string showing the date and time.
	timeToStringFormatResult = StringCchPrintf( bufferForString, timeInStringFormatSize,
		TEXT("%02d/%02d/%04d  %02d:%02d:%02d"),
		localTime.wMonth, localTime.wDay, localTime.wYear,
		localTime.wHour, localTime.wMinute, localTime.wSecond );

	if (timeToStringFormatResult == S_OK )
		return TRUE;
	else return FALSE;
}


int logFileExtention(testResults *newTestResults) {
	/*
	inspired by http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
	*/
	Sleep(10);

	FILE *inputFile = NULL, *outputFile = NULL;
	char *fileExtention = NULL;

	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	outputFile = fopen(newTestResults->outputFilePath, "a");
	if (outputFile == NULL) {
		printf("Output log File openning failed!");
		return 1;
	}

	char *dotLocationInFileName = strrchr(newTestResults->inputFileName, '.');
	if (!dotLocationInFileName || dotLocationInFileName == newTestResults->inputFileName)
		return 1;
	fileExtention = dotLocationInFileName + 1;

	// writing the result into the output log file:
	fprintf(outputFile, "The file extension of the test file is \".%s\"\n", fileExtention);
	
	fclose(inputFile);
	fclose(outputFile);
	return 0;
}


int logFileName(testResults *newTestResults){
	Sleep(10);

	FILE *outputFile = NULL;

	outputFile = fopen(newTestResults->outputFilePath, "a");
	if (outputFile == NULL) {
		printf("Output log File openning failed!");
		return 1;
	}

	// writing the result into the output log file:
	fprintf(outputFile, "%s\n", newTestResults->inputFileName);
	
	fclose(outputFile);
	return 0;
}


int logFirstFiveCharsInFile(testResults *newTestResults) {
	Sleep(10);

	FILE *inputFile = NULL, *outputFile = NULL;
	char firstFiveCharsFromFile[6];

	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	outputFile = fopen(newTestResults->outputFilePath, "a");
	if (outputFile == NULL) {
		printf("Output log File openning failed!");
		return 1;
	}

	
	fgets(firstFiveCharsFromFile, 6, inputFile);

	// writing the result into the output log file:
	fprintf(outputFile, "The files first 5 bytes are: %s\n", firstFiveCharsFromFile);

	fclose(inputFile);
	fclose(outputFile);
	return 0;
}


int logFileSize(testResults *newTestResults) {
	Sleep(10);

	FILE *inputFile = NULL, *outputFile = NULL;
	HANDLE fileHandler;
	DWORD fileSize = 0;

	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	outputFile = fopen(newTestResults->outputFilePath, "a");
	if (outputFile == NULL) {
		printf("Output log File openning failed!");
		return 1;
	}

	// creating a file handler to use in 'GetFileSize' function:
	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	if ( fileHandler == INVALID_HANDLE_VALUE ) {
		printf("File Handler Error: could not open file, error %ul\n", GetLastError());
		return 1;
	}

	// getting the file's size:
	fileSize = GetFileSize( fileHandler, NULL );

	// writing the result into the output log file:
	fprintf(outputFile, "The test file size is %d bytes\n", fileSize);

	fclose(inputFile);
	fclose(outputFile);
	return 0;
}


int logFileCreationTime(testResults *newTestResults) {
	Sleep(10);

	FILE *inputFile = NULL, *outputFile = NULL;
	HANDLE fileHandler;
	FILETIME fileCreationTime; //, fileLastModifiedTime;
	TCHAR fileCreationTimeString[21];

	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	outputFile = fopen(newTestResults->outputFilePath, "a");
	if (outputFile == NULL) {
		printf("Output log File openning failed!");
		return 1;
	}

	// creating a file handler to use in 'GetFileSize' function:
	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	if (fileHandler == INVALID_HANDLE_VALUE) {
		printf("File Handler Error: could not open file, error %ul\n", GetLastError());
		return 1;
	}

	//getting file's creation time and last modified time:
	GetFileTime(fileHandler, &fileCreationTime, NULL, NULL);

	if (GetFileTimeString(fileCreationTime, fileCreationTimeString) == FALSE) {
		printf("Failed converting the creation time to string, error %ul\n", GetLastError());
		return 1;
	}

	// writing the result into the output log file:
	fprintf(outputFile, "The file was created on %ws\n", fileCreationTimeString);

	fclose(inputFile);
	fclose(outputFile);
	return 0;
}


int logFileLastModifiedTime(testResults *newTestResults) {
	Sleep(10);

	FILE *inputFile = NULL, *outputFile = NULL;
	HANDLE fileHandler;
	FILETIME fileLastModifiedTime;
	TCHAR fileLastModifiedTimeString[21];

	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	outputFile = fopen(newTestResults->outputFilePath, "a");
	if (outputFile == NULL) {
		printf("Output log File openning failed!");
		return 1;
	}

	// creating a file handler to use in 'GetFileSize' function:
	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	if (fileHandler == INVALID_HANDLE_VALUE) {
		printf("File Handler Error: could not open file, error %ul\n", GetLastError());
		return 1;
	}

	//getting file's creation time and last modified time:
	GetFileTime(fileHandler, NULL, NULL, &fileLastModifiedTime);

	if (GetFileTimeString(fileLastModifiedTime, fileLastModifiedTimeString) == FALSE) {
		printf("Failed converting the creation time to string, error %ul\n", GetLastError());
		return 1;
	}

	// writing the result into the output log file:
	fprintf(outputFile, "The file was last modified on %ws\n", fileLastModifiedTimeString);

	fclose(inputFile);
	fclose(outputFile);
	return 0;
}


HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr)
{
	return CreateThread(
		NULL,            /*  default security attributes */
		0,                /*  use default stack size */
		StartAddress,    /*  thread function */
		ParameterPtr,    /*  argument to thread function */
		0,                /*  use default creation flags */
		ThreadIdPtr);    /*  returns the thread identifier */
}


// errorHandling(){
//    free all allocations
//    free all threads
//    close all files
//    print the error on the screen -> use GetLastError()
//    write the error into the outputFile
//    in case of an error in one of the threads, should have a dictionary that prints an error into the outputfile for the relevant thread
// }
