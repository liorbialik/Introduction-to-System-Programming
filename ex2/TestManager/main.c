/*
Main.c: TestManager.exe
- This program manages all runnings of TestFiles.
- Each file will be sent by TestManager to a set of tests, as a different process.
- The TestManager also outputs a status log file by sampling the processes every time set, noted by 'processStatusCheckFrequency'
- The testManager finishes its work only when all processes are finished running.
Tomer Shahar 301359410, Lior Bialik 301535316
*/

#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#define TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55

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

char *outputLogFileArgumentCreation(char*, char*);
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr);
char *openRunTimeLogFile(char*, char*);
LPTSTR ConvertCharStringToLPTSTR(const char *Source);
TCHAR *charArray_To_TcharArray(char *source, char *dest);


int main(int argc, char *argv[]) {
	FILE *fileInput = NULL; FILE *runTime_logFileOutput = NULL; 
	char *fileName = NULL; char *outputFileName = NULL;
	const char *dirName = NULL; LPTSTR dirNameLPTSTR = NULL;
	char *runTime_logFileName = NULL;
	char *outputLogFile = NULL; char *FileToTestName = NULL;
	fileName = argv[1];
	dirName = argv[2];

	
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
	// TODO: Need to create a case whether to open a dir if not exist
	mkdir(dirName);

	if (openRunTimeLogFile(dirName, NULL) == NULL) {
		printf("failed to open runTime_logFile/n");
		exit(1);
	}

	// open FilesToTest and for each file create a process

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
	char *CommandLineArguentStringArray[] = { NULL }; char *TestFileProgramName = { "FileTest.exe " }; char *TestFileArgumentString = NULL; char *fileToTest = NULL;
	
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
		CommandLineArguentStringArray[i] = (char *)malloc(((FilesToTestLengthArray[i]) +  14 + strlen(outputLogFile)) * sizeof(char));
		// 14 for the const "FileTest.exe" and for two spaces
		if (CommandLineArguentStringArray[i] == NULL) {
			printf("allocation was failed, error %ul\n", GetLastError());
		}
		strcpy(CommandLineArguentStringArray[i], TestFileProgramName);
		strcat(CommandLineArguentStringArray[i], fileToTest);
		strcat(CommandLineArguentStringArray[i], " ");
		strcat(CommandLineArguentStringArray[i], outputLogFile);

	}

	//// Demonstrates win32 process creation and termination of a process
	PROCESS_INFORMATION *procinfo;
	DWORD				waitcode;
	DWORD				exitcode;
	BOOL				retVal;
	LPTSTR				command;

	procinfo = (PROCESS_INFORMATION*)malloc(sizeof(PROCESS_INFORMATION));
	char *ex = { "calc.exe" };
	command = ConvertCharStringToLPTSTR(CommandLineArguentStringArray[0]);
	retVal = CreateProcessSimple(command, procinfo);
	if (retVal == 0) {
		printf("Process Creation Failed!\n");
		return;
	}
	getchar();

	//waitcode = WaitForSingleObject(
	//	procinfo.hProcess,
	//	TIMEOUT_IN_MILLISECONDS); /* Waiting 5 secs for the process to end */

	//printf("WaitForSingleObject output: ");
	//switch (waitcode)
	//{
	//case WAIT_TIMEOUT:
	//	printf("WAIT_TIMEOUT\n"); break;
	//case WAIT_OBJECT_0:
	//	printf("WAIT_OBJECT_0\n"); break;
	//default:
	//	printf("0x%x\n", waitcode);
	//}

	//if (waitcode == WAIT_TIMEOUT) /* Process is still alive */
	//{
	//	printf("Process was not terminated before timeout!\n"
	//		"Terminating brutally!\n");
	//	TerminateProcess(
	//		procinfo.hProcess,
	//		BRUTAL_TERMINATION_CODE); /* Terminating process with an exit code of 55h */
	//	Sleep(10); /* Waiting a few milliseconds for the process to terminate */
	//}

	//GetExitCodeProcess(procinfo.hProcess, &exitcode);

	//printf("The exit code for the process is 0x%x\n", exitcode);

	//CloseHandle(procinfo.hProcess); /* Closing the handle to the process */
	//CloseHandle(procinfo.hThread); /* Closing the handle to the main thread of the process */




/*
	//TEST
	for (i = 0; i < TotalNumberOfFiles; i++) {
		printf("%s\n", CommandLineArguentStringArray[i]);
	}
	getchar();
*/

	//closing files that have been opened during the program
	fclose(fileInput);

	// free all allocated memories
	free(FilesToTestLengthArray);
	free(runTime_logFileName);
	for (i = 0; i < TotalNumberOfFiles; i++) {
		free(CommandLineArguentStringArray[i]);
	}
	return 0;
}

char *openRunTimeLogFile(char *dirName, char *runTime_logFileName) {

	// create runTime_log File inside <OutputFilesDirectory> directory
	char *runTime_logFileNameEnding = "\\runtime_logfile.txt"; char *runTime_logFileOutput = NULL;
	runTime_logFileName = (char *)malloc(1 + strlen(dirName) + strlen(runTime_logFileNameEnding));
	if (runTime_logFileName == NULL) {
		printf("runTime_logFileName allocation failed/n");
		exit(1);
	}
	strcpy(runTime_logFileName, dirName);
	strcat(runTime_logFileName, runTime_logFileNameEnding);
	runTime_logFileOutput = fopen(runTime_logFileName, "w");
	if (runTime_logFileOutput == NULL) {
		printf("Could not open file, error %ul\n", GetLastError()); //TODO: need to add an error handling function
		exit(1);
	}
	fclose(runTime_logFileOutput);
	return runTime_logFileOutput;
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

//TCHAR *charArray_To_TcharArray(char *source, char *dest) {
//	
//	int i;
//	dest = (TCHAR*)malloc((strlen(source) + 10) * sizeof(TCHAR));
//	if (dest == NULL) {
//		printf("runTime_logFileName allocation failed/n");
//		exit(1);
//	}
//	for (i = 0; i < strlen(source); i++) {
//		dest[i] = (TCHAR)source[i];
//	}
//	dest[i] = 0;
//
//	return dest;
//}

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

//going over the fileInput in a loop:
// 1. read the lines as a single string
// 2. build arguments for the file string
// 3. concatenated all parameters to a single string as well, "TestFiles.exe <hw.txt> <OutputFilesDirectory>\<hw.txt>"
// 4. create process for that string
// 5. call TestFiles.exe 


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
