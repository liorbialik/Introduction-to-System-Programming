#include <stdio.h>
#include <stdlib.h>
#include "errorHandling.h"

// using 'extern' makes sure the error variable is the not duplicated but rather the same one across the program //
extern int errorNumber;


FILE* getInputFileData(char* inputFilName) {
	FILE *inputFile;
    inputFile = fopen(inputFilName, "r");
	if (inputFile == NULL) {
        printErrorAndExitProgram(errorNumber);
	}
	return inputFile;
}


char* readFileDataIntoBufferArray(FILE *fileToRead){
    long bufferArraySize;
    char *bufferArray;
    char *allocationErrorStr[20] = "memory alloc fails";

    fseek(fileToRead, 0L, SEEK_END); // Seek to the end of the file //
    bufferArraySize = ftell(fileToRead); // set the size of the buffer to allocate. //
    if (buffer == NULL) {
        printErrorAndExitProgram(errorNumber, allocationErrorStr);
    }

    rewind(fileToRead); // reset to point to the beginning of the file //



    return bufferArray;
}


//int writeDataToOutputFile(char* outputData){}
//
//char* callSolver(char *data)
//{
//	char *solution = 0;
//
//	return solution;
//}
//
//char* callChecker(char *data)
//{
//	char *solution = 0;
//
//	return solution;
//}



int main(int argc, char *argv[]){
	
	FILE *inputFile;
	char* runMode, inputFileName, inputBufferArray;
	char *data[11][11] = {0}; // TODO: need to add a #define ROWS/COLS in .h file

    // char instructionInputCmd[??]  and add #include <string.h>
    // printf("Enter The following:\n <Working Mode> <Input FileName> <Output FileName - Optional>\n");
    // fgets(instructionInputCmd, 10, stdin);

	// TODO: check if the arguments are givent at run time or needed inside the program
	runMode = argv[1]; 
	inputFileName = argv[2];
	// open the file from argv[1]
	inputFile = getInputFileData(inputFileName);
    inputBufferArray = readFileDataIntoBufferArray(inputFile);

    // parse file to manegable format (without the seperations?, create structures?)
	// check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)
	//switch (runMode)
	//{
	//case 0:
	//	callSolver(data);
	//	break;
	//case 1:
	//	callChecker(data);
	//	break;
	//}
	// reparse the data into the solutions style
	// write the reparsed-date into the output file (argv[2])
	getchar();
    return 0;
}