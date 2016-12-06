/*
Main.c: TestManager.exe
- This program manages all runnings of TestFiles.
- Each file will be sent by TestManager to a set of tests, as a different process.
- The TestManager also outputs a status log file by sampling the processes every time set, noted by 'processStatusCheckFrequency'
- The testManager finishes its work only when all processes are finished running.
Tomer Shahar 301359410, Lior Bialik 301535316
*/


/* Constants: */
#define TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55

/* Libraries: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <tchar.h>
#include <Strsafe.h>
#include <conio.h>
#include <process.h>
#include <sys/stat.h>
#include "TestFile.h"
#include "TestManager.h"
#include <sys/types.h>
#include <direct.h>


/* Function Declarations: */
char *outputLogFileArgumentCreation(char*, char*);
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr);
char *createRunTimeLogFileInsideOutputDirectory(char*, char*);
LPTSTR ConvertCharStringToLPTSTR(const char *Source);
void checkProcessStatus(DWORD waitcode, FILE *runTime_logFileOutput, PROCESS_INFORMATION *procinfo, HANDLE *handleProcessArray, DWORD *exitcodeArray, int NumberOfProcess);
//void printListOfProcess(HANDLE *handleProcessArray, DWORD *exitcodeArray);
void *getDWORDMallocArray(DWORD *nameArray, int MUL);


int main(int argc, char *argv[]) {
	FILE *fileInput = NULL; FILE *runTime_logFileOutput = NULL;
	char *fileName = NULL; char *outputFileName = NULL;
	char *dirName = NULL; LPTSTR dirNameLPTSTR = NULL; char *loopTime = NULL;
	char *runTime_logFileName = NULL;
	char *outputLogFile = NULL; char *FileToTestName = NULL;
	fileName = argv[1];
	dirName = argv[2];
	loopTime = argv[3];
	PROCESS_INFORMATION *procinfo;
	DWORD				waitcode;
	DWORD				*exitcodeArray;
	BOOL				retVal;
	LPTSTR				command;
	DWORD				ProcessStatusCheckFrequency = (int)loopTime;
	HANDLE				*handleProcessArray;


	const FILETIME		*FileTime;
	LPSYSTEMTIME		*SystemTime = NULL;



	// Verify that the number of command line argument is correct
	if (argc != 4) {
		printf("Number of Command line Arguments isn't compatible,  error %ul\n", GetLastError());
		exit(1);
	}


	// open the FileInput by getting the file Name as an argument
	fileInput = fopen(fileName, "r");
	if (fileInput == NULL) {
		printf("Could not open file, error %ul\n", GetLastError());
		exit(1);
	}


	// Create directory if not exist
	mkdir(dirName);


	runTime_logFileOutput = fopen(createRunTimeLogFileInsideOutputDirectory(dirName, NULL), "w");
	if (runTime_logFileOutput == NULL) {
		printf("failed to open runTime_logFile, error %ul\n", GetLastError()); //TODO: need to add an error handling function
		exit(1);
	}


	// go over 'FilesToTest' file and count all files to be tested
	// fgetc() is inspired by http://stackoverflow.com/questions/12733105/c-function-that-counts-lines-in-file
	char ch = NULL; int TotalNumberOfFiles = 0;
	while (!feof(fileInput)) {
		ch = fgetc(fileInput);
		if (ch == '\n') {
			TotalNumberOfFiles++;
		}
	}
	// Resetting pointer to the start of file
	rewind(fileInput);


	// Assign *FilesToTestArray in size of TotalNumberOfFiles
	int *FilesToTestLengthArray = NULL; int i;
	FilesToTestLengthArray = (int *)malloc(TotalNumberOfFiles * sizeof(int));
	if (FilesToTestLengthArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		FilesToTestLengthArray[i] = 0;	// initialize FilesToTestArray
	}
	// go over FilesToTestArray and save each test's length
	for (i = 0; i<TotalNumberOfFiles; i++) {
		while (fgetc(fileInput) != '\n') {
			FilesToTestLengthArray[i]++;
		}
	}
	// Resetting pointer to the start of file
	rewind(fileInput);


	// Assign *CommandLineArguentStringArray[] in size of TotalNumberOfFiles, an array of pointers holding the commandLineArgumentString of eact test
	// concatenated all parameters to a single string


	// TODO: Move to declerations
	char *CommandLineArguentStringArray[] = { NULL }; char *TestFileProgramName = { "main.exe " };
	char *TestFileArgumentString = NULL; char *fileToTest = NULL;
	procinfo = (PROCESS_INFORMATION*)malloc(TotalNumberOfFiles * sizeof(PROCESS_INFORMATION));
	int numOfRunningProcess = 0;
	if (procinfo == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}


	//Dynamic memory allocations
	handleProcessArray = (HANDLE*)malloc(TotalNumberOfFiles * sizeof(HANDLE));
	if (handleProcessArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		handleProcessArray[i] = 0;	// initialize handleProcessArray
	}
	FileTime = (const FILETIME*)malloc(TotalNumberOfFiles * sizeof(const FILETIME));
	if (FileTime == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	//for (i = 0; i < TotalNumberOfFiles; i++) {
	//	FileTime[i] = NULL;	// initialize handleProcessArray
	//}
	//FileTimeToSystemTime(FileTime, SystemTime);
	getDWORDMallocArray(exitcodeArray, TotalNumberOfFiles);




	for (i = 0; i < TotalNumberOfFiles; i++) {
		fileToTest = (char *)malloc(FilesToTestLengthArray[i] * sizeof(char));
		if (fileToTest == NULL) {
			printf("allocation was failed, error %ul\n", GetLastError());
		}
		if (fgets(fileToTest, 1 + FilesToTestLengthArray[i], fileInput) == NULL) {
			printf("reading a string from fileInput was failed, error %ul\n", GetLastError());
		}
		fgetc(fileInput);
		outputLogFile = outputLogFileArgumentCreation(dirName, fileToTest);
		CommandLineArguentStringArray[i] = (char *)malloc(((FilesToTestLengthArray[i]) + 14 + strlen(outputLogFile)) * sizeof(char));
		// 14 for the const "FileTest.exe" and for two spaces
		if (CommandLineArguentStringArray[i] == NULL) {
			printf("allocation was failed, error %ul\n", GetLastError());
		}
		strcpy(CommandLineArguentStringArray[i], TestFileProgramName);
		strcat(CommandLineArguentStringArray[i], fileToTest);
		strcat(CommandLineArguentStringArray[i], " ");
		strcat(CommandLineArguentStringArray[i], outputLogFile);


		command = ConvertCharStringToLPTSTR(CommandLineArguentStringArray[i]);
		retVal = CreateProcessSimple(command, &procinfo[i]);
		if (retVal == 0) {
			printf("!!! Failed to create new process to run %s. Error code: %d !!!\n", fileToTest, GetLastError());
			fprintf(runTime_logFileOutput, "!!! Failed to create new process to run %s. Error code: %d !!!\n", fileToTest, GetLastError());
			// TODO: Need to output hex rep. error code
		}
		else {
			fprintf(runTime_logFileOutput, "Successfully created a process with ID %d to execute %s\n", procinfo->dwProcessId, fileToTest);
			// save handleProcess that were succedded in an handleProcessArray
			handleProcessArray[i] = procinfo->hProcess;
		}
	}


	for (i = 0; i<TotalNumberOfFiles; i++) {
		if (handleProcessArray[i] != 0) {
			numOfRunningProcess++;
		}
	}
	// TODO: call organizeArray for handleProcessArray


	do {
		waitcode = WaitForMultipleObjects(numOfRunningProcess, handleProcessArray, 1, ProcessStatusCheckFrequency);
		// analyze results into runTime_logFile
		checkProcessStatus(waitcode, runTime_logFileOutput, procinfo, handleProcessArray, exitcodeArray, numOfRunningProcess);


	} while (numOfRunningProcess > 0);


	//All the processes have finished running. Exiting program
	fprintf(runTime_logFileOutput, "All the processes have finished running. Exiting program\n");


	CloseHandle(procinfo->hProcess); /* Closing the handle to the process */
	CloseHandle(procinfo->hThread); /* Closing the handle to the main thread of the process */
	fclose(fileInput);
	fclose(runTime_logFileOutput);


	// free all allocated memories
	free(FilesToTestLengthArray);
	free(runTime_logFileName);
	for (i = 0; i < TotalNumberOfFiles; i++) {
		free(CommandLineArguentStringArray[i]);
	}
	return 0;
}


char *createRunTimeLogFileInsideOutputDirectory(char *dirName, char *runTime_logFileName) {
	// create runTime_log File inside <OutputFilesDirectory> directory
	char *runTime_logFileNameEnding = "\\runtime_logfile.txt"; char *runTime_logFileOutput = NULL;
	runTime_logFileName = (char *)malloc(1 + strlen(dirName) + strlen(runTime_logFileNameEnding));
	if (runTime_logFileName == NULL) {
		printf("runTime_logFileName allocation failed/n");
		exit(1);
	}
	strcpy(runTime_logFileName, dirName);
	strcat(runTime_logFileName, runTime_logFileNameEnding);


	return runTime_logFileName;
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

LPTSTR ConvertCharStringToLPTSTR(const char *Source) {
	/* the win32 API LPTSTR string type is defined in one of two ways, */
	/* as a simple char string or as a wide-character (unicode) string.*/
	/* If the second case is true, the macro UNICODE should be defined. */


#ifdef UNICODE     
#define STR_COPY_FUNCTION mbstowcs /* converts a simple char string */
	/* to a wide char string */
	typedef size_t CopyFunctionOutput_t;
#else
#define STR_COPY_FUNCTION strncpy 
	typedef char *CopyFunctionOutput_t;
#endif


	TCHAR *Dest = NULL;
	CopyFunctionOutput_t CopyFunctionOutput;
	BOOL CopyFunctionSucceeded;
	size_t NumOfLettersInSource;
	size_t LengthOfSourceIncludingTerminatingZero;


	if (Source == NULL)
		return NULL;


	NumOfLettersInSource = strlen(Source);
	LengthOfSourceIncludingTerminatingZero = NumOfLettersInSource + 1;


	Dest = (TCHAR*)malloc(sizeof(TCHAR) * LengthOfSourceIncludingTerminatingZero);


	CopyFunctionOutput = STR_COPY_FUNCTION(
		Dest,
		Source,
		LengthOfSourceIncludingTerminatingZero);


	/* Add terminating zero: */
	Dest[LengthOfSourceIncludingTerminatingZero - 1] = _T('\0');


#ifdef UNICODE     
	CopyFunctionSucceeded = (CopyFunctionOutput == NumOfLettersInSource);
#else        
	CopyFunctionSucceeded = STRINGS_ARE_IDENTICAL(Dest, Source);
#endif


	if (!CopyFunctionSucceeded)
	{
		free(Dest);
		return NULL;
	}


	return (LPTSTR)Dest;


#undef STR_COPY_FUNCTION 
}

BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr)
{
	STARTUPINFO	startinfo = { sizeof(STARTUPINFO), NULL, 0 }; /* <ISP> here we */
															  /* initialize a "Neutral" STARTUPINFO variable. Supplying this to */
															  /* CreateProcess() means we have no special interest in this parameter. */
															  /* This is equivalent to what we are doing by supplying NULL to most other */
															  /* parameters of CreateProcess(). */


	return CreateProcess(NULL, /*  No module name (use command line). */
		CommandLine,			/*  Command line. */
		NULL,					/*  Process handle not inheritable. */
		NULL,					/*  Thread handle not inheritable. */
		FALSE,					/*  Set handle inheritance to FALSE. */
		NORMAL_PRIORITY_CLASS,	/*  creation/priority flags. */
		NULL,					/*  Use parent's environment block. */
		NULL,					/*  Use parent's starting directory. */
		&startinfo,				/*  Pointer to STARTUPINFO structure. */
		ProcessInfoPtr			/*  Pointer to PROCESS_INFORMATION structure. */
	);
}

void checkProcessStatus(DWORD waitcode, FILE *runTime_logFileOutput, PROCESS_INFORMATION *procinfo, HANDLE *handleProcessArray, DWORD *exitcodeArray, int numOfRunningProcess) {
	//Check status of processes and print to runTime_logFile
	//printf("WaitForSingleObject output: ");
	int i; DWORD exitCode = 0; DWORD *finishedProcessArray; DWORD *runningProcessArray;
	getDWORDMallocArray(finishedProcessArray, numOfRunningProcess);
	getDWORDMallocArray(runningProcessArray, numOfRunningProcess);

	switch (waitcode)
	{
	case WAIT_TIMEOUT:
		printf("WAIT_TIMEOUT\n"); break;
	case WAIT_FAILED:
		printf("WaitForMultipleObjects function has failed, error %ul\n", GetLastError()); break;
	default:
		printf("0x%x\n", waitcode);
	}


	if (waitcode == WAIT_TIMEOUT) { /* Processes is still alive */
									//go over handleProcessArray and getExitCodeProcess
		for (i = 0; i<numOfRunningProcess; i++) {
			if (GetExitCodeProcess(handleProcessArray[i], exitcodeArray) == FALSE) {
				printf("Handle GetExitCodeProcess %d failure\n", procinfo->dwProcessId);
			}
		}
		Sleep(10);
		//printListOfProcess(handleProcessArray, exitcodeArray);
		if (exitcodeArray[i] == STILL_ACTIVE) {
			finishedProcessArray[i] = procinfo[i]->dwProcessId;
			numOfRunningProcess--;
		}
		else {
			runningProcessArray[i] = procinfo[i]->dwProcessId;
		}
	}
	printProcessArray(runningProcessArray, );
	printProcessArray(finishedProcessArray, );
}

void *getDWORDMallocArray(DWORD *nameArray, int MUL) {
	int i;
	nameArray = (DWORD*)malloc(MUL * sizeof(DWORD));
	if (nameArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < MUL; i++) {
		nameArray[i] = 0;	// initialize nameArray
	}
}


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

/*
//TEST
for (i = 0; i < TotalNumberOfFiles; i++) {
printf("%s\n", CommandLineArguentStringArray[i]);
}
getchar();
*/




