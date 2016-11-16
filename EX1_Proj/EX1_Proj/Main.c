#include <stdio.h>
#include <stdlib.h>
#include "errorHandling.h"
#include <ctype.h>

// using 'extern' makes sure the error variable is the not duplicated but rather the same one across the program //
extern int errorNumber;

// functions decelerations:
FILE getInputFileData(char* inputFilName);
char* readFileDataIntoBufferArray(FILE *fileToRead);
char* parseInputBufferIntoMatrix(char *parsedSudokuMatrix, char *inputBufferArray);
void writeDataToOutputFile(char *outputData, char *outputFileName);


int main(int argc, char *argv[]) {

	FILE *inputFile;
	int runMode = 0;
	char* inputFileName, outputFileName, inputBufferArray, solution;
	char *outputFileNameEnding[8] = "_sol.txt", *parsedSudokuMatrix[9][9] = { 0 }; // TODO: need to add a #define ROWS/COLS in .h file

	runMode = argv[1][0] - '0'; //convert the input running mode into an integer.
	inputFileName = argv[2];
	if (argc < 4) { // if no output file name was given, use the input file name and add "_sol" to its end
		outputFileName = argv[2];
		outputFileName[strlen(inputFileName) - 4] = '\0'; // remove the ".txt" ending from the input string
		strcat(outputFileName, outputFileNameEnding); // add the correct ending
	}
	else
		outputFileName = argv[3];

	inputFile = getInputFileData(inputFileName);
	inputBufferArray = readFileDataIntoBufferArray(inputFile);

	parseInputBufferIntoMatrix(parsedSudokuMatrix, inputBufferArray); // parse file to manegable format (9X9 matrix)
																	  // check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)
	switch (runMode)
	{
	case 0:
		parsedSudokuMatrix = 'temp';//callSolver(parsedSudokuMatrix);
		break;

	case 1:
		parsedSudokuMatrix = 'temp';//callChecker(parsedSudokuMatrix);
		break;

	default:
		printf("invalid running mode"); // TODO: call invalid argument error and exit
	}

	writeDataToOutputFile(parsedSudokuMatrix, outputFileName);
	getchar();
	return 0;
}

FILE getInputFileData(char* inputFilName) {
	FILE *inputFile;
	char *openFileErrorStr[26] = "Input File openning failed!";

	fopen_s(inputFile, inputFilName, "r");
	if (inputFile == NULL) {
		printErrorAndExitProgram(errorNumber, openFileErrorStr);
	}
	return *inputFile;
}

char* readFileDataIntoBufferArray(FILE *fileToRead) {
	int i = 0, bufferArraySize = 81;
	char *bufferArray[bufferArraySize];
	char *allocationErrorStr[20] = { 0 };
	char charFromInput;

	allocationErrorStr = "memory alloc fails";

	//fseek(fileToRead, 0L, SEEK_END); // Seek to the end of the file //
	//inputFileLength = ftell(fileToRead); // set the size of the buffer to allocate. //
	//rewind(fileToRead); // reset to point to the beginning of the file //
	// if (inputFileLength > 255)
	//     printf("Invalid input size"); exit(1);


	while ((charFromInput = getc(fileToRead)) != EOF) {
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
// TODO: need to make an external module for error pronting: printErrorToFile(char *errorPtr, FILE *outputFile)
