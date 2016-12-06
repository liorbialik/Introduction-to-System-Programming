/*
TestFile - Main.c:
- the program recieves the following arguments from testManager.exe:
@param filePath: the path of the file which tests will be executed on
@param OutputLogFile: the path of the log file to which the results will be written to

- the program will create a thread for each test needed to run
- each test will save the result to a designated structure
- when all the threads are done, the results will be written into the output log file
- after finishing the logging, all thread handlers are closed and their exit code noted
*/


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
#define NUM_OF_THREADS 5

/* Types: */
typedef struct testResults
{
	char *inputFileName;
	char *fileExtention;
	DWORD fileSize;
	TCHAR fileCreationTimeString[21];
	TCHAR fileLastModifiedTimeString[21];
	char firstFiveChars[6];
} testResults;


/* Function Declarations: */
HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);
int getFileExtention(testResults *newtestResults);
int getFirstFiveCharsInFile(testResults *newtestResults);
int getFileSize(testResults *newtestResults);
int getFileCreationTime(testResults *newtestResults);
int getFileLastModifiedTime(testResults *newtestResults);
BOOL getFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString);
void writeTestResultsToFile(char *outputFilePath, testResults *newTestResults);


int main(int argc, char *argv[]) {
	char *outputFilePath = NULL;
	int i;
	HANDLE threadHandles[NUM_OF_THREADS] = { 0 }; /* An array of thread handles */
	DWORD threadIDs[NUM_OF_THREADS] = { 0 }; /* An array of threadIDs */
	DWORD exitCode;
	testResults newTestResults;

	// Initiating newTestResults pointers:
	newTestResults.inputFileName = NULL;
	newTestResults.fileExtention = NULL;

	// Getting the arguments given by testManager
	newTestResults.inputFileName = argv[1];
	outputFilePath = argv[2];

	// running each test on the input file in a different thread, using 'CreateThreadSimple' function from the recitation: 

	threadHandles[0] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFileExtention,              /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[0]);                                        /*  returns the thread identifier */

	threadHandles[1] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFileSize,                   /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[1]);                                        /*  returns the thread identifier */

	threadHandles[2] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFirstFiveCharsInFile,       /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[2]);										   /*  returns the thread identifier */

	threadHandles[3] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFileCreationTime,           /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[3]);                                        /*  returns the thread identifier */

	threadHandles[4] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)getFileLastModifiedTime,       /*  thread function */
		&newTestResults,                                       /*  argument to thread function */
		&threadIDs[4]);                                        /*  returns the thread identifier */

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
		GetExitCodeThread(threadHandles[i], &exitCode);
		printf("Thread number %d returned exit code %d\n", i, exitCode);
		CloseHandle(threadHandles[i]);
	}

	return 0;
}


void writeTestResultsToFile(char *outputFilePath, testResults *newTestResults) {
	/*
	@ Description: The function will write all the results recieved by the threads into the output file.
	@ Param outputFilePath: the file to which the data will be written into.
	@ Param newTestResults: the struct containing all the results.
	@ Return: None
	*/
	FILE *outputFile = NULL;

	// creating the output file
	outputFile = fopen(outputFilePath, "w+");
	if (outputFile == NULL) {
		printf("Output log File creation failed!");
		exit(1);
	}

	// Writing the results:
	fprintf(outputFile, "%s\n", newTestResults->inputFileName);
	fprintf(outputFile, "The file extension of the test file is \".%s\"\n", newTestResults->fileExtention);
	fprintf(outputFile, "The test file size is %ld bytes\n", newTestResults->fileSize);
	fprintf(outputFile, "The file was created on %ws\n", newTestResults->fileCreationTimeString);
	fprintf(outputFile, "The file was last modified on %ws\n", newTestResults->fileLastModifiedTimeString);
	fprintf(outputFile, "The files first 5 bytes are: %s\n", newTestResults->firstFiveChars);

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
		TEXT("%02d/%02d/%04d  %02d:%02d:%02d"),
		localTime.wMonth, localTime.wDay, localTime.wYear,
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
	Sleep(10);

	FILE *inputFile = NULL;
	char *fileExtention = NULL;

	inputFile = fopen(newTestResults->inputFileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		return 1;
	}

	char *dotLocationInFileName = strrchr(newTestResults->inputFileName, '.');
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
	Sleep(10);

	FILE *inputFile = NULL;

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
	Sleep(10);

	FILE *inputFile = NULL;
	HANDLE fileHandler;
	DWORD fileSize;

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


int getFileCreationTime(testResults *newTestResults) {
	/*
	@ Description: The function will create a file handler and use the GetFileTime() function
	to retrive the input file's creation time. it will save it into the
	relevant result structure variable.
	@ Param newTestResults: the struct containing all the results.
	@ Return: 0 -> Success, 1 -> Failure
	*/
	Sleep(10);

	FILE *inputFile = NULL;
	HANDLE fileHandler;
	FILETIME fileCreationTime;

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
	GetFileTime(fileHandler, &fileCreationTime, NULL, NULL);

	if (getFileTimeString(fileCreationTime, newTestResults->fileCreationTimeString) == FALSE) {
		printf("Failed converting the creation time to string, error %ul\n", GetLastError());
		return 1;
	}

	fclose(inputFile); // Closing both file and handler

	return 0;
}


int getFileLastModifiedTime(testResults *newTestResults) {
	/*
	@ Description: The function will create a file handler and use the GetFileTime() function to
	retrive the input file's time when it was last modified. it will save it into
	the relevant result structure variable.
	@ Param newTestResults: the struct containing all the results.
	@ Return: 0 -> Success, 1 -> Failure
	*/
	Sleep(10);

	FILE *inputFile = NULL;
	HANDLE fileHandler;
	FILETIME fileLastModifiedTime;

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
	GetFileTime(fileHandler, NULL, NULL, &fileLastModifiedTime);

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

