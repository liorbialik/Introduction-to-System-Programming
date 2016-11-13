#include <stdio.h>
#include <stdlib.h>
#include "errorHandling.h"
#include <ctype.h>

// using 'extern' makes sure the error variable is the not duplicated but rather the same one across the program //
extern int errorNumber;


FILE getInputFileData(char* inputFilName) {
	FILE *inputFile;
	char *openFileErrorStr[26] = "Input File openning failed!";

	fopen_s(inputFile, inputFilName, "r");
	if (inputFile == NULL) {
        printErrorAndExitProgram(errorNumber, openFileErrorStr);
	}
	return *inputFile;
}


char* readFileDataIntoBufferArray(FILE *fileToRead){
    int i = 0 ,bufferArraySize = 81;
    char *bufferArray[bufferArraySize];
    char *allocationErrorStr[20] = {0};
    char charFromInput;

    allocationErrorStr = "memory alloc fails";

    fseek(fileToRead, 0L, SEEK_END); // Seek to the end of the file //
    inputFileLength = ftell(fileToRead); // set the size of the buffer to allocate. //
    rewind(fileToRead); // reset to point to the beginning of the file //

    while((charFromInput = getc(fileToRead)) != EOF){
        if ((charFromInput == ".") || (charFromInput > "0" && charFromInput < "10"))
            bufferArraySize[i] = charFromInput;
        else
            continue;
    }

    return bufferArray;
}

char* parseInputBufferIntoMatrix(char *parsedSudokuMatrix, char *inputBufferArray) {
    int i, j, inputBufferArraySize = 81;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            parsedSudokuMatrix[i][j] = inputBufferArray[i*inputBufferArraySize + j];
        }
    }
}

void writeDataToOutputFile(char *outputData, char *outputFileName) {
    FILE *outputFile;
    char *openFileErrorStr[27] = "Output File creation failed!";

    outputFile = fopen_s(outputFileName, "w+");
    if (outputFile == NULL) {
        printErrorAndExitProgram(errorNumber, openFileErrorStr);
    }
    fputs(outputData, outputFile);
    fclose(outputFile);
}


int main(int argc, char *argv[]){
	
	FILE *inputFile;
	int runMode = 0;
	char* inputFileName, outputFileName, inputBufferArray, solution;
    char *outputFileNameEnding[8] = "_sol.txt", *parsedSudokuMatrix[9][9] = {0}; // TODO: need to add a #define ROWS/COLS in .h file

	runMode = argv[1][0] - '0'; //convert the input running mode into an integer.
	inputFileName = argv[2];
    if (argc < 3){
        // allocate the size of outputFileName to be inputFileName.length - "" + "_sol" + 1 (for '\0')
        // inputFileNameLength = strlen(inputFileName);
        // outputFileName = inputFileName;
        // outputFileName[inputFileNameLength -1] = '\0';
        // strcat(outputFileName, outputFileNameEnding)
        // create the outputFileName = inputFileName - ".txt" + "_sol.txt"
    }
    else
        outputFileName = argv[3];

	inputFile = getInputFileData(inputFileName);
    inputBufferArray = readFileDataIntoBufferArray(inputFile);

    // parse file to manegable format (9X9 matrix)
    parseInputBufferIntoMatrix(parsedSudokuMatrix, inputBufferArray);
	// check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)
	switch (runMode)
	{
	case 0:
		solution = 'temp';//callSolver(parsedSudokuMatrix);
		break;

    case 1:
        solution = 'temp';//callChecker(parsedSudokuMatrix);
		break;

    default :
        printf("invalid running mode"); // TODO: call invalid argument error and exit
    }

    // check if argv[3] (output name) was given:
    //  - if yes => outputFileName = argv[3]
    //  - if no => outputFileName = inputFileName - ".txt" + "_sol" + ".txt"
    // write the reparsed-date into the output file (outputFileName)
    writeDataToOutputFile(solution, outputFileName);
	getchar();
    return 0;
}