#pragma once
/*
TestManager - TestManager.h:
- the header file for TestManager.c which contains the execution for funning the Processes.
- this file contains the definitions, used structs and the decleration which used to call the main
function in TestManager.c
*/

#ifndef TestManager_H
#define TestManager_H

/* Constants: */
#define LONG_SLEEP_TIME 5000
#define SHORT_SLEEP_TIME 5
#define SECOND_DIVIDER 10000000000
#define MILLI_SECOND_DIVIDER 1000000


/* Libraries: */
#include <Windows.h>


/* Struct Declarations: */
typedef struct PROCESS_TIME_INF {
	FILETIME CreationTime;
	FILETIME ExitTime;
	FILETIME KernelTime;
	FILETIME UserTime;
} PROCESS_TIME_INF;

typedef struct FILE_TIME {
	DWORD second;
	DWORD millisecond;
} FILE_TIME;

/* Function Declarations: */
int executeManagerOnFile(char *argv[], int argc);

#endif#pragma once
#pragma once