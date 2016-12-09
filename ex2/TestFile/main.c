/*
TestFile - Main.c:
- the program recieves the following arguments from testManager.exe:
@param filePath: the path of the file which tests will be executed on
@param OutputLogFile: the path of the log file to which the results will be written to
- the program will then send these arguments to the function that executes the tests
*/


/* Libraries: */
#include <stdio.h>
#include "testFile.h"

int main(int argc, char *argv[]) {
	
	int returnValueFromTestModule = 0;

	returnValueFromTestModule = executeTestsOnFile(argv);

	return 0;
}

