/*
TestFile - testFile.h:
- the header file for testFile.c which contains the execution log for threaded tests.
- this file contains the definitions, used structs and the decleration which used to call the main
  function in testFile.c
*/

#ifndef TEST_FILE
#define TEST_FILE

/* Libraries: */
#include <Windows.h>

/* global definition for the number of tests being executed */
#define NUM_OF_THREADS 4

/* restults struc*/
typedef struct testResults
{
	char *inputFileName;
	char *fileExtention;
	DWORD fileSize;
	TCHAR fileCreationTimeString[21];
	TCHAR fileLastModifiedTimeString[21];
	char firstFiveChars[6];
	
	// possible thread errors:
	const char *fileSizeThreadError;
	const char *fileExtentionThreadError;
	const char *firstFiveCharsThreadError;
	const char *fileCreationAndLastModifiedTimeStringThreadError;
} testResults;

/* Function Declarations: */
int executeTestsOnFile(char *argv[]);

#endif