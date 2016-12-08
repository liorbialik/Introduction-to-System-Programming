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
#define RUN 2
#define FINISH 1


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
#include "TestManager.h"
#include <sys/types.h>
#include <direct.h>




/* Function Declarations: */
char *fileTestOutputLogPathCreation(char*, char*);
char *createRunTimeLogFileInsideOutputDirName(char *outpuDirName, char *runTime_logFileName);
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr);
LPTSTR ConvertCharStringToLPTSTR(const char *Source);
//void printListOfProcess(HANDLE *handleProcessArray, DWORD *exitcodeArray);
//void *getDynamicAllocationArrayOfDWORD(DWORD *nameArray, int MUL);
int CountNumOfTests(FILE *fileInput);
int *CountLengthOfEachTest(FILE *fileInput, int TotalNumberOfFiles);
char *FullCommandLineStringCreation(FILE *fileInput, int FilesToTestLength, char *outpuDirName);
void checkProcessStatus(DWORD waitcode, FILE *runTime_logFileOutput, PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, DWORD *exitcodeArray, int TotalNumberOfFiles, int AllProcessFinished);
void PrintSortedProcess(int *exitcodeArray, int TotalNumberOfFiles, PROCESS_INFORMATION *ProcessInfoPtr);
void bubble_sort(PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, int n);





int main(int argc, char *argv[]) {
	/* Internal Declarations: */
	FILE *fileInput = NULL; FILE *runTime_logFileOutput = NULL; int i; int AllProcessFinished = 0;
	char *fileName = NULL; char *outputFileName = NULL;
	char *outpuDirName = NULL; LPTSTR dirNameLPTSTR = NULL; char *loopTime = NULL;
	char *runTime_logFileName = NULL;
	char *outputLogFile = NULL; char *FileToTestName = NULL;
	int TotalNumberOfFiles = 0; int *FilesToTestLengthArray = NULL; char *CommandLineArguentStringArray[] = { NULL };
	fileName = argv[1];
	outpuDirName = argv[2];
	loopTime = argv[3];


	PROCESS_INFORMATION *ProcessInfoPtr;


	DWORD				waitcode;
	DWORD				*exitcodeArray;
	BOOL				retVal;
	LPTSTR				command;
	DWORD				ProcessStatusCheckFrequency = (int)loopTime;
	HANDLE				*handleProcessArray;




	const FILETIME		*FileTime;
	LPSYSTEMTIME		*SystemTime = NULL;


	//Start of Program


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
	_mkdir(outpuDirName);


	runTime_logFileOutput = fopen(createRunTimeLogFileInsideOutputDirName(outpuDirName, NULL), "w");
	if (runTime_logFileOutput == NULL) {
		printf("failed to open runTime_logFile, error %ul\n", GetLastError()); //TODO: need to add an error handling function
		exit(1);
	}


	// go over 'FilesToTest' file and count all files to be tested
	TotalNumberOfFiles = CountNumOfTests(fileInput);


	// Assign FilesToTestLengthArray in size of TotalNumberOfFiles
	FilesToTestLengthArray = CountLengthOfEachTest(fileInput, TotalNumberOfFiles);


	// Assign *CommandLineArguentStringArray[] in size of TotalNumberOfFiles, an array of pointers holding the commandLineArgumentString of eact test
	// concatenated all parameters to a single string
	for (i = 0; i<TotalNumberOfFiles; i++) {
		int FilesToTestLength = FilesToTestLengthArray[i];
		CommandLineArguentStringArray[i] = FullCommandLineStringCreation(fileInput, FilesToTestLength, outpuDirName);
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
	ProcessInfoPtr = (PROCESS_INFORMATION*)malloc(TotalNumberOfFiles * sizeof(PROCESS_INFORMATION));
	if (ProcessInfoPtr == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	exitcodeArray = (DWORD *)malloc(TotalNumberOfFiles * sizeof(DWORD));
	if (exitcodeArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		exitcodeArray[i] = 0;	// initialize nameArray
	}


	for (i = 0; i < TotalNumberOfFiles; i++, ProcessInfoPtr++) {
		command = ConvertCharStringToLPTSTR(CommandLineArguentStringArray[i]);		// Convert char* to TCHAR 
		retVal = CreateProcessSimple(command, ProcessInfoPtr);
		if (retVal == 0) {
			printf("!!! Failed to create new process to run %s. Error code: %d !!!\n", CommandLineArguentStringArray[i], GetLastError());
			fprintf(runTime_logFileOutput, "!!! Failed to create new process to run %s. Error code: %d !!!\n", CommandLineArguentStringArray[i], GetLastError());
			// TODO: Need to output hex rep. error code
		}
		else {
			fprintf(runTime_logFileOutput, "Successfully created a process with ID %d to execute %s\n", (ProcessInfoPtr)->dwProcessId, CommandLineArguentStringArray[i]);
			// save handleProcess that were succedded in an handleProcessArray
			handleProcessArray[i] = ProcessInfoPtr->hProcess;
		}
	}


	//for (i = 0; i < TotalNumberOfFiles; i++) {
	//	FileTime[i] = NULL;	// initialize handleProcessArray
	//}
	//FileTimeToSystemTime(FileTime, SystemTime);



	//for (i = 0; i<TotalNumberOfFiles; i++) {
	//	if (handleProcessArray[i] != 0) {
	//		numOfRunningProcess++;
	//	}
	//}
	//// TODO: call organizeArray for handleProcessArray



	do {
		waitcode = WaitForMultipleObjects(TotalNumberOfFiles, handleProcessArray, 1, ProcessStatusCheckFrequency);
		// analyze results into runTime_logFile
		checkProcessStatus(waitcode, runTime_logFileOutput, ProcessInfoPtr, handleProcessArray, exitcodeArray, TotalNumberOfFiles, AllProcessFinished);
		//TODO: Need to define sleep_time as LONGSLEEPTIME, SHORTSLEEPTIME.

	} while (!waitcode);		// return value of waitcode == TIME_0 means all process have finished, thus define as end condition




								//All the processes have finished running. Exiting program
	fprintf(runTime_logFileOutput, "All the processes have finished running. Exiting program\n");




	//CloseHandle(procinfo->hProcess); /* Closing the handle to the process */
	//CloseHandle(procinfo->hThread); /* Closing the handle to the main thread of the process */
	fclose(fileInput);
	fclose(runTime_logFileOutput);




	// free all allocated memories
	free(handleProcessArray);
	free(FileTime);
	free(ProcessInfoPtr);
	free(FilesToTestLengthArray);
	free(runTime_logFileName);
	free(FilesToTestLengthArray);
	free(CommandLineArguentStringArray);
	//free(Dest);
	//free(fileToTest);
	//free(outputLogFileName);

	for (i = 0; i < TotalNumberOfFiles; i++) {
		free(CommandLineArguentStringArray[i]);
	}
	return 0;
}




char *createRunTimeLogFileInsideOutputDirName(char *outpuDirName, char *runTime_logFileName) {
	// create runTime_log File inside <OutputFilesDirectory> directory
	char *runTime_logFileNameEnding = "\\runtime_logfile.txt"; char *runTime_logFileOutput = NULL;
	runTime_logFileName = (char *)malloc(1 + strlen(outpuDirName) + strlen(runTime_logFileNameEnding));
	if (runTime_logFileName == NULL) {
		printf("runTime_logFileName allocation failed/n");
		exit(1);
	}
	strcpy(runTime_logFileName, outpuDirName);
	strcat(runTime_logFileName, runTime_logFileNameEnding);




	return runTime_logFileName;
}


char *fileTestOutputLogPathCreation(char* outpuDirName, char* fileToTestName) {
	// prepare arguments for calling 'TestFiles' program
	char *outputLogFileNameEnding = { "_log.txt" }; char *outputLogFileName = NULL; int outputLogFileNameLength = 0;


	outputLogFileNameLength = strlen(outpuDirName) + strlen(fileToTestName) + strlen(outputLogFileNameEnding) - 4 + 2 + 1;
	// '-4' due to deduction of '.txt' ending since it appearse twice, '-2' due to addition of '\\', '+1' due to '\0' ending
	outputLogFileName = (char*)malloc(sizeof(char) * outputLogFileNameLength);
	if (outputLogFileName == NULL) {
		printf("outputLogFileName allocation failed/n");
		exit(1);
	}
	strcpy(outputLogFileName, outpuDirName);
	strcat(outputLogFileName, "\\");
	strcat(outputLogFileName, fileToTestName);
	outputLogFileName[strlen(outputLogFileName) - 4] = '\0'; // mark the end of the precious string before the '.txt' ending
	strcat(outputLogFileName, outputLogFileNameEnding); // add the correct ending




	return outputLogFileName;
}


int CountNumOfTests(FILE *fileInput) {
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


	return TotalNumberOfFiles;
}


int *CountLengthOfEachTest(FILE *fileInput, int TotalNumberOfFiles) {
	int *FilesToTestLengthArray = NULL; int i;
	FilesToTestLengthArray = (int *)malloc(TotalNumberOfFiles * sizeof(int));
	if (FilesToTestLengthArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		FilesToTestLengthArray[i] = 0;			// initialize FilesToTestArray
	}
	// go over FilesToTestArray and save each test's length
	for (i = 0; i<TotalNumberOfFiles; i++) {
		while (fgetc(fileInput) != '\n') {
			FilesToTestLengthArray[i]++;
		}
	}
	// Resetting pointer to the start of file
	rewind(fileInput);


	return FilesToTestLengthArray;
}


char *FullCommandLineStringCreation(FILE *fileInput, int FilesToTestLength, char *outpuDirName) {


	char *CommandLineArguentStringArray = NULL; char *TestFileProgramName = { "TestFile.exe " };
	char *fileToTest = NULL; int i; char *fileTestOutputLogPath = NULL;
	fileToTest = (char *)malloc(FilesToTestLength * sizeof(char));
	if (fileToTest == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	if (fgets(fileToTest, 1 + FilesToTestLength, fileInput) == NULL) {
		printf("reading a string from fileInput was failed, error %ul\n", GetLastError());
	}


	fileTestOutputLogPath = fileTestOutputLogPathCreation(outpuDirName, fileToTest);
	CommandLineArguentStringArray = (char *)malloc((FilesToTestLength + 14 + strlen(fileTestOutputLogPath)) * sizeof(char));
	// 14 for the const "FileTest.exe" and for two spaces
	if (CommandLineArguentStringArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	// Building the command line string
	strcpy(CommandLineArguentStringArray, TestFileProgramName);
	strcat(CommandLineArguentStringArray, fileToTest);
	strcat(CommandLineArguentStringArray, " ");
	strcat(CommandLineArguentStringArray, fileTestOutputLogPath);


	fgetc(fileInput);				// This command should lower the fileToTest pointer to read the next line 

	return CommandLineArguentStringArray;
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


void checkProcessStatus(DWORD waitcode, FILE *runTime_logFileOutput, PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, DWORD *exitcodeArray, int TotalNumberOfFiles, int AllProcessFinished) {
	//	//Check status of processes and print to runTime_logFile
	//	//printf("WaitForSingleObject output: ");
	//	int i; DWORD exitCode = 0; DWORD *finishedProcessArray; DWORD *runningProcessArray;
	//	getDWORDMallocArray(finishedProcessArray, numOfRunningProcess);
	//	getDWORDMallocArray(runningProcessArray, numOfRunningProcess);
	//

	//		printf("WAIT_TIMEOUT\n"); break;
	//	case WAIT_FAILED:
	//		printf("WaitForMultipleObjects function has failed, error %ul\n", GetLastError()); break;
	//	default:
	//		printf("0x%x\n", waitcode);
	//	}
	//
	//
	int i; int NeedToPrintMessageFlag = 0;
	DWORD *RunningProcessArray = NULL; DWORD *FinishedProcessArray = NULL;
	RunningProcessArray = (DWORD *)malloc(TotalNumberOfFiles * sizeof(DWORD));
	if (RunningProcessArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		RunningProcessArray[i] = 0;					// initialize nameArray
	}
	FinishedProcessArray = (DWORD *)malloc(TotalNumberOfFiles * sizeof(DWORD));
	if (FinishedProcessArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		FinishedProcessArray[i] = 0;				// initialize nameArray
	}
	switch (waitcode)
	{
	case WAIT_OBJECT_0: //all process were signaled, need to print them as finished process


	case WAIT_TIMEOUT: // Processes are still running, go over handleProcessArray, sort the ID field, get cell's ExitCodeProcess and print status		
		bubble_sort(ProcessInfoPtr, handleProcessArray, TotalNumberOfFiles);
		for (i = 0; i < TotalNumberOfFiles; i++) {
			if (GetExitCodeProcess(handleProcessArray[i], exitcodeArray[i]) == FALSE) {
				printf("Handle GetExitCodeProcess %d failure\n", ProcessInfoPtr->dwProcessId);
			}
			Sleep(10);
			if (exitcodeArray[i] != STILL_ACTIVE) {
				NeedToPrintMessageFlag = 1;
			}
		}

		if (NeedToPrintMessageFlag = 1) {
			while (exitcodeArray[i] == STILL_ACTIVE) {
				fprintf(runTime_logFileOutput, "Process %d running command %s for %d second %d millisecond\n", (ProcessInfoPtr)->dwProcessId, CommandLineArguentStringArray[i], /*times*/);
			}
		}

		PrintSortedRunningProcess(RunningProcessArray, TotalNumberOfFiles, ProcessInfoPtr);
		//Sort both running and finished processID and print appropriate message


		//		//printListOfProcess(handleProcessArray, exitcodeArray);
		//		if (exitcodeArray[i] == STILL_ACTIVE) {
		//			finishedProcessArray[i] = procinfo[i]->dwProcessId;
		//			numOfRunningProcess--;
		//		}
		//		else {
		//			runningProcessArray[i] = procinfo[i]->dwProcessId;
		//		}
		//	}
		//	printProcessArray(runningProcessArray, );
		//	printProcessArray(finishedProcessArray, );
	}

}

void bubble_sort(PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, int n) {
	int c, d, t, s;

	for (c = 0; c < (n - 1); c++) {
		for (d = 0; d < n - c - 1; d++) {
			if (ProcessInfoPtr[d].dwProcessId > ProcessInfoPtr[d + 1].dwProcessId) {
				/* Swapping */

				t = ProcessInfoPtr[d].dwProcessId;
				s = handleProcessArray[d];
				ProcessInfoPtr[d].dwProcessId = ProcessInfoPtr[d + 1].dwProcessId;
				handleProcessArray[d] = handleProcessArray[d + 1];
				ProcessInfoPtr[d + 1].dwProcessId = t;
				handleProcessArray[d + 1] = s;
			}
		}
	}
}

void PrintSortedProcess(int *exitcodeArray, int TotalNumberOfFiles, PROCESS_INFORMATION *ProcessInfoPtr) {
	int i;
	for (i = 0; i < TotalNumberOfFiles; i++) {
		if (exitcodeArray[i] != 0) {

		}
	}
}
//void *getDynamicAllocationArrayOfDWORD(DWORD *nameArray, int MUL) {
//	int i;
//	nameArray = (DWORD*)malloc(MUL * sizeof(DWORD));
//	if (nameArray == NULL) {
//		printf("allocation was failed, error %ul\n", GetLastError());
//	}
//	for (i = 0; i < MUL; i++) {
//		nameArray[i] = 0;	// initialize nameArray
//	}
//
//	return nameArray;
//}


// TestManager algorithm flow:
// 1. gets a 'fileToTest' file name and opens it.
// 2. for every fileTest in the content file:
//		a. going over the line, reads it and allocates a pointer for the specific line (Dynamic allocation). The program will hold array of pointers where each cell points to a different command line string.
//		b. call for TestFile.exe for every line (process). The program should send two arguments: <FileTestName>.txt, <OutputFilesDirectory>\\<FileTestName>_log.txt.
// 4. Create a 'runTime_logFile.txt inside 'Output Files Directory'.
// 5. The TestManager samples every x milisecond the processes, and updates their progress.
// 6. Allocate array of handlers.
// 7. going over all handles array in a constant loop. Each segment time print the process status as follows:
//		a. going over handles array and sort it according to ProcInfoPtr's ID in an ascending order.
//		b. arrange exit code array in the same order. The exit code will hold the exit code definition of each process.
//		c. go over the process. if exitcode == STILL ALIVE => print process alive according to context.
//								if exitcode != STILL ALIVE => print process finished according to context. 
//		


/*
//TEST
for (i = 0; i < TotalNumberOfFiles; i++) {
printf("%s\n", CommandLineArguentStringArray[i]);
}
getchar();
*/












