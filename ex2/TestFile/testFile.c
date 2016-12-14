/*
TestFile - testFile.c:
- the program recieves the following arguments from the main module:
@param filePath: the path of the file which tests will be executed on
@param OutputLogFile: the path of the log file to which the results will be written to
- the program will create a thread for each test needed to run
- each test will save the result to a designated structure
- when all the threads are done, the results will be written into the output log file
- after finishing the logging, all thread handlers are closed and their exit code noted
*/

/* Libraries: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#define DIM(x) (sizeof(x)/sizeof(*(x))) // will be used in the file's size conversion function

#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <tchar.h>
#include <Strsafe.h>
#include "testFile.h"
#pragma warning( disable: 4995 ) // disabling the deprecation warnings of 'strcpy' and 'sprintf'

/* Function Declarations: */
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);
void initializeTestResultsPointersToNull(testResults *newTestResults);
int getFileExtention(testResults *newtestResults);
int getFirstFiveCharsInFile(testResults *newtestResults);
int getFileSize(testResults *newtestResults);
int getFileCreationAndLastModifiedTime(testResults *newtestResults);
BOOL getFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString);
void writeTestResultsToFile(char *outputFilePath, testResults *newTestResults);
char *calculateSize(unsigned long long size);

int executeTestsOnFile(char *argv[])
{
	// Variables decleration:
	char *outputFilePath = NULL;
	int i;
	HANDLE threadHandles[NUM_OF_THREADS] = { 0 }; /* An array of thread handles */
	DWORD threadIDs[NUM_OF_THREADS] = { 0 }; /* An array of threadIDs */
	DWORD exitCode;
	testResults newTestResults;

	// possible thread error strings:
	const char *getFileExtentionThreadCreationError = "Problem creating 'File Extention Test' thread\n";
	const char *getFileSizeThreadCreationError = "Problem creating 'File Size Test' thread\n";
	const char *getFirstFiveCharsInFileThreadCreationError = "Problem creating 'First Five Chars Test' thread\n";
	const char *getFileCreationAndLastModifiedTimeThreadCreationError = "Problem creating 'File Creation And Last Modified Time Test' thread\n";

	// Initiating newTestResults pointers:
	initializeTestResultsPointersToNull(&newTestResults);

	// Getting the arguments given by testManager
	newTestResults.inputFileName = argv[1];
	outputFilePath = argv[2];

	// running each test on the input file in a different thread, using 'CreateThreadSimple' function from the recitation: 
	threadHandles[0] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFileExtention,              /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[0]);                                        /*  returns the thread identifier */
	if (threadHandles[0] == NULL) {
		printf("Problem creating 'File Extention Test' thread");
		newTestResults.fileExtentionThreadError = getFileExtentionThreadCreationError;
	}

	threadHandles[1] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFileSize,                   /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[1]);                                        /*  returns the thread identifier */
	if (threadHandles[1] == NULL) {
		printf("Problem creating 'File Size Test' thread");
		newTestResults.fileSizeThreadError = getFileSizeThreadCreationError;
	}

	threadHandles[2] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFirstFiveCharsInFile,       /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[2]);										   /*  returns the thread identifier */
	if (threadHandles[5] == NULL) {
		printf("Problem creating 'First Five Chars Test' thread");
		newTestResults.firstFiveCharsThreadError = getFirstFiveCharsInFileThreadCreationError;
	}

	threadHandles[3] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFileCreationAndLastModifiedTime,   /*  thread function */
		&newTestResults,                                              /*  argument to thread function */
		&threadIDs[3]);                                               /*  returns the thread identifier */
	if (threadHandles[3] == NULL) {
		printf("Problem creating 'File Creation and Last Modified Time Test' thread");
		newTestResults.fileCreationAndLastModifiedTimeStringThreadError = getFileCreationAndLastModifiedTimeThreadCreationError;
	}

	//Wait for thread to finish
	WaitForMultipleObjects(
		NUM_OF_THREADS,
		threadHandles,
		TRUE,       /* wait until all threads finish */
		INFINITE);

	Sleep(10);

	// Writing the results into the output file
	writeTestResultsToFile(outputFilePath, &newTestResults); // TODO: 

															 // Safely close all threads and print their exit code:
	for (i = 0; i < NUM_OF_THREADS; i++)
	{
		if (GetExitCodeThread(threadHandles[i], &exitCode)) {
			printf("Problem getting exit code for thread number %d", i);
		}
		else {
			printf("Thread number %d returned exit code %d\n", i, exitCode);
			CloseHandle(threadHandles[i]);
		}
	}

	return 0;
}


void initializeTestResultsPointersToNull(testResults *newTestResults) {

	newTestResults->inputFileName = NULL;
	newTestResults->fileExtention = NULL;
	newTestResults->fileCreationAndLastModifiedTimeStringThreadError = NULL;
	newTestResults->firstFiveCharsThreadError = NULL;
	newTestResults->fileSizeThreadError = NULL;
	newTestResults->fileExtentionThreadError = NULL;
	return;
}


void writeTestResultsToFile(char *outputFilePath, testResults *newTestResults) {
	/*
	@ Description: The function will write all the results recieved by the threads into the output file.
	@ Param outputFilePath: the file to which the data will be written into.
	@ Param newTestResults: the struct containing all the results.
	@ Return: None
	*/
	FILE *outputFile = NULL;
	char *normalizedFileSize = NULL;

	// creating the output file
	outputFile = fopen(outputFilePath, "w+");
	if (outputFile == NULL) {
		printf("Output log File creation failed!");
		exit(1);
	}

	// Writing the results:
	fprintf(outputFile, "%s\n", newTestResults->inputFileName);

	if (newTestResults->fileExtentionThreadError)
		fprintf(outputFile, "%s", newTestResults->fileExtentionThreadError);
	else
		fprintf(outputFile, "The file extension of the test file is \".%s\"\n", newTestResults->fileExtention);

	if (newTestResults->fileSizeThreadError)
		fprintf(outputFile, "%s", newTestResults->fileSizeThreadError);
	else
		normalizedFileSize = calculateSize(newTestResults->fileSize);
	fprintf(outputFile, "The test file size is %s\n", normalizedFileSize);

	if (newTestResults->fileCreationAndLastModifiedTimeStringThreadError)
		fprintf(outputFile, "%s", newTestResults->fileCreationAndLastModifiedTimeStringThreadError);
	else {
		fprintf(outputFile, "The file was created on %ws\n", newTestResults->fileCreationTimeString);
		fprintf(outputFile, "The file was last modified on %ws\n", newTestResults->fileLastModifiedTimeString);
	}

	if (newTestResults->firstFiveCharsThreadError)
		fprintf(outputFile, "%s", newTestResults->firstFiveCharsThreadError);
	else
		fprintf(outputFile, "The file's first 5 bytes are: %s\n", newTestResults->firstFiveChars);

	fclose(outputFile);
	return;
}


BOOL getFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString) {
	/*
	@ Description: The function recieve a FILETIME instance and convert it into a readable string in the desired format.
	inspired by https://msdn.microsoft.com/en-us/library/windows/desktop/ms724926(v=vs.85).aspx
	@ Param fileCreationTime: the FILETIME instance which will be converted.
	@ Param bufferForString: the buffer to which the out string is saved.
	@ Return: TRUE -> Success, FALSE -> Failure
	*/
	DWORD timeInStringFormatSize = 21;
	SYSTEMTIME utcTime, localTime;
	DWORD timeToStringFormatResult;

	// Convert the last-write time to local time.
	FileTimeToSystemTime(&fileCreationTime, &utcTime);
	SystemTimeToTzSpecificLocalTime(NULL, &utcTime, &localTime);

	// Build a string showing the date and time.
	timeToStringFormatResult = StringCchPrintf(bufferForString, timeInStringFormatSize,
		TEXT("%02d/%02d/%04d, %02d:%02d:%02d"),
		localTime.wDay, localTime.wMonth, localTime.wYear,
		localTime.wHour, localTime.wMinute, localTime.wSecond);

	if (timeToStringFormatResult == S_OK)
		return TRUE;
	else return FALSE;
}


int getFileExtention(testResults *newTestResults) {
	/*
	@ Description: The function will retrive the input file's extention type.
	it will save it into the relevant result structure variable.
	inspired by http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
	@ Param newTestResults: the struct containing all the results.
	@ Return: 0 -> Success, 1 -> Failure
	*/
	FILE *inputFile = NULL;
	char *dotLocationInFileName = NULL;

	Sleep(10);
	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	dotLocationInFileName = strrchr(newTestResults->inputFileName, '.');
	if (!dotLocationInFileName || dotLocationInFileName == newTestResults->inputFileName)
		return 1;
	newTestResults->fileExtention = dotLocationInFileName + 1;

	fclose(inputFile);
	return 0;
}


int getFirstFiveCharsInFile(testResults *newTestResults) {
	/*
	@ Description: The function will retrive the input file's first five characters.
	it will save it into the relevant result structure variable.
	@ Param newTestResults: the struct containing all the results.
	@ Return: 0 -> Success, 1 -> Failure
	*/
	FILE *inputFile = NULL;

	Sleep(10);
	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	// retrieving the first five chars from the input file: 
	fgets(newTestResults->firstFiveChars, 6, inputFile);

	fclose(inputFile); // Closing both file and handler

	return 0;
}


int getFileSize(testResults *newTestResults) {
	/*
	@ Description: The function will create a file handler and use the GetFileSize() function to
	retrive the input file's size. it will save it into the relevant result structure variable.
	@ Param newTestResults: the struct containing all the results.
	@ Return: 0 -> Success, 1 -> Failure
	*/

	FILE *inputFile = NULL;
	HANDLE fileHandler;
	DWORD fileSize;

	Sleep(10);
	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	// creating a file handler to use in 'GetFileSize' function:
	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	if (fileHandler == INVALID_HANDLE_VALUE) {
		printf("File Handler Error: could not open file, error %ul\n", GetLastError());
		return 1;
	}

	// getting the file's size:
	fileSize = GetFileSize(fileHandler, NULL);
	newTestResults->fileSize = fileSize;

	fclose(inputFile); // Closing both file and handler

	return 0;
}


int getFileCreationAndLastModifiedTime(testResults *newTestResults) {
	/*
	@ Description: The function will create a file handler and use the GetFileTime() function
	to retrive the input file's creation time and when it was last modified. it will save it into the
	relevant result structure variables.
	@ Param newTestResults: the struct containing all the results.
	@ Return: 0 -> Success, 1 -> Failure
	*/
	FILE *inputFile = NULL;
	HANDLE fileHandler;
	FILETIME fileCreationTime, fileLastModifiedTime;

	Sleep(10);
	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	// creating a file handler to use in 'GetFileSize' function:
	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	if (fileHandler == INVALID_HANDLE_VALUE) {
		printf("File Handler Error: could not open file, error %ul\n", GetLastError());
		return 1;
	}

	//getting file's creation time and last modified time:
	GetFileTime(fileHandler, &fileCreationTime, NULL, &fileLastModifiedTime);

	// converting the times recieved into a string
	if (getFileTimeString(fileCreationTime, newTestResults->fileCreationTimeString) == FALSE) {
		printf("Failed converting the creation time to string, error %ul\n", GetLastError());
		return 1;
	}

	if (getFileTimeString(fileLastModifiedTime, newTestResults->fileLastModifiedTimeString) == FALSE) {
		printf("Failed converting the creation time to string, error %ul\n", GetLastError());
		return 1;
	}

	fclose(inputFile); // Closing both file and handler

	return 0;
}


HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress, LPVOID ParameterPtr, LPDWORD ThreadIdPtr)
{
	/*
	@ Description: This function creates a thread by calling Win32 Api's CreateThread()
	function, and setting some of the parameters to default value.
	@ Param StartAddress: a pointer to the function that will be run by the thread
	@ Param ParameterPtr: a pointer to the parameter that will be supplied to the
	function run by the thread
	@ Param ThreadIdPtr: return argument: a pointer to a DWORD variable into which
	the function will write the created thread's ID.
	@ Return: The thread handler
	*/
	return CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		StartAddress,    /*  thread function */
		ParameterPtr,    /*  argument to thread function */
		0,               /*  use default creation flags */
		ThreadIdPtr);    /*  returns the thread identifier */
}


char *calculateSize(unsigned long long size) {
	/*
	@ Description: The function will get a size of a file in bytes and normalize it into bigger units if needed.
	inspired by: http://stackoverflow.com/questions/3898840/converting-a-number-of-bytes-into-a-file-size-in-c
	@ Param size: the size of the file in bytes.
	@ Return: the normalized size of the file as a string

	*/

	char     *sizes[] = { "EB", "PB", "TB", "GB", "MB", "KB", "Bytes" }; // list of normalized sizes
	unsigned long long  exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL; // the biggest size possible in this function
	char     *result = (char *)malloc(sizeof(char) * 20);
	unsigned long long  multiplier = exbibytes;
	int i;

	for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
	{
		if (size < multiplier)
			continue;
		if (size % multiplier == 0)                                          // check if the devision if mantissa free
			sprintf(result, "%llu %s", size / multiplier, sizes[i]);
		else
			sprintf(result, "%.2f %s", (float)size / multiplier, sizes[i]);
		return result;
	}
	strcpy(result, "0");
	return result;
}