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


void writeDataToOutputFile(char* outputData, outputFileName);

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
	char* runMode, inputFileName, outputFileName, inputBufferArray, solution;
	char *data[11][11] = {0}; // TODO: need to add a #define ROWS/COLS in .h file

	runMode = argv[1]; 
	inputFileName = argv[2];
    // open the file from argv[1]
	inputFile = getInputFileData(inputFileName);
    inputBufferArray = readFileDataIntoBufferArray(inputFile);

    // parse file to manegable format (9X9 matrix)
	// check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)
	switch (runMode)
	{
	case "0":
        solution = callSolver(data);
		break;

    case "1":
        solution = callChecker(data);
		break;

    default :
        print("invalid running mode"); // TODO: call invalid argument error and exit
    }

    // check if argv[3] (output name) was given:
    //  - if yes => outputFileName = argv[3]
    //  - if no => outputFileName = inputFileName - ".txt" + "_sol" + ".txt"
    // write the reparsed-date into the output file (outputFileName)
    writeDataToOutputFile(solution, outputFileName);
	getchar();
    return 0;
}