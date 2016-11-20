#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>

/*oOoOoOoOoOoOoOoOoOoOoOoOoO Types oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

typedef struct {
	unsigned int number;
	char table;
} GuestSeating_t;

/*oOoOoOoOoOoOoOoOoOoOoOoOo Constants oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define DB_SIZE 1024
#define NUM_OF_THREADS 3
#define FILETIME_UNITS_PER_MILLISECOND 10000 

/*oOoOoOoOoOoOoOoOoOoOoOoOo Function Declarations oOoOoOoOoOoOoOoOoOoOoOoO*/

void LastErrorAndTimesExample();

BOOL CreateProcessSimple(LPTSTR CommandLine,
	PROCESS_INFORMATION *ProcessInfoPtr);

FILETIME SubtractFiletimes(FILETIME Late, FILETIME Early);

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);

void fillDB(GuestSeating_t GuestDB[], int ArraySize);

int searchDB(GuestSeating_t *ItemPtr);

void MainThreadExample();

/*oOoOoOoOoOoOoOoOoOoO Global Variables oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/* Global memory is used to store a shared resource between several threads */

GuestSeating_t GuestDB[DB_SIZE]; /* Database containing guests and their tables */

								 /*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void main()
{

	LastErrorAndTimesExample();
	printf("=================================================\n");
	MainThreadExample();
	printf("=================================================\n");
	ShareYourHeap();

	
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/**
* <ISP> Example of getting last error value, and working with process times.
*/
void LastErrorAndTimesExample()
{
	PROCESS_INFORMATION procinfo;
	DWORD               waitcode;
	BOOL                retVal;
	SYSTEMTIME          UTC_Time, LocalTime;
	FILETIME            CreationTime, ExitTime, KernelTime, UserTime, TotalTime;
	TCHAR               Command[] = _T("calc.exe");
	

	/*-----------------------------*/
	/* 1. Start the child process: */
	/*-----------------------------*/
	retVal = CreateProcessSimple(Command, &procinfo);

	if (retVal == 0)
	{
		printf("Couldn't create process, error code %d\n", GetLastError());
		return;
	}

	CloseHandle(procinfo.hThread); /* We don't need the handle to the thread */
								   /* so we can close it right now. The thread will continue to run. */

  /*--------------------------------*/
 /* 2. Wait for process to finish: */
/*--------------------------------*/
	waitcode = WaitForSingleObject(procinfo.hProcess, INFINITE);

	if (waitcode != WAIT_OBJECT_0)
	{
		printf("Unexpected output value of 0x%x from WaitForSingleObject(). "
			"Ending function.\n", waitcode);
		CloseHandle(procinfo.hProcess);
		return;
	}

	/*-----------------------------------------------*/
	/* 3. Read, convert and print the process times: */
	/*-----------------------------------------------*/

	/* a. Retrieve the process times: */
	GetProcessTimes(
		procinfo.hProcess,
		&CreationTime,
		&ExitTime,
		&KernelTime,
		&UserTime
		);

	/* b. Convert the creation time to local time: */

	FileTimeToSystemTime(&CreationTime, /* input */
		&UTC_Time);    /* output */

	SystemTimeToTzSpecificLocalTime(NULL, /* use default time zone */
		&UTC_Time,     /* input */
		&LocalTime);  /* output */

					  /* c. Print the date and time: */

/*
	printf("Prints the process's time fields- raw data- FILETIME structs:\n");
	printf("CreationTime\n.dwLowDateTime= %lld\n", CreationTime.dwLowDateTime);
	printf("ExitTime\n.dwLowDateTime= %lld\n", ExitTime.dwLowDateTime);
*/
	printf("Process started at: %2d/%02d/%d  %02d:%02d:%02d\n",
		LocalTime.wDay, LocalTime.wMonth, LocalTime.wYear,
		LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);

	TotalTime = SubtractFiletimes(ExitTime, CreationTime);

	FileTimeToSystemTime(&TotalTime, /* input */
		&UTC_Time);    /* output */

	printf("Total running time : %d minutes, %d seconds, %d milliseconds\n",
		UTC_Time.wMinute, UTC_Time.wSecond, UTC_Time.wMilliseconds);

	if ((KernelTime.dwHighDateTime == 0) &&
		(UserTime.dwHighDateTime == 0))
	{
		printf("Kernel time : %dms\n",
			KernelTime.dwLowDateTime / FILETIME_UNITS_PER_MILLISECOND);

		printf("User time : %dms\n",
			UserTime.dwLowDateTime / FILETIME_UNITS_PER_MILLISECOND);
	}
	else
	{
		printf("This function can only print the Kernel Time and User Time "
			"if they are small enough to be held in a single DWORD each. "
			"That is not true, so they will not be printed. ");
	}

	CloseHandle(procinfo.hProcess); /* Closing tbe handle to the process */
}

/**
* CreateProcessSimple uses the win32 API to create a process that runs the
* command in 'CommandLine'. it uses the win32 API function CreateProcess()
* using default values for most parameters.
*
* Accepts:
* --------
* CommandLine - a windows generic string containing the command that the new
*               process performs. ( See CreateProcess( documentation for more ).
* ProcessInfoPtr - an output parameter, used to return a PROCESS_INFORMATION
*					structure containing data about the process that was created.
*					( See CreateProcess() documentation for more ).
* Returns:
* --------
* the output of CreateProcess().
*/
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr)
{
	STARTUPINFO    startinfo = { sizeof(STARTUPINFO), NULL, 0 }; /* <ISP> here we */
																 /* initialize a "Neutral" STARTUPINFO variable. Supplying this to */
																 /* CreateProcess() means we have no special interest in this parameter. */
																 /* This is equivalent to what we are doing by supplying NULL to most other */
																 /* parameters of CreateProcess(). */

	return CreateProcess(NULL, /*  No module name (use Command line). */
		CommandLine,            /*  Command line. */
		NULL,                    /*  Process handle not inheritable. */
		NULL,                    /*  Thread handle not inheritable. */
		FALSE,                    /*  Set handle inheritance to FALSE. */
		NORMAL_PRIORITY_CLASS,    /*  creation/priority flags. */
		NULL,                    /*  Use parent's environment block. */
		NULL,                    /*  Use parent's starting directory. */
		&startinfo,                /*  Pointer to STARTUPINFO structure. */
		ProcessInfoPtr            /*  Pointer to PROCESS_INFORMATION structure. */
		);
}

/**
* calculates the difference ( a - b ) between two times in FILETIME format.
*
* parameters:
* ----------
* Late - the time that will be subtracted from ( a )
* Early - the time that will be subtracted ( b ) : should be earlier than 'Late'
*
* Returns:
* -------
* The difference in times: ( Late - Early )
*/
FILETIME SubtractFiletimes(FILETIME Late, FILETIME Early)
{
	typedef unsigned __int64 Unsigned64BitType;
	Unsigned64BitType Late64BitVal;
	Unsigned64BitType Early64BitVal;
	Unsigned64BitType Difference64BitVal;
	FILETIME DifferenceAsFILETIME;
	const Unsigned64BitType Low32BitsMask = 0x00000000FFFFFFFF;

	Late64BitVal = ((Unsigned64BitType)(Late.dwHighDateTime) << 32) +
		Late.dwLowDateTime;
	Early64BitVal = ((Unsigned64BitType)(Early.dwHighDateTime) << 32) +
		Early.dwLowDateTime;
	Difference64BitVal = Late64BitVal - Early64BitVal;

	DifferenceAsFILETIME.dwLowDateTime =
		(DWORD)(Difference64BitVal & Low32BitsMask);
	DifferenceAsFILETIME.dwHighDateTime =
		(DWORD)(Difference64BitVal >> 32);

	return DifferenceAsFILETIME;
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/**
* <ISP> Example of using threads.
*/
void MainThreadExample()
{
	HANDLE ThreadHandles[NUM_OF_THREADS];   /* An array of thread handles */
	DWORD ThreadIDs[NUM_OF_THREADS];        /* An array of threadIDs */
	DWORD exitcode;
	GuestSeating_t guest0, guest1, guest2; /* Items to be compared with the GuestDB */
	int i;

	fillDB(GuestDB, DB_SIZE);

	guest0.number = 956;    guest0.table = 3;
	guest1.number = 648;    guest1.table = 3; /*  This guest is seated at table #2; */
	guest2.number = 282;    guest2.table = 1;

	ThreadHandles[0] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)searchDB,  /*  thread function */
		&guest0,                            /*  argument to thread function */
		&ThreadIDs[0]);                    /*  returns the thread identifier */

	ThreadHandles[1] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)searchDB,
		&guest1,
		&ThreadIDs[1]);

	ThreadHandles[2] = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)searchDB,
		&guest2,
		&ThreadIDs[2]);

	/* Wait for all threads to end: */
	WaitForMultipleObjects(
		NUM_OF_THREADS,
		ThreadHandles,
		TRUE,       /* wait until all threads finish */
		INFINITE);

	Sleep(10);

	for (i = 0; i < NUM_OF_THREADS; i++)
	{
		GetExitCodeThread(ThreadHandles[i], &exitcode);
		printf("Thread number %d returned exit code %d\n", i, exitcode);
		CloseHandle(ThreadHandles[i]);
	}
}

/**
* Fills in an array of GuestSeating Data.
*
* parameters:
* ----------
* GuestDB - an empty array of GuestSeating data, is filled by this function.
* ArraySize - the number of cells in the array.
*/
void fillDB(GuestSeating_t GuestDB[], int ArraySize)
{
	int i;
	for (i = 0; i < ArraySize; i++)
	{
		GuestDB[i].number = i;
		GuestDB[i].table = i / 256;
	}
}

/**
* Searches the global GuestDB and checks if the GuestSeating given as input
* exists in the DB.
*
* parameters:
* ----------
* ItemPtr - a pointer to a GuestSeating object, describing the
*           sitting of one guest.
* returns:
* -------
* 1 if the Item is in the GuestDB, 0 otherwise.
*/
int searchDB(GuestSeating_t *ItemPtr)
{
	int i;

	printf("Created thread that checks if guest number %d sits at table"
		" number %d!\n", ItemPtr->number, ItemPtr->table);
	/* <ISP> printf is thread-safe */

	for (i = 0; i < DB_SIZE; i++)
	{
		if ((GuestDB[i].number == ItemPtr->number) &&
			(GuestDB[i].table == ItemPtr->table))
		{
			printf("Guest number %d indeed sits at table number %d!\n",
				ItemPtr->number, ItemPtr->table);
			return 1;
		}
		Sleep(1); /* <ISP> slowing the search down so that threading behavior */
				  /* becomes more visible */
	}
	printf(" !!! Guest number %d doesn't sit at table number %d !!!\n",
		ItemPtr->number, ItemPtr->table);

	return 0;
}

/**
* This function creates a thread by calling Win32 Api's CreateThread()
* function, and setting some of the parameters to default value.
*
* parameters:
* ----------
* StartAddress - a pointer to the function that will be run by the thread.
* ParameterPtr - a pointer to the parameter that will be supplied to the
*                function run by the thread.
* ThreadIdPtr - return argument: a pointer to a DWORD variable into which
*               the function will write the created thread's ID.
*
* returns:
* --------
* On success, a handle to the created thread. On Failure - NULL.
*/
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



void ZeroThreadsProcess() {
	HANDLE process;
}










BOOL CreateProcessSimpleNoCommand(PROCESS_INFORMATION *ProcessInfoPtr)
{
	STARTUPINFO	startinfo = { sizeof(STARTUPINFO), NULL, 0 }; /* <ISP> here we */
															  /* initialize a "Neutral" STARTUPINFO variable. Supplying this to */
															  /* CreateProcess() means we have no special interest in this parameter. */
															  /* This is equivalent to what we are doing by supplying NULL to most other */
															  /* parameters of CreateProcess(). */

	return CreateProcess(NULL, /*  No module name (use command line). */
		NULL,			/*  Command line. */
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




/*	
The next example shows that threads share the same heap. First we declare a global int* without allocating 
any memory. Inside ShareYourHeap() we create NUM_OF_THREADS threads that call AllocateAndPrintArray but only
the one with index 0 initiate the array. So if we change the 'Sleep(50)' line in AllocateAndPrintArray so that
one time the initiate thread will be sleeping, that will cause the others to access uninitiate memory- an exception
*/

//declare int array without allocating memory
int* sharedArray;

int ShareYourHeap() {
	void AllocateAndPrintArray(int[]);
	HANDLE threadHandles[NUM_OF_THREADS];
	DWORD threadIDs[NUM_OF_THREADS];
	
	//Create threads - one of them should init the array
	for (int i = 0; i < NUM_OF_THREADS; i++) {
		threadHandles[i] =	 CreateThreadSimple((LPTHREAD_START_ROUTINE)AllocateAndPrintArray, i, &threadIDs[i]);
	}

	//Wait for thread to finish
	WaitForMultipleObjects(
		NUM_OF_THREADS,
		threadHandles,
		TRUE,       /* wait until all threads finish */
		INFINITE);

	for (int i = 0; i < NUM_OF_THREADS; i++)
	{
		CloseHandle(threadHandles[i]);
	}

	free(sharedArray);
	
}
	
void AllocateAndPrintArray(int threadIndex) {
	if (threadIndex != 0)
		Sleep(50);
	if (threadIndex == 0) {
		//Sleep(50);
		sharedArray = (int*)calloc(NUM_OF_THREADS, sizeof(int));
		for (int i = 0; i < NUM_OF_THREADS; i++) {
			sharedArray[i] = GetCurrentProcessId()+ i*threadIndex;   //The multiplication assure that thread 0 is initiating the sharedArray            
		}
		printf("Thread of index %d was the initiator\n", threadIndex);
	}
	printf("Thread of round %d printing sharedArray at 0x%p: {%u, %u, %u}\n", threadIndex ,sharedArray, 
		sharedArray[0], sharedArray[1], sharedArray[2]);

};