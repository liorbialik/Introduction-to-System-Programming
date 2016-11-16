#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"
#include "Checker.h"
#include <ctype.h>
#include <string.h>

// functions decelerations:
void readFileDataIntoBufferArray(FILE *fileToRead, char bufferArray[]);
void parseInputBufferIntoMatrix(char parsedSudokuMatrix[][9], char *inputBufferArray);
void parseMatrixIntoOutputFile(FILE *fileToWriteInto, char parsedSudokuMatrix[][9]);
//void writeDataToOutputFile(char *outputData, char *outputFileName);


int main(int argc, char *argv[]) {

	FILE *inputFile = NULL, *outputFile = NULL;
	int runMode = 0;
	size_t outputFileNameLength = 0;
	char *inputFileName = NULL, *outputFileName = NULL, *inputBufferArrayPtr = NULL, *outputFileNameEnding = { "_sol.txt" };//, *parsedSudokuMatrixPtr = NULL; //, *solution;
	char inputBufferArray[81] = { "" }, parsedSudokuMatrix[9][9] = { 0 };
	inputBufferArrayPtr = inputBufferArray;

	runMode = argv[1][0] - '0'; //convert the input running mode into an integer.
	inputFileName = argv[2];
	if (argc < 4) { // if no output file name was given, use the input file name and add "_sol" to its end
		outputFileNameLength = strlen(inputFileName) + strlen(outputFileNameEnding) - 4 + 1; // '-4' deducts the '.txt' ending since it appearse twice. '+1' accounts for the '\0' ending
		outputFileName = malloc(sizeof(char) * outputFileNameLength); // the '4' stands for the shared ".txt" 
		if (outputFileName == NULL) {
			printf("outputFileName allocation faild.");
			exit(1);
		}
		strcpy(outputFileName, inputFileName);
		outputFileName[strlen(outputFileName) - 4] = '\0'; // mark the end of the precious string before the '.txt' ending
		strcat(outputFileName, outputFileNameEnding); // add the correct ending
	}
	else {
		outputFileName = argv[3];
	}
	
	inputFile = fopen( inputFileName, "r" );
	if (inputFile == NULL) {
		printf("Input File openning failed!");
		exit(1);
	}

	readFileDataIntoBufferArray(inputFile, inputBufferArrayPtr);
	fclose(inputFile);
	parseInputBufferIntoMatrix(parsedSudokuMatrix, inputBufferArray); // parse file to manegable format (9X9 matrix)
	
	// check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)
	switch (runMode) {
	case 0: {
		callSolver(parsedSudokuMatrix);
		break;
		}
	
	case 1: {
		callChecker(parsedSudokuMatrix);
		break;
		}
	
	default:
		printf("invalid running mode"); // TODO: call invalid argument error and exit
	}

	outputFile = fopen( outputFileName, "w" );
	if (outputFile == NULL) {
		printf("Input File openning failed!");
		exit(1);
	}
	parseMatrixIntoOutputFile(outputFile,parsedSudokuMatrix);
	fclose(outputFile);
	//writeDataToOutputFile(parsedSudokuMatrix, outputFileName);
	free(outputFileName);
	return(0);
}

void readFileDataIntoBufferArray(FILE *fileToRead, char bufferArray[]) {
	int i = 0;
	char charFromInput;

	while (( charFromInput = fgetc(fileToRead)) != EOF ) {
		if ( ( charFromInput == '.' ) || isdigit(charFromInput) ){
			bufferArray[i] = charFromInput;
			i++;
		}
		else {
			continue;
		}
	}
}

void parseInputBufferIntoMatrix(char parsedSudokuMatrix[][9], char *inputBufferArray) {
	int i, j, inputBufferArraySize = 81;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			parsedSudokuMatrix[i][j] = inputBufferArray[i * inputBufferArraySize/9 + j];
		}
	}
}


//TODO: need to validate
void parseMatrixIntoOutputFile(FILE *fileToWriteInto, char parsedSudokuMatrix[][9]){
	int i, j;

	for ( i = 0 ; i < 9 ; i++){
		for ( j = 0 ; j < 9 ; j++){
			fputc(" " ,fileToWriteInto);
			fputc(parsedSudokuMatrix[i][j] ,fileToWriteInto);
			if( (j == 3) || (j == 6) ){
				fputc(" |" ,fileToWriteInto);
			}
		}
		if( (i+1)%3 == 0){
			fputc("-------+-------+-------\n" ,fileToWriteInto);
		}
		else{
			fputc("\n" ,fileToWriteInto);
		}
	}
}

//void writeDataToOutputFile(char *outputData, char *outputFileName) {
//	FILE *outputFile;
//	char *openFileErrorStr[27] = "Output File creation failed!";
//
//	outputFile = fopen_s(outputFileName, "w+");
//	if (outputFile == NULL) {
//		printErrorAndExitProgram(errorNumber, openFileErrorStr);
//	}
//	fputs(outputData, outputFile);
//	fclose(outputFile);
//}
// TODO: need to make an external module for error pronting: printErrorToFile(char *errorPtr, FILE *outputFile)
