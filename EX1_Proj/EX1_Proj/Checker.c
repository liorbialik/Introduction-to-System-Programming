/*
Checker.c:
- The program recieves a solved soduko char matrix as an input
- For each cell in the matrix, the possible numbers are checked by row, column and sub-grid.
- If a cell has a single possible number that fits, this number becomes the cell's new value.
- During a run on the whole matrix, if there was a value assignment, the matrix will be reviewed again in search for another cell to solve.
- If no cell has been changed during a run, the solution could not be reached by the algorithm and the appropriate message is written.
- if all cells are solved, it will return the solution.
Tomer Shahar 301359410, Lior Bialik 301535316
*/ 
// TODO: update headline

#include <stdio.h>
#include <stdlib.h>
#include "Checker.h"
#include "Solver.h"


void printRowDuplications(char soduko[9][9], int numOfDuplications, int row) 
{/*
	This function ellaborates row errors that were indicated at the given Soduko;
	It gets as a parameter the soduko matrix and rowDuplicationArray which points on the revealed errors;
	and prints the error messages
 */
	int i, j, counter = 0;
	char *errorMessage = NULL;

	do {
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (soduko[row][i] == soduko[row][j]) {
					printf("Line error : digit %c appears at(%d, %d) and (%d, %d)\n", soduko[row][i], row + 1, i + 1, row + 1, j + 1);
					getchar();
					counter++;
					//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
				}
			}
		}
	} while (counter < numOfDuplications);
}


//This function ellaborates col errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and colDuplicationArray which points on the revealed errors;
// and prints the error messages
void printColDuplications(char soduko[9][9], int numOfDuplications, int col) {
	int i, j, counter = 0;
	char *errorMessage = NULL;

	do {
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (soduko[i][col] == soduko[j][col]) {
					printf("Column error : digit %c appears at(%d, %d) and (%d, %d)\n", soduko[i][col], i + 1, col, j + 1, col);
					counter++;
					//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
				}
			}
		}
	} while (counter < numOfDuplications);
}


//This function ellaborates subGrid errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and subGridDuplicationMatrix which points on the revealed errors;
// and prints the error messages
void printSubGridDuplications(char soduko[9][9], int numOfDuplications, int row, int col) {
	int i, j, k = 0, counter = 0;
	char *errorMessage = NULL;
	char subGridArray[9] = { '0' };
	int rowIndexArray[9] = { row,row,row,row + 1,row + 1,row + 1,row + 2,row + 2,row + 2 };
	int colIndexArray[9] = { col,col+1,col+2,col,col + 1,col + 2,col,col + 1,col + 2 };

	do {
		//Convert subGrid to subGridArray
		for (i = row; i < row + 3; i++) {
			for (j = col; j < col + 3; j++) {
				subGridArray[k] = soduko[i][j];
				k++;
			}
		}

		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (subGridArray[i] == subGridArray[j])
					printf("SubGrid error : digit %c appears at(%d, %d) and (%d, %d)\n", subGridArray[i], rowIndexArray[i] + 1, colIndexArray[i] + 1, rowIndexArray[j] + 1, colIndexArray[j] + 1);
				//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
			}
		
		}
	} while (counter < numOfDuplications);
}


//Checker

void callChecker(FILE *fileToWriteInto, char soduko[9][9])
{
	printf("tt");
	getchar();
	int i=0, j=0, k;
	int subRow = 3 * i, subCol = 3 * j;
	int solutionRowErrorCounter = 0;
	int solutionColErrorCounter = 0;
	int solutionSubGridErrorCounter = 0;	//indicated the number of error duplications in a particular template

			//	going over the rows and check for errors
			for (i = 0; i < 9; i++) {
				int flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
				callCheckRow(soduko, flagArrayForPossibleErrors, i);
				for (k = 0; k < 9; k++)
					solutionRowErrorCounter += flagArrayForPossibleErrors[k];
				if (solutionRowErrorCounter != 0)
					printRowDuplications(soduko, solutionRowErrorCounter, i);
			}

			//	going over the cols and check for errors
			for (j = 0; j < 9; j++) {
				int flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
				callCheckCol(soduko, flagArrayForPossibleErrors, j);
				for (k = 0; k < 9; k++)
					solutionColErrorCounter += flagArrayForPossibleErrors[k];
				if (solutionColErrorCounter != 0)
					printColDuplications(soduko, solutionColErrorCounter, j);
			}


			//	going over the subGrids and check for errors
			for (i = 0; subRow < 9; i++) {
				for (j = 0; subCol < 9; j++) {
					int flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
					callCheckSubGrid(soduko, flagArrayForPossibleErrors, subRow, subCol);
					for (k = 0; k < 9; k++) {
						solutionSubGridErrorCounter += flagArrayForPossibleErrors[k];
						if (solutionSubGridErrorCounter != 0)
							printSubGridDuplications(soduko, solutionSubGridErrorCounter, subRow, subCol);
					}
				}
			}
		}

