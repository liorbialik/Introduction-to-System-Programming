/*
Solver.c:
- The program recieves a soduko char matrix as an input
- For each cell in the matrix, the possible numbers are checked by row, column and sub-grid.
- If a cell has a single possible number that fits, this number becomes the cell's new value. 
- During a run on the whole matrix, if there was a value assignment, the matrix will be reviewed again in search for another cell to solve.
- If no cell has been changed during a run, the solution could not be reached by the algorithm and the appropriate message is written.
- if all cells are solved, it will return the solution.
Tomer Shahar 301359410, Lior Bialik 301535316
*/

#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"

// functions declerations:
char checkPossibleNumForCell(char soduko[9][9], int row, int col);
void callCheckRow(char soduko[9][9], int flagArrayForPossibleValues[9], int row);
void callCheckCol(char soduko[9][9], int flagArrayForPossibleValues[9], int col);
void callCheckSubGrid(char soduko[9][9], int flagArrayForPossibleValues[9], int row, int col);
int sodukoStatus(char soduko[9][9]);


// TODO: need to add to solver a parameter for outputFileInstance
void callSolver(FILE *fileToWriteInto, char soduko[9][9])
{/*
	@ Description: recieves a 2D matrix soduko and solves it if possible. if possible return the solved matrix, otherwise it will output an appropriate impossible message.
	@ Param soduko: The matrix containing the numbers and missing cells.
	@ Return: None
	*/
	int solved = 0, sodukoStatusFlag = 0;
	char cellValue = '0';
	int i, j;
	int assigningWasMadeFlag;
	//char* solutionPtr = NULL;
	do{
		assigningWasMadeFlag = 0;
		//	going over the matrix from top-left side and start filling it with numbers if possible
		for (i = 0; i < 9; i++) {
			for (j = 0; j < 9; j++) {
				if (soduko[i][j] != '.') {
					continue;
				}

				else {
					cellValue = checkPossibleNumForCell(soduko, i, j); //TODO: add assignment variable
					if (cellValue != '0') {
						//Make an assignment for that cell
						soduko[i][j] = cellValue;
						assigningWasMadeFlag = 1;
					}
					else
						continue;
				}
			}
		}
		sodukoStatusFlag = sodukoStatus(soduko);
		if (solved = assigningWasMadeFlag && sodukoStatusFlag)
			break;

	} while (assigningWasMadeFlag);

	//	one step before returning to caller; here we will deter whether the soduko was solved or not;
	switch (solved) {
		case 0: {
			fputs("Sudoku puzzle is too hard for me to solve\n", fileToWriteInto);
			soduko[0][0] = '0'; // this will be used in the main module for checking
			return;
		}

		case 1: {
			return;
		}
	}
}


void callCheckRow(char soduko[9][9], int flagArrayForPossibleValues[9], int row)
{/*
 @ Description: This function checks feasability of a value for the correspond row by passing on the relevant row values and modify a flag array accordingly
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Param flagArrayForPossibleValues: array that each index corresponds to a possible value for the given cell
 @ Return: None
 */
	int j;
	for (j = 0; j < 9; j++) {
		if (soduko[row][j] != '.') {
			flagArrayForPossibleValues[(soduko[row][j] - 1) - '0'] = 0;
		}
	}
}

void callCheckCol(char soduko[9][9], int flagArrayForPossibleValues[9], int col)
{/*
 @ Description: This function checks feasability of a value for the correspond column by passing on the relevant row values and modify a flag array accordingly
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Param flagArrayForPossibleValues: array that each index corresponds to a possible value for the given cell
 @ Return: None
 */
	int i;
	for (i = 0; i < 9; i++) {
		if (soduko[i][col] != '.') {
			flagArrayForPossibleValues[(soduko[i][col] - 1) - '0'] = 0;
		}
	}
}

void callCheckSubGrid(char soduko[9][9], int flagArrayForPossibleValues[9], int row, int col)
{/*
 @ Description: This function checks feasability of a value for the correspond sub-grid by passing on the relevant row values and modify a flag array accordingly
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Param flagArrayForPossibleValues: array that each index corresponds to a possible value for the given cell
 @ Return: None
 */
	int rowSubgrid = (row / 3) * 3;
	int colSubGrid = (col / 3) * 3;
	int i, j;
	for (i = rowSubgrid; i < rowSubgrid + 3; i++) {
		for (j = colSubGrid; j < colSubGrid + 3; j++) {
			if (soduko[i][j] != '.') {
				flagArrayForPossibleValues[(soduko[i][j] - 1) - '0'] = 0;
			}
		}
	}
}

char checkPossibleNumForCell(char soduko[9][9], int row, int col)
{/*
 @ Description: This function checks feasability of a value for a single cell. whether it has specific num => will return its value, else it will return '0';
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Param row: The row of the given cell in the matrix.
 @ Param col: The column of the given cell in the matrix.
 @ Return: None
 */
	int possibleValueCounter = 0, k;
	char returnValue = '0';
	//	allocate a 9cell array initiated by 'ones':
	int flagArrayForPossibleValues[9] = { 1,1,1,1,1,1,1,1,1 };

	callCheckRow(soduko, flagArrayForPossibleValues, row);
	callCheckCol(soduko, flagArrayForPossibleValues, col);
	callCheckSubGrid(soduko, flagArrayForPossibleValues, row, col);

	//	count all 'one' values at flagArrayForPossibleValues[9];
	//	if cnt == 1 => find the value and return its index; ow => return 0;
	for (k = 0; k < 9; k++) {
		possibleValueCounter += flagArrayForPossibleValues[k];
		if (flagArrayForPossibleValues[k] == 1) {
			returnValue = (k + 1) + '0';
		}
	}
	if (possibleValueCounter == 1) {
		return returnValue;
	}
	else
		return '0';
}

int sodukoStatus(char soduko[9][9])
{/*
 @ Description: This funcion checks the status of the soduko martix - whether it is full or not.
 @ Param soduko: The matrix containing the numbers and missing cells.
 @ Return: if the martix is still not full it will return 0. if the martix is full it will return 1
 */
	int i, j, dotCounter = 0;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (soduko[i][j] == '.') {
				dotCounter++;
			}
		}
	}
	if (dotCounter == 0) { //it means that soduko matrix is full with numbers, i.e solved
		return 1;
	}
	else
		return 0;
}
