//15.11 20:30
// Checker's part

#include <stdio.h>
#include <stdlib.h>
#include "Checker.h"
#include <ctype.h>
#include "Common.h"


//This function ellaborates row errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and rowDuplicationArray which points on the revealed errors;
// and prints the error messages
void printRowDuplications(char soduko[9][9], int numOfDuplications, int row) {
	int i, j, counter = 0;
	char *errorMessage = NULL;

	do {
		for (i = 0; i < 9; i++) {
			for (j = i + 1; j < 9; j++) {
				if (soduko[row][i] == soduko[row][j]) {
					printf("Line error : digit %c appears at(%d, %d) and (%d, %d)\n", soduko[row][i], row + 1, i + 1, row + 1, j + 1);
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

	char *solution = 0;
	return solution;
}

//Checker
// for every cel

char* callChecker(FILE *fileToWriteInto, char soduko){
	int i, j; 
	int subRow = 3 * i, subCol = 3 * j;
		int solutionErrorFlag = 0; //indicated the number of error duplications in a particular template

		do {
			//	going over the rows and check for errors
			for (i = 0; i < 9; i++) {
				int *flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
				callCheckRow(soduko, flagArrayForPossibleErrors, i);
				for (k = 0; k < 9; k++)
					solutionErrorFlag += *flagArrayForPossibleErrors[k];
				if (solutionErrorFlag != 0)
					printRowDuplications(soduko, solutionErrorFlag, i);
			}

			//	going over the cols and check for errors
			for (j = 0; j < 9; j++) {
				int *flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
				callCheckCol(soduko, flagArrayForPossibleErrors, j);
				for (k = 0; k < 9; k++)
					solutionErrorFlag += *flagArrayForPossibleErrors[k];
				if (solutionErrorFlag != 0)
					printColDuplications(soduko, solutionErrorFlag, j);
			}


			//	going over the subGrids and check for errors
			for (i = 0; subRow < 9; i++) {
				for (j = 0; subCol < 9; j++) {
					int *flagArrayForPossibleErrors[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
					callCheckSubGrid(soduko, flagArrayForPossibleErrors, subRow, subCol);
					for (k = 0; k < 9; k++) {
						solutionErrorFlag += *flagArrayForPossibleErrors[k];
						if (solutionErrorFlag != 0)
							printSubGridDuplications(soduko, solutionErrorFlag, subRow, subCol);
					}
				}
			}
		}

