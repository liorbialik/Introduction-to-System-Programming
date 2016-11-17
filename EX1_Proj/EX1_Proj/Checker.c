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

// functions decelerations:
void callCheckRow(char soduko[9][9], int flagArrayForPossibleErrors[9], int row);
void callCheckCol(char soduko[9][9], int flagArrayForPossibleErrors[9], int col);
void callCheckSubGrid(char soduko[9][9], int flagArrayForPossibleErrors[9], int row, int col);
void printRowDuplications(char soduko[9][9], int numOfDuplications, int row);
void printColDuplications(char soduko[9][9], int numOfDuplications, int col);
void printSubGridDuplications(char soduko[9][9], int numOfDuplications, int row, int col);
void callChecker(char soduko[9][9]);


int main()
{
	char soduko[9][9] = {
		{ '2','7','3','9','1','5','6','4','8' },
		{'9','4','6','2','8','3','5','7','1' },
		{'1', '8', '5', '6', '4', '7', '9', '3', '2'},
		{	'7', '6', '9', '1', '3', '8', '2', '5', '4'},
		{'8','5','2','7','6','4','1','9','3'},
		{'3','1','4','5','2','9', '8', '6', '7'},
		{'4' ,'2', '7' ,'8' ,'5', '6' , '3' ,'1', '9'},
		{'5' ,'3', '8', '4', '9' ,'1' , '7', '1' ,'6'},
		{'6' ,'9', '1' , '3', '7', '2', '4', '8' ,'5' },

	};

	callChecker(soduko);

	return 0;
}


//Checker

void callChecker(char soduko[9][9])
{
	int i=0, j=0, k;
	int solutionRowErrorCounter = 0;
	int solutionColErrorCounter = 0;
	int solutionSubGridErrorCounter = 0;	//indicated the number of error duplications in a particular template

			//	going over the rows and check for errors
			for (i = 0; i < 9; i++) {
				int flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
				callCheckRow(soduko, flagArrayForPossibleErrors, i);
				for (k = 0; k < 9; k++) {
					solutionRowErrorCounter += flagArrayForPossibleErrors[k];
				}
				if (solutionRowErrorCounter != 0)
					printRowDuplications(soduko, solutionRowErrorCounter, i);
			}

			//	going over the cols and check for errors
			for (j = 0; j < 9; j++) {
				int flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
				callCheckCol(soduko, flagArrayForPossibleErrors, j);
				for (k = 0; k < 9; k++) {
					solutionColErrorCounter += flagArrayForPossibleErrors[k];
				}
				if (solutionColErrorCounter != 0)
					printColDuplications(soduko, solutionColErrorCounter, j);
			}


			//	going over the subGrids and check for errors
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 3; j++) {
					int subRow = 3 * i, subCol = 3 * j;
					int flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
					callCheckSubGrid(soduko, flagArrayForPossibleErrors, subRow, subCol);
					for (k = 0; k < 9; k++) {
						solutionSubGridErrorCounter += flagArrayForPossibleErrors[k];
					}
						if (solutionSubGridErrorCounter != 0)
							printSubGridDuplications(soduko, solutionSubGridErrorCounter, subRow, subCol);
				}
			}
}

void callCheckRow(char soduko[9][9], int flagArrayForPossibleErrors[9], int row)
{/*
 @ Description: This function checks feasability of a value for the correspond row by passing on the relevant row values and modify a flag array accordingly
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Param flagArrayForPossibleValues: array that each index corresponds to a possible value for the given cell
 @ Return: None
 */
	int j;
	for (j = 0; j < 9; j++) {
		if (soduko[row][j] != '.') {
			flagArrayForPossibleErrors[(soduko[row][j] - 1) - '0'] = 0;
		}
	}
}

void callCheckCol(char soduko[9][9], int flagArrayForPossibleErrors[9], int col)
{/*
 @ Description: This function checks feasability of a value for the correspond column by passing on the relevant row values and modify a flag array accordingly
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Param flagArrayForPossibleValues: array that each index corresponds to a possible value for the given cell
 @ Return: None
 */
	int i;
	for (i = 0; i < 9; i++) {
		if (soduko[i][col] != '.') {
			flagArrayForPossibleErrors[(soduko[i][col] - 1) - '0'] = 0;
		}
	}
}

void callCheckSubGrid(char soduko[9][9], int flagArrayForPossibleErrors[9], int row, int col)
{/*
 @ Description: This function checks feasability of a value for the correspond sub-grid by passing on the relevant row values and modify a flag array accordingly
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Param flagArrayForPossibleValues: array that each index corresponds to a possible value for the given cell
 @ Return: None
 */
	int rowSubgrid = (row / 3) * 3;
	int colSubGrid = (col / 3) * 3;
	for (row = rowSubgrid; row < rowSubgrid + 3; row++) {
		for (col = colSubGrid; col < colSubGrid + 3; col++) {
			if (soduko[row][col] != '.') {
				flagArrayForPossibleErrors[(soduko[row][col] - 1) - '0'] = 0;
			}
		}
	}
}

void printRowDuplications(char soduko[9][9], int numOfDuplications, int row)
{/*
 This function ellaborates row errors that were indicated at the given Soduko;
 It gets as a parameter the soduko matrix and rowDuplicationArray which points on the revealed errors;
 and prints the error messages
 */
	int i, j;
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (soduko[row][i] == soduko[row][j]) {
					printf("Line error : digit %c appears at(%d,%d) and (%d,%d)\n", soduko[row][i], row + 1, i + 1, row + 1, j + 1);
					//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
				}
			}
		}
}


//This function ellaborates col errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and colDuplicationArray which points on the revealed errors;
// and prints the error messages
void printColDuplications(char soduko[9][9], int numOfDuplications, int col) {
	int i, j;
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (soduko[i][col] == soduko[j][col]) {
					printf("Column error : digit %c appears at(%d,%d) and (%d,%d)\n", soduko[i][col], i + 1, col + 1, j + 1, col + 1);
					//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
				}
			}
		}
}


//This function ellaborates subGrid errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and subGridDuplicationMatrix which points on the revealed errors;
// and prints the error messages
void printSubGridDuplications(char soduko[9][9], int numOfDuplications, int row, int col) {
	int i, j, k;
	char subGridArray[9] = { '0' };
	int rowIndexArray[9] = { row,row,row,row + 1,row + 1,row + 1,row + 2,row + 2,row + 2 };
	int colIndexArray[9] = { col,col + 1,col + 2,col,col + 1,col + 2,col,col + 1,col + 2 };

		//Convert subGrid to subGridArray
		for (i = row, k=0; i < row + 3; i++, k++) {
			for (j = col; j < col + 3; j++) {
				subGridArray[k] = soduko[i][j];
			}
		}

		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (subGridArray[i] == subGridArray[j]) {
					printf("SubGrid error : digit %c appears at(%d,%d) and (%d,%d)\n", subGridArray[i], rowIndexArray[i] + 1, colIndexArray[i] + 1, rowIndexArray[j] + 1, colIndexArray[j] + 1);
					getchar();
				}
				//asprintf(&errorMessage, "Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[row][i], row+1, i+1, row+1, j+1);
			}

		}
}