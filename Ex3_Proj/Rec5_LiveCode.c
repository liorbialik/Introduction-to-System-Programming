#define _CRT_SECURE_NO_WARNINGS /* to suppress compiler warnings (VS 2010 ) */
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>

/*oOoOoOoOoOoOoOoOoOoOoOoOoO Types oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

typedef enum { BASE_G, BASE_T, BASE_C, BASE_A, NUM_OF_BASES } Bases_t;

typedef enum { 
    ISP_SUCCESS, 
    ISP_FILE_OPEN_FAILED, 
    ISP_FILE_SEEK_FAILED,
    ISP_FILE_READING_FAILED,
    ISP_FTELL_FAILED,
    ISP_MUTEX_OPEN_FAILED,
    ISP_MUTEX_CREATE_FAILED,
    ISP_MUTEX_WAIT_FAILED,
    ISP_MUTEX_ABANDONED,
    ISP_MUTEX_RELEASE_FAILED,
    ISP_ILLEGAL_LETTER_WAS_READ
} ErrorCode_t;

typedef struct 
{
    long int Start;
    long int End;
} Range_t;

typedef struct {
    Range_t FilePosition;
    HANDLE MutexHandleA;
} ThreadFuncInput_t;

/*oOoOoOoOoOoOoOoOoOoOoOoOo Constants oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

#define NUM_OF_THREADS 2

/*oOoOoOoOoOoOoOoOoOoOoOoOo Function Declarations oOoOoOoOoOoOoOoOoOoOoOoO*/

HANDLE CreateThreadSimple(  LPTHREAD_START_ROUTINE StartAddress, 
                            LPVOID ParameterPtr, 
                            LPDWORD ThreadIdPtr );

DWORD CountBases( LPVOID Argument );

BOOL DivideGenomeFilePositions( 
    Range_t* FirstRangePtr, 
    Range_t* SecondRangePtr );

HANDLE CreateMutexSimple( LPCTSTR MutexName );

DWORD ScanFileAndRecordOccurences(
    FILE* GenomeFile,
    ThreadFuncInput_t *I,
    HANDLE MutexHandleT,
    HANDLE MutexHandleC );

DWORD LockUpdateAndUnlock( Bases_t Base, HANDLE MutexHandle );

/*oOoOoOoOoOoOoOoOoOoO Global Variables oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

/* Global memory is used to store a resource shared between several threads */

const char* GenomeFileName = "Genome.txt";

LPCTSTR MutexNameG = _T( "GenomeMutexG" );
LPCTSTR MutexNameT = _T( "GenomeMutexT" );
LPCTSTR MutexNameC = _T( "GenomeMutexC" );
LPCTSTR MutexNameA = _T( "GenomeMutexA" );

volatile unsigned int Histogram[ NUM_OF_BASES ] = { 0 }; /* initialize all cells to zero */

HANDLE MutexHandleG;

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

void main()
{
    Range_t Ranges[NUM_OF_THREADS];
    HANDLE ThreadHandles[NUM_OF_THREADS];    
    ThreadFuncInput_t ThreadInput[NUM_OF_THREADS];
    HANDLE MutexHandleC;
    HANDLE MutexHandleA;
    DWORD ExitCode;
    DWORD WaitRes;    
    int i;
    int ThreadInd;
    
    /* ----------------------------- */
    /* 1. Divide file into two areas */
    /* ----------------------------- */
    BOOL DivisionSucceeded = DivideGenomeFilePositions( 
        &(Ranges[0]), 
        &(Ranges[1]) );

    if ( !DivisionSucceeded ) 
    { 
        printf("Division of file areas failed. ending program" ); 
        exit(1); 
    }    
    
    /* ----------------------------- */
    /*  2. Create Mutexes            */
    /* ----------------------------- */

    /* Store Mutex G's handle in a global variable. All threads will */
    /* use this variable. */
    MutexHandleG = CreateMutexSimple( MutexNameG );

    if ( MutexHandleG == NULL) 
        { printf("CreateMutex error: %d\n", GetLastError()); exit(1); }

    /* Mutex A's handle will be passed as an argument to the threads */
    MutexHandleA = CreateMutexSimple( MutexNameA );
    
    if (MutexHandleA == NULL) 
        {printf("CreateMutex error: %d\n", GetLastError()); goto Main_Cleanup_3;}
   
    /* Mutex C is created, but the handle is not passed to the threads. Each */
    /* thread will get a handle to it by using the function OpenMutex(), with */
    /* Mutex C's name as an argument */
    MutexHandleC = CreateMutexSimple( MutexNameC );

    if ( MutexHandleC == NULL) 
        { printf("CreateMutex error: %d\n", GetLastError()); goto Main_Cleanup_2; }

    /* Mutex T is not created here. It will be created by the worker threads. */ 

    /* ----------------------------- */
    /*      3. Create Threads        */
    /* ----------------------------- */
    for ( ThreadInd = 0; ThreadInd < NUM_OF_THREADS; ThreadInd++ )
    {        
        ThreadInput[ThreadInd].MutexHandleA = MutexHandleA;
        ThreadInput[ThreadInd].FilePosition = Ranges[ThreadInd];

        ThreadHandles[ThreadInd] = CreateThreadSimple(  
            (LPTHREAD_START_ROUTINE)CountBases, 
            (LPVOID)&(ThreadInput[ThreadInd]), 
            NULL ); /* not storing the ID of the created thread */

        if ( ThreadHandles[ThreadInd] == NULL )
        {
            printf("Failed to create Thread. Exiting program.\n");
            goto Main_Cleanup_1;
        }
    }

    WaitRes = WaitForMultipleObjects( 
        NUM_OF_THREADS, 
        ThreadHandles, 
        TRUE /* wait for all */, 
        INFINITE );

    if ( WaitRes == WAIT_FAILED )
    { 
        printf("Waiting for threads failed. Ending program.\n"); 
        goto Main_Cleanup_1; 
    }

    /* ------------------ */
    /* 4. Output results  */
    /* ------------------ */

    printf("Base occurrences in file: G %d   T %d   C %d   A %d\n",
        Histogram[BASE_G], 
        Histogram[BASE_T], 
        Histogram[BASE_C], 
        Histogram[BASE_A] );

    printf("Total Letters in Histogram: %d\n",
        Histogram[BASE_G] + Histogram[BASE_T] + 
        Histogram[BASE_C] + Histogram[BASE_A] );

    /* ------------------- */
    /* 5. Close Everything */
    /* ------------------- */

    /* <ISP> the only legitimate use for the 'goto' command: jumping to a */
    /* cleanup section at the end of the function. In this section, resources */
    /* are released in an opposite order to how they were allocated. If error  */
    /* occurs somewhere in the function, we use goto to jump to the location in */
    /* the cleanup section that will release only the resources that were */
    /* already allocated. */

Main_Cleanup_1:
    
    /* The following loop closes handles to all threads that were created. The */
    /* variable 'ThreadInd' is set to one more than the index of the last thread */
    /* that was created */
    for ( i = 0 ; i < ThreadInd ; i++ )
    {
        GetExitCodeThread( ThreadHandles[i], &ExitCode );
        printf("Thread %d ended with exit code 0x%x\n",i,ExitCode);
        CloseHandle( ThreadHandles[i] );
    }


    CloseHandle( MutexHandleC );

Main_Cleanup_2:
    CloseHandle( MutexHandleA );

Main_Cleanup_3:
    CloseHandle( MutexHandleG );
}


/**
 * This function divides the positions in the Genome file into two ranges of 
 * positions:
 * the first from the beginning to the middle, and the second from the middle to 
 * the end.
 *
 * Accepts
 * -------
 * FirstRangePtr - output parameter, points to a Range_t struct that will 
 *                 be filled with the lower positions.
 * SecondRangePtr - output parameter, points to a Range_t struct that will 
 *                  be filled with the higher positions.
 * Returns: 
 * -------
 * TRUE on success, FALSE on failure
 */ 
BOOL DivideGenomeFilePositions( Range_t* FirstRangePtr, Range_t* SecondRangePtr )
{
    int Res;
    long int LastPosition;
    long int MiddlePosition;
    long int FirstPosition;

    FILE* GenomeFile = fopen( GenomeFileName, "r" );
    
    if ( GenomeFile == NULL ) return FALSE;

    FirstPosition = ftell( GenomeFile );
    
    if ( FirstPosition == -1L ) { fclose( GenomeFile ); return FALSE; }

    Res = fseek( GenomeFile, -1, SEEK_END );

    if ( Res != 0 ) { fclose( GenomeFile ); return FALSE; }    

    LastPosition = ftell( GenomeFile );

    if ( LastPosition == -1L ) { fclose( GenomeFile ); return FALSE; }

    MiddlePosition = LastPosition / 2; 
    
    FirstRangePtr->Start = FirstPosition;
    FirstRangePtr->End = MiddlePosition ;
    SecondRangePtr->Start = MiddlePosition + 1;
    SecondRangePtr->End = LastPosition;

    fclose( GenomeFile );
    return TRUE;  
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
HANDLE CreateThreadSimple(  LPTHREAD_START_ROUTINE StartAddress, 
                            LPVOID ParameterPtr, 
                            LPDWORD ThreadIdPtr )
{
    return CreateThread(
                        NULL,            /*  default security attributes */
                        0,                /*  use default stack size */
                        StartAddress,    /*  thread function */
                        ParameterPtr,    /*  argument to thread function */
                        0,                /*  use default creation flags */
                        ThreadIdPtr );    /*  returns the thread identifier */
}

/**
 * This is the thread function. It reads a part of a file and counts occurences
 * of the different letters in it, and records them in the global variable
 * Histogram. 
 * <ISP> this function demonstrates creation, destruction and use of Mutexes.
 */ 
DWORD CountBases( LPVOID Argument )
{
    ThreadFuncInput_t *I = (ThreadFuncInput_t*)Argument;
    int SeekRes;
    BOOL Stop = FALSE ;
    HANDLE MutexHandleC;
    HANDLE MutexHandleT;
    DWORD ExitCode = ISP_SUCCESS; /* if no errors occur, this value */ 
                                    /* will be returned */

    FILE* GenomeFile = fopen( GenomeFileName, "r" );

    if ( GenomeFile == NULL ) return ISP_FILE_OPEN_FAILED;

    SeekRes = fseek( GenomeFile, I->FilePosition.Start, SEEK_SET );

    if ( SeekRes != 0 ) 
        { ExitCode = ISP_FILE_SEEK_FAILED; goto CountBases_Cleanup_3; }

    /* Get a handle to MutexC, that was already created by the main thread: */
    MutexHandleC = OpenMutex( 
        SYNCHRONIZE, /* default value */
        FALSE,       /* default value */
        MutexNameC ); /* <ISP> This MUST be the EXACT same name as was used when */
                      /* the mutex was created. To save heartache, use a string */
                      /* constants ( as is done here ). */
    
    if ( MutexHandleC == NULL )
    {
        if ( GetLastError() == ERROR_FILE_NOT_FOUND )
            printf("A mutex with the requested name does not exit.\n");
        
        ExitCode = ISP_MUTEX_OPEN_FAILED; 
        goto CountBases_Cleanup_3;
    }

    MutexHandleT = CreateMutexSimple( MutexNameT );

    if ( MutexHandleT == NULL ) 
    { 
        ExitCode = ISP_MUTEX_CREATE_FAILED; 
        goto CountBases_Cleanup_2; 
    }
    else
    {
        if ( GetLastError() == ERROR_ALREADY_EXISTS )
            printf("Thread %d using already existing mutex T\n", 
                    GetCurrentThreadId() );
        else
            printf("Thread %d created mutex T\n", GetCurrentThreadId() );
    }
    
    ExitCode = ScanFileAndRecordOccurences( 
        GenomeFile,
        I, 
        MutexHandleT,
        MutexHandleC );

    if ( ExitCode != ISP_SUCCESS ) goto CountBases_Cleanup_1;

CountBases_Cleanup_1:
    CloseHandle(MutexHandleC);
CountBases_Cleanup_2:
    CloseHandle(MutexHandleT);
CountBases_Cleanup_3:
    fclose( GenomeFile ); 

    /* Do not close I->MutexHandleA !!! a copy of it exists in each thread, */
    /* so if we close it here, it will be closed multiple times. Instead,  */
    /* the main thread should close it. */

    return ExitCode;
}

/**
 * This function uses the win32 api function CreateMutex() to create 
 * a named mutex, without accepting the ownership for it (in other words, 
 * without locking it).
 */
HANDLE CreateMutexSimple( LPCTSTR MutexName )
{
    return CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        MutexName);             
}

/**
 * This function implements the basic lock, critical section, unlock
 * part of the algorithm. In the critical section, this function
 * writes into the global variable Histogram.
 */
DWORD LockUpdateAndUnlock( Bases_t Base, HANDLE MutexHandle )
{
    BOOL ReleaseRes;

    DWORD WaitRes = WaitForSingleObject( MutexHandle, INFINITE );
                
    if ( WaitRes != WAIT_OBJECT_0 )
    { 
        if ( WaitRes == WAIT_ABANDONED )
        {
            printf("Some thread has previously exited without releasing a mutex."
                    " This is not good programming. Please fix the code.\n" );
            return ( ISP_MUTEX_ABANDONED );
        }
        else
            return( ISP_MUTEX_WAIT_FAILED );
    }
                
    /* ........Critical Section Start................ */

    Histogram[ Base ]++;

    /* ........Critical Section End.................. */
    
    ReleaseRes = ReleaseMutex( MutexHandle );

    if ( ReleaseRes == FALSE )
        return ( ISP_MUTEX_RELEASE_FAILED );

    return ( ISP_SUCCESS );
}

/**
 * This function goes over a part of the file, and records the number of 
 * occurrences of each letter in the global Histogram array.
 */
DWORD ScanFileAndRecordOccurences(
    FILE* GenomeFile,
    ThreadFuncInput_t *I,
    HANDLE MutexHandleT,
    HANDLE MutexHandleC )
{
    DWORD ExitCode;
    int Letter;
    BOOL Stop = FALSE;

    while ( !Stop )
    {
        long int Position = ftell(GenomeFile);

        if ( Position == -1L ) return ( ISP_FTELL_FAILED );

        if ( Position == I->FilePosition.End ) 
            Stop = TRUE; /* handle last letter, then exit loop */

        Letter = fgetc( GenomeFile ); /* automatically advances the file */ 
        /* position indicator, affecting the result of the next call to ftell() */

        if ( Letter == EOF ) return ( ISP_FILE_READING_FAILED );

        switch( Letter )
        {
        case 'G':
            ExitCode = LockUpdateAndUnlock( BASE_G, MutexHandleG );  //MutexHandleG is global
            if ( ExitCode != ISP_SUCCESS ) return( ExitCode );
            break;
        case 'A':
            ExitCode = LockUpdateAndUnlock( BASE_A, I->MutexHandleA ); 
            if ( ExitCode != ISP_SUCCESS ) return( ExitCode );
            break;
        case 'T':
            ExitCode = LockUpdateAndUnlock( BASE_T, MutexHandleT );
            if ( ExitCode != ISP_SUCCESS ) return( ExitCode );
            break;
        case 'C':
            ExitCode = LockUpdateAndUnlock( BASE_C, MutexHandleC );
            if ( ExitCode != ISP_SUCCESS ) return( ExitCode );
            break;
        default:
            return( ISP_ILLEGAL_LETTER_WAS_READ );
            
        };

        Sleep(10); /* <ISP> slowing the operation down so that threading */
                  /* behavior becomes more visible */
    }

    return ISP_SUCCESS;
}