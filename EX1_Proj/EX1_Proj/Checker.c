/*
Checker.c:
- The program recieves a solved soduko char matrix as an input
- For each group in the matrix (rows, columns and subGrids), the algorithm checks whether there is only one assignment value.
- If the whole soduko is valid, the program will return "No errors found" message.
- Otherwise, the program will detect row, col and subGrid errors respectively, and will output an appropriate message ellaborating the duplicated value and its duplicated locations.
- In both cases, the output will be written to the solution file txt.
Tomer Shahar 301359410, Lior Bialik 301535316
*/ 

#include <stdio.h>
#include <stdlib.h>
#include "Checker.h"
#include "Common.h"

// functions declerations:
void printRowDuplications(char soduko[9][9], int numOfDuplications, int row);
void printColDuplications(char soduko[9][9], int numOfDuplications, int col);
void printSubGridDuplications(char soduko[9][9], int numOfDuplications, int row, int col);


/*int main()
{
	char soduko[9][9] = {
		{'3','7','5','9','6','1','8','2','4'},
		{'4','6','2','5','3','8','1','9','7'},
		{'8','9','1','7','2','4','3','6','5'},
		{'1','3','6','2','1','9','7','4','8'},
		{'2','8','9','3','4','7','5','1','6'},
		{'7','5','4','8','1','6','2','3','9'},
		{'6','4','7','1','8','3','9','5','2'},
		{'9','2','3','4','7','5','6','8','1'},
		{'5','1','8','6','9','2','4','7','3'},

	};

	callChecker(soduko);

	return 0;
}
*/

void callChecker(FILE *fileToWriteInto, char soduko[9][9])
{/*
	@ Description: recieves a 2D matrix soduko and validate its correctness. if valid output "No error message", ow will ellaborate the errors.
	@ Param soduko: The matrix containing the numbers.
	@ Return: None
	*/
	int i = 0, j = 0, k = 0;
	int errorMessageWasPrintedFlag = 0, colErrorWasPrintedFlag = 0, subGridErrorWasPrintedFlag = 0;
	int solutionRowErrorCounter = 0;
	int solutionColErrorCounter = 0;
	int solutionSubGridErrorCounter = 0;	//indicated the number of error duplications in a particular group

	for (i = 0; i < 9; i++) {			 //	going over the rows and check for errors
		int flagArrayForPossibleErrors[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		callCheckRow(soduko, flagArrayForPossibleErrors, i);
		for (k = 0; k < 9; k++) {
			solutionRowErrorCounter += flagArrayForPossibleErrors[k];
		}
		if (solutionRowErrorCounter != 0) {
			if (!errorMessageWasPrintedFlag) {
				printf("Found errors in given Sudoku puzzle.\nThe errors are:\n");
				/*fputs("Found errors in given Sudoku puzzle.\nThe errors are:\n", fileToWriteInto);
				soduko[0][0] = '0'; // this will be used in the main module for checking
				return;*/
				errorMessageWasPrintedFlag = 1;
			}
			printRowDuplications(soduko, solutionRowErrorCounter, i);	
		}
	}

	for (j = 0; j < 9; j++) {				//	going over the cols and check for errors
		int flagArrayForPossibleErrors[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		callCheckCol(soduko, flagArrayForPossibleErrors, j);
		for (k = 0; k < 9; k++) {
			solutionColErrorCounter += flagArrayForPossibleErrors[k];
		}
		if (solutionColErrorCounter != 0) {
			if (!errorMessageWasPrintedFlag) {
				printf("Found errors in given Sudoku puzzle.\nThe errors are:");
				/*fputs("Found errors in given Sudoku puzzle.\nThe errors are:\n", fileToWriteInto);
				soduko[0][0] = '0'; // this will be used in the main module for checking
				return;*/
				errorMessageWasPrintedFlag = 1;
			}
			printColDuplications(soduko, solutionColErrorCounter, j);
		}
	}

	for (i = 0; i < 3; i++) {				//	going over the subGrids and check for errors
		for (j = 0; j < 3; j++) {
			int subRow = 3 * i, subCol = 3 * j;
			int flagArrayForPossibleErrors[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
			callCheckSubGrid(soduko, flagArrayForPossibleErrors, subRow, subCol);
			for (k = 0; k < 9; k++) {
				solutionSubGridErrorCounter += flagArrayForPossibleErrors[k];
			}
			if (solutionSubGridErrorCounter != 0) {
				if (!errorMessageWasPrintedFlag) {
					printf("Found errors in given Sudoku puzzle.\nThe errors are:");
					/*fputs("Found errors in given Sudoku puzzle.\nThe errors are:\n", fileToWriteInto);
					soduko[0][0] = '0'; // this will be used in the main module for checking
					return;*/
					errorMessageWasPrintedFlag = 1;
				}
			printSubGridDuplications(soduko, solutionSubGridErrorCounter, subRow, subCol);
			}
		}
	}

	if (!errorMessageWasPrintedFlag) {		//if errorMessageWasPrintedFlag equals zero, it means no errors were detected by now
		//printf("No errors found in given Sudoku puzzle.");
		fputs("No errors found in given Sudoku puzzle.", fileToWriteInto);
		soduko[0][0] = '0'; // this will be used in the main module for checking
		return;
	}
}


void printRowDuplications(char soduko[9][9], int numOfDuplications, int row)
{/*
 @ Description: This function ellaborates row errors that were indicated at the given Soduko;
 @ Param soduko: The matrix and number of duplication that were indicated
 @ Return: None
 */
	int i, j;
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (soduko[row][i] == soduko[row][j]) {
					printf("Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row + 1, i + 1, row + 1, j + 1);
					//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
				}
			}
		}
}

void printColDuplications(char soduko[9][9], int numOfDuplications, int col) 
{/*
 @ Description: This function ellaborates col errors that were indicated at the given Soduko;
 @ Param soduko: The matrix and number of duplication that were indicated
 @ Return: None
 */
	int i, j;
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (soduko[i][col] == soduko[j][col]) {
					printf("Column error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[i][col], i + 1, col + 1, j + 1, col + 1);
					//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
				}
			}
		}
}

void printSubGridDuplications(char soduko[9][9], int numOfDuplications, int row, int col) 
{/*
 @ Description: This function ellaborates subGrid errors that were indicated at the given Soduko;
 @ Param soduko: The matrix, number of duplication that were indicated and the particular subGrid containing the errors
 @ Return: None
 */
	int i, j, k = 0;
	char subGridArray[9] = { '0' };
	int rowIndexArray[9] = { row,row,row,row + 1,row + 1,row + 1,row + 2,row + 2,row + 2 };
	int colIndexArray[9] = { col,col + 1,col + 2,col,col + 1,col + 2,col,col + 1,col + 2 };

		//Convert subGrid to subGridArray
		for (i = row; i < row + 3; i++) {
			for (j = col; j < col + 3; j++) {
				subGridArray[k] = soduko[i][j]; k++;
			}
		}

		//identifying duplicated values and printing them
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (subGridArray[i] == subGridArray[j]) {
					printf("SubGrid error: digit %c appears at (%d,%d) and (%d,%d)\n", subGridArray[i], rowIndexArray[i] + 1, colIndexArray[i] + 1, rowIndexArray[j] + 1, colIndexArray[j] + 1);
				}
				//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
			}

		}
}