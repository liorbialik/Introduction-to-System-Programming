/*
TestManager - Main.c:
- the program recieves the following arguments:
@param FilesToTest: The text file which consists the processes to be tested
@param OutputFilesDirectory: The output file directory that will hold in it all log files
- the program will then send these arguments to the function that manages the processes
*/


/* Libraries: */
#include <stdio.h>
#include "TestManager.h"

int main(int argc, char *argv[]) {

	int returnValueFromManagerModule = 0;

	returnValueFromManagerModule = executeManagerOnFile(argv, argc);

	return 0;
}

