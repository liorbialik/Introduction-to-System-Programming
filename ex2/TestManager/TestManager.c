/*
Main.c: TestManager.exe
- This program manages all runnings of TestFiles.
- Each file will be sent by TestManager to a set of tests, as a different process.
- The TestManager also outputs a status log file by sampling the processes every time set, noted by 'processStatusCheckFrequency'
- The testManager finishes its work only when all processes are finished running.
Tomer Shahar 301359410, Lior Bialik 301535316
*/


/* Constants: */
#define LONG_SLEEP_TIME 5000
#define SHORT_SLEEP_TIME 5
#define SECOND_DIVIDER 10000000000
#define MILLI_SECOND_DIVIDER 1000000




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
int CountNumOfTests(FILE *fileInput);
int *CountLengthOfEachTest(FILE *fileInput, int TotalNumberOfFiles);
char *FullCommandLineStringCreation(FILE *fileInput, int FilesToTestLength, char *outpuDirName);
void checkProcessStatus(DWORD waitcode, FILE *runTime_logFileOutput, char **CommandLineArguentStringArray, PROCESS_INFORMATION *ProcessInfoPtr,
	HANDLE *handleProcessArray, DWORD *exitcodeArray, int TotalNumberOfFiles);
FILETIME SubtractTimesOfProcess(FILETIME exit, FILETIME creation);
void bubble_sort(PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, int n);

struct PROCESS_TIME_INF {
	FILETIME CreationTime;
	FILETIME ExitTime;
	FILETIME KernelTime;
	FILETIME UserTime;
};
struct FILE_TIME {
	DWORD second;
	DWORD millisecond;
};


int main(int argc, char *argv[]) {
	/* Internal Declarations: */
	FILE *fileInput = NULL; FILE *runTime_logFileOutput = NULL;
	LPTSTR dirNameLPTSTR = NULL;
	char *outputLogFile = NULL, *FileToTestName = NULL, **CommandLineArguentStringArray = { NULL }, *runTime_logFileName = NULL,
		*outpuDirName = NULL, *fileName = NULL, *outputFileName = NULL, *loopTime = NULL;
	int TotalNumberOfFiles = 0, *FilesToTestLengthArray = NULL, i, AllProcessFinished = 0, errorCodeFlag = 0, ProcessesAreStillRunningFlag = 0;

	PROCESS_INFORMATION *ProcessInfoPtr = NULL;
	DWORD				waitcode = 0;
	DWORD				*exitcodeArray = NULL;
	BOOL				retVal = 0;
	LPTSTR				command = NULL;
	DWORD				ProcessStatusCheckFrequency = 0;
	HANDLE				*handleProcessArray = NULL;


	//Start of Program
	fileName = argv[1];
	outpuDirName = argv[2];
	loopTime = argv[3];
	ProcessStatusCheckFrequency = atoi(loopTime);

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
	CommandLineArguentStringArray = (char**)malloc(TotalNumberOfFiles * sizeof(char*));
	for (i = 0; i < TotalNumberOfFiles; i++) {
		CommandLineArguentStringArray[i] = FullCommandLineStringCreation(fileInput, FilesToTestLengthArray[i], outpuDirName);
	}


	//Dynamic memory allocations
	handleProcessArray = (HANDLE*)malloc(TotalNumberOfFiles * sizeof(HANDLE));
	if (handleProcessArray == NULL) {
		printf("allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		handleProcessArray[i] = 0;	// initialize handleProcessArray
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
		exitcodeArray[i] = 0;	// initialize exitcodeArray
	}


	// Creating a processes
	for (i = 0; i < TotalNumberOfFiles; i++) {
		command = ConvertCharStringToLPTSTR(CommandLineArguentStringArray[i]);		// Convert char* to TCHAR 
		retVal = CreateProcessSimple(command, &ProcessInfoPtr[i]);
		if (retVal == 0) {
			printf("!!! Failed to create new process to run %s. Error code: %ul !!!\n", CommandLineArguentStringArray[i], GetLastError());
			fprintf(runTime_logFileOutput, "!!! Failed to create new process to run %s. Error code: %ul !!!\n", CommandLineArguentStringArray[i], GetLastError());
			// TODO: Need to output hex rep. error code
		}
		else {
			fprintf(runTime_logFileOutput, "Successfully created a process with ID %d to execute %s\n", ProcessInfoPtr[i].dwProcessId, CommandLineArguentStringArray[i]);
			// save handleProcess that were succedded in an handleProcessArray
			handleProcessArray[i] = ProcessInfoPtr[i].hProcess;
		}
	}

	// analyze results into runTime_logFile
	do {
		ProcessesAreStillRunningFlag = 0;
		waitcode = WaitForMultipleObjects(TotalNumberOfFiles, handleProcessArray, TRUE, ProcessStatusCheckFrequency);
		if (waitcode == 0xFFFFFFFF) {
			printf("!!! WaitForMultipleObjects function failed, Error code: %ul !!!\n", GetLastError()); break;
		}
		if (waitcode == WAIT_OBJECT_0) {
			checkProcessStatus(waitcode, runTime_logFileOutput, CommandLineArguentStringArray, ProcessInfoPtr, handleProcessArray, exitcodeArray, TotalNumberOfFiles); break;
		}
		if (waitcode == WAIT_TIMEOUT) {
			ProcessesAreStillRunningFlag = 1;
			checkProcessStatus(waitcode, runTime_logFileOutput, CommandLineArguentStringArray, ProcessInfoPtr, handleProcessArray, exitcodeArray, TotalNumberOfFiles);
		}
	} while (ProcessesAreStillRunningFlag);

	//All the processes have finished running. Exiting program
	fprintf(runTime_logFileOutput, "All the processes have finished running. Exiting program\n");


	// Close files
	fclose(fileInput);
	fclose(runTime_logFileOutput);

	// free all allocated memories
	free(handleProcessArray);				 /* Closing the handle to the process */
	free(CommandLineArguentStringArray);
	free(ProcessInfoPtr);
	free(FilesToTestLengthArray);
	free(runTime_logFileName);
	free(exitcodeArray);

	return 0;
}


/* Function Definitions */
char *createRunTimeLogFileInsideOutputDirName(char *outpuDirName, char *runTime_logFileName) {
	// create runTime_log File inside <OutputFilesDirectory> directory
	char *runTime_logFileNameEnding = "\\runtime_logfile.txt"; char *runTime_logFileOutput = NULL;
	runTime_logFileName = (char *)malloc(1 + strlen(outpuDirName) + strlen(runTime_logFileNameEnding));
	if (runTime_logFileName == NULL) {
		printf("runTime_logFileName allocation failed, error %ul\n", GetLastError());
		exit(1);
	}
	strcpy(runTime_logFileName, outpuDirName);
	strcat(runTime_logFileName, runTime_logFileNameEnding);








	return runTime_logFileName;
}


char *fileTestOutputLogPathCreation(char* outpuDirName, char* fileToTestName) {
	// prepare arguments for calling 'TestFiles' program
	char *outputLogFileNameEnding = { "_log.txt" }; char *outputLogFileName = NULL; size_t outputLogFileNameLength = 0;

	outputLogFileNameLength = strlen(outpuDirName) + strlen(fileToTestName) + strlen(outputLogFileNameEnding) - 4 + 2 + 1;
	// '-4' due to deduction of '.txt' ending since it appearse twice, '-2' due to addition of '\\', '+1' due to '\0' ending
	outputLogFileName = (char*)malloc(sizeof(char) * outputLogFileNameLength);
	if (outputLogFileName == NULL) {
		printf("outputLogFileName allocation failed, error %ul\n", GetLastError());
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
	int ch;
	int TotalNumberOfFiles = 0;
	while (!feof(fileInput)) {
		ch = fgetc(fileInput);
		if ((ch == '\n') || (ch == EOF)) {
			TotalNumberOfFiles++;
		}
	}
	// Resetting pointer to the start of file
	rewind(fileInput);

	return TotalNumberOfFiles;
}


int *CountLengthOfEachTest(FILE *fileInput, int TotalNumberOfFiles) {
	int *FilesToTestLengthArray = NULL; int i; char ch = NULL;
	FilesToTestLengthArray = (int *)malloc(TotalNumberOfFiles * sizeof(int));
	if (FilesToTestLengthArray == NULL) {
		printf("FilesToTestLengthArray allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfFiles; i++) {
		FilesToTestLengthArray[i] = 0;			// initialize FilesToTestArray
	}
	// go over FilesToTestArray and save each test's length
	for (i = 0; i<TotalNumberOfFiles - 1; i++) {
		while (fgetc(fileInput) != '\n') {
			FilesToTestLengthArray[i]++;
		}
	}
	if (i == TotalNumberOfFiles - 1) {
		while (fgetc(fileInput) != EOF) {
			FilesToTestLengthArray[i]++;
		}
	}
	// Resetting pointer to the start of file
	rewind(fileInput);

	return FilesToTestLengthArray;
}


char *FullCommandLineStringCreation(FILE *fileInput, int FilesToTestLength, char *outpuDirName) {
	char *CommandLineArguentStringArray = NULL; char *TestFileProgramName = { "TestFile.exe " };
	char *fileToTest = NULL; char *fileTestOutputLogPath = NULL;

	fileToTest = (char *)malloc(FilesToTestLength * sizeof(char));
	if (fileToTest == NULL) {
		printf("fileToTest allocation was failed, error %ul\n", GetLastError());
	}
	if (fgets(fileToTest, 1 + FilesToTestLength, fileInput) == NULL) {
		printf("reading a string from fileInput was failed, error %ul\n", GetLastError());
	}

	fileTestOutputLogPath = fileTestOutputLogPathCreation(outpuDirName, fileToTest);
	CommandLineArguentStringArray = (char *)malloc((FilesToTestLength + 15 + strlen(fileTestOutputLogPath)) * sizeof(char));
	// 15 for the const "FileTest.exe" + two spaces + '\0'
	if (CommandLineArguentStringArray == NULL) {
		printf("CommandLineArguentStringArray allocation was failed, error %ul\n", GetLastError());
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


void checkProcessStatus(DWORD waitcode, FILE *runTime_logFileOutput, char **CommandLineArguentStringArray, PROCESS_INFORMATION *ProcessInfoPtr,
	HANDLE *handleProcessArray, DWORD *exitcodeArray, int TotalNumberOfFiles) {
	//Check status of processes and print to runTime_logFile

	/* Function Declarations: */
	int i, SomeProcesssHasFinishedFlag = 0;
	PROCESS_TIME_INF *ProcessTimeInf = NULL;
	FILE_TIME *RunningProcessTimeResult_int;
	FILE_TIME *FinishedProcessTimeResult_int;
	FILETIME *RunningProcessTimeResult, *FinishedProcessTimeResult, SystemTime_FILETIME;
	ProcessTimeInf = (PROCESS_TIME_INF*)malloc(TotalNumberOfFiles * sizeof(PROCESS_TIME_INF));
	RunningProcessTimeResult = (FILETIME*)malloc(TotalNumberOfFiles * sizeof(FILETIME));
	FinishedProcessTimeResult = (FILETIME*)malloc(TotalNumberOfFiles * sizeof(FILETIME));
	RunningProcessTimeResult_int = (FILE_TIME*)malloc(TotalNumberOfFiles * sizeof(FILE_TIME));
	FinishedProcessTimeResult_int = (FILE_TIME*)malloc(TotalNumberOfFiles * sizeof(FILE_TIME));

	bubble_sort(ProcessInfoPtr, handleProcessArray, TotalNumberOfFiles);		// sort handle array and ProcInfo->dwProcessId array in order to print in ascending order

	switch (waitcode) {

	case WAIT_OBJECT_0: 		// all process were signaled, need to print them as finished process														
								// Print finished process	
		printf("List of finished processes:\n");
		fprintf(runTime_logFileOutput, "List of finished processes:\n");
		for (i = 0; i < TotalNumberOfFiles; i++) {
			if (GetExitCodeProcess(handleProcessArray[i], &exitcodeArray[i]) == FALSE) {
				printf("Handle %d GetExitCodeProcess failure, error %ul\n", ProcessInfoPtr[i].dwProcessId, GetLastError());
			}
			if (exitcodeArray[i] != STILL_ACTIVE) {
				GetProcessTimes(handleProcessArray[i], &ProcessTimeInf[i].CreationTime,
					&ProcessTimeInf[i].ExitTime, &ProcessTimeInf[i].KernelTime, &ProcessTimeInf[i].UserTime);
				GetSystemTimeAsFileTime(&SystemTime_FILETIME);
				FinishedProcessTimeResult[i] = SubtractTimesOfProcess(ProcessTimeInf[i].ExitTime, ProcessTimeInf[i].CreationTime);
				FinishedProcessTimeResult_int[i].millisecond = (FinishedProcessTimeResult[i].dwLowDateTime / MILLI_SECOND_DIVIDER);
				FinishedProcessTimeResult_int[i].second = (FinishedProcessTimeResult[i].dwLowDateTime / SECOND_DIVIDER);

				printf("Process %d ran command %s and exited with exit code 0x%x after %d seconds and %d millisecond\n",
					ProcessInfoPtr[i].dwProcessId, CommandLineArguentStringArray[i], exitcodeArray[i], FinishedProcessTimeResult_int[i].second, FinishedProcessTimeResult_int[i].millisecond);
				fprintf(runTime_logFileOutput, "Process %d ran command %s and exited with exit code 0x%x after %d seconds and %d millisecond\n",
					ProcessInfoPtr[i].dwProcessId, CommandLineArguentStringArray[i], exitcodeArray[i], FinishedProcessTimeResult_int[i].second, FinishedProcessTimeResult_int[i].millisecond);
			}
		}

		break;

	case WAIT_TIMEOUT: 	// Not all process were signaled, need to print running & finished process															
						// Print Running process
		printf("List of running processes:\n");
		fprintf(runTime_logFileOutput, "List of running processes:\n");
		for (i = 0; i < TotalNumberOfFiles; i++) {
			if (GetExitCodeProcess(handleProcessArray[i], &exitcodeArray[i]) == FALSE) {
				printf("Handle %d GetExitCodeProcess failure, error %ul\n", ProcessInfoPtr[i].dwProcessId, GetLastError());
			}
			if (exitcodeArray[i] == STILL_ACTIVE) {
				GetProcessTimes(handleProcessArray[i], &ProcessTimeInf[i].CreationTime,
					&ProcessTimeInf[i].ExitTime, &ProcessTimeInf[i].KernelTime, &ProcessTimeInf[i].UserTime);
				GetSystemTimeAsFileTime(&SystemTime_FILETIME);
				RunningProcessTimeResult[i] = SubtractTimesOfProcess(SystemTime_FILETIME, ProcessTimeInf[i].CreationTime);
				RunningProcessTimeResult_int[i].millisecond = (RunningProcessTimeResult[i].dwLowDateTime / MILLI_SECOND_DIVIDER);
				RunningProcessTimeResult_int[i].second = (RunningProcessTimeResult[i].dwLowDateTime / SECOND_DIVIDER);

				printf("Process %d running command %s for %d seconds and %d millisecond\n",
					ProcessInfoPtr[i].dwProcessId, CommandLineArguentStringArray[i], RunningProcessTimeResult_int[i].second, RunningProcessTimeResult_int[i].millisecond);
				fprintf(runTime_logFileOutput, "Process %d running command %s for %d seconds and %d millisecond\n",
					ProcessInfoPtr[i].dwProcessId, CommandLineArguentStringArray[i], RunningProcessTimeResult_int[i].second, RunningProcessTimeResult_int[i].millisecond);
			}
			if (exitcodeArray[i] != STILL_ACTIVE)
				SomeProcesssHasFinishedFlag = 1;
		}


		if (SomeProcesssHasFinishedFlag) {
			// Print finished process	
			printf("List of finished processes:\n");
			fprintf(runTime_logFileOutput, "List of finished processes:\n");
			for (i = 0; i < TotalNumberOfFiles; i++) {
				if (exitcodeArray[i] != STILL_ACTIVE) {
					GetProcessTimes(handleProcessArray[i], &ProcessTimeInf[i].CreationTime,
						&ProcessTimeInf[i].ExitTime, &ProcessTimeInf[i].KernelTime, &ProcessTimeInf[i].UserTime);
					FinishedProcessTimeResult[i] = SubtractTimesOfProcess(ProcessTimeInf[i].ExitTime, ProcessTimeInf[i].CreationTime);
					FinishedProcessTimeResult_int[i].millisecond = (FinishedProcessTimeResult[i].dwLowDateTime / MILLI_SECOND_DIVIDER) % SECOND_DIVIDER;
					FinishedProcessTimeResult_int[i].second = (FinishedProcessTimeResult[i].dwLowDateTime / MILLI_SECOND_DIVIDER) / SECOND_DIVIDER;

					printf("Process %d ran command %s and exited with exit code 0x%x after %d seconds and %d millisecond\n",
						ProcessInfoPtr[i].dwProcessId, CommandLineArguentStringArray[i], exitcodeArray[i], FinishedProcessTimeResult_int[i].second, FinishedProcessTimeResult_int[i].millisecond);
					fprintf(runTime_logFileOutput, "Process %d ran command %s and exited with exit code 0x%x after %d seconds and %d millisecond\n",
						ProcessInfoPtr[i].dwProcessId, CommandLineArguentStringArray[i], exitcodeArray[i], FinishedProcessTimeResult_int[i].second, FinishedProcessTimeResult_int[i].millisecond);
				}
			}
		}

		break;
	}

	free(RunningProcessTimeResult);
	free(FinishedProcessTimeResult);
	free(RunningProcessTimeResult_int);
	free(FinishedProcessTimeResult_int);
}


void bubble_sort(PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, int n) {
	int c, d, t;
	HANDLE s;


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


FILETIME SubtractTimesOfProcess(FILETIME exit, FILETIME creation) {
	typedef  unsigned __int64 Unsigned64BitType;
	Unsigned64BitType Late64BitVal;
	Unsigned64BitType Early64BitVal;
	Unsigned64BitType Difference64BitVal;
	FILETIME DifferenceAsFILETIME;
	const Unsigned64BitType Low32BitsMask = 0x00000000FFFFFFFF;


	Late64BitVal = ((Unsigned64BitType)(exit.dwHighDateTime) << 32) + exit.dwLowDateTime;
	Early64BitVal = ((Unsigned64BitType)(creation.dwHighDateTime) << 32) + creation.dwLowDateTime;


	Difference64BitVal = Late64BitVal - Early64BitVal;


	DifferenceAsFILETIME.dwLowDateTime = (DWORD)(Difference64BitVal & Low32BitsMask);
	DifferenceAsFILETIME.dwHighDateTime = (DWORD)(Difference64BitVal >> 32);


	return DifferenceAsFILETIME;
}
