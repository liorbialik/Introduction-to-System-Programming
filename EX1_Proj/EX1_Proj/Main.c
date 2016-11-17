/*
Main.c:
- The program recieves a soduko in a text file as an input
- if runMode = 0: It will try to solve it and return the solution. else it will give an error
- if runMode = 1: It will verify the given solution.

Tomer Shahar 301359410, Lior Bialik 301535316
*/

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

	outputFile = fopen(outputFileName, "w");
	if (outputFile == NULL) {
		printf("Input File openning failed!");
		exit(1);
	}

	readFileDataIntoBufferArray(inputFile, inputBufferArrayPtr);
	fclose(inputFile);
	parseInputBufferIntoMatrix(parsedSudokuMatrix, inputBufferArray); // parse file to manegable format (9X9 matrix)
	
	// check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)
	switch (runMode) {
	case 0: {
		callSolver(outputFile, parsedSudokuMatrix);
		break;
		}
	
	case 1: {
		//callChecker(outputFile, parsedSudokuMatrix);
		break;
		}
	
	default:
		printf("invalid running mode"); // TODO: call invalid argument error and exit
	}

	
	if (parsedSudokuMatrix[0][0] != '0') // if the first cell is '0', no solution could be found and there is no need to print the matrix
		parseMatrixIntoOutputFile(outputFile, parsedSudokuMatrix);
	fclose(outputFile);
	free(outputFileName);
	return(0);
}


void readFileDataIntoBufferArray(FILE *fileToRead, char bufferArray[]) {
	/*
	@ Description: The function recieves a file to read data from and converts it's data into a buffer array of chars.
	@ Param fileToRead: the file from which the data will be extracted.
	@ Param bufferArray: the buffer to which the data is written into.
	@ Return: None
	*/
	
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
	/*
	@ Description: The function recieves a buffer array and converts it into a 2D matrix of chars.
	@ Param parsedSudokuMatrix: the matrix to which the the characters are written into.
	@ Param inputBufferArray: the buffer from which the data is extracted from.
	@ Return: None
	*/
	int i, j, inputBufferArraySize = 81;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			parsedSudokuMatrix[i][j] = inputBufferArray[i * inputBufferArraySize/9 + j];
		}
	}
}


void parseMatrixIntoOutputFile(FILE *fileToWriteInto, char parsedSudokuMatrix[][9]){
	/*
	@ Description: The function recieves a buffer array and converts it into a 2D matrix of chars.
	@ Param parsedSudokuMatrix: the matrix to which the the characters are taken from.
	@ Param fileToWriteInto: the file to which the data will be written into.
	@ Return: None
	*/
	int i, j;

	for ( i = 0 ; i < 9 ; i++){
		for ( j = 0 ; j < 9 ; j++){
			fputs(" ",fileToWriteInto);
			fputc(parsedSudokuMatrix[i][j] ,fileToWriteInto);
			if( (j == 2) || (j == 5) ){
				fputs(" |" ,fileToWriteInto);
			}
		}
		if( (i == 2) || (i == 5) ){
			fputs("\n-------+-------+-------\n" ,fileToWriteInto);
		}
		else{
			fputs("\n" ,fileToWriteInto);
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
