//#include <stdio.h>
//#include <stdlib.h>
//#include "Main.h"
//
//
//// Solver's part
//
//
////	passing on the relevant row values and modify flagArray[existValue] = '0';
//void callCheckRow(char* soduko[], int array[], int row)
//{
//	for (int j = 0; j < 9; j++)
//		if (soduko[row][j] != '.') array[soduko[row][j] - 1] = 0;
//}
//
//
////	passing on the relevant col values and modify flagArray[existValue] = '0';
//void callCheckCol(char* soduko[], int array[], int col)
//{
//	for (int i = 0; i < 9; i++)
//		if (soduko[i][col] != '.') array[soduko[i][col] - 1] = 0;
//}
//
//
////	passing on the relevant subGrid values and modify flagArray[existValue] = '0';
//void callCheckSubGrid(char* soduko[], int array[], int row, int col)
//{
//	int rowSubgrid = (row / 3) * 3;
//	int colSubGrid = (col / 3) * 3;
//	int i, j;
//	for (i = rowSubgrid; i < rowSubgrid + 3; i++)
//	{
//		for (j = colSubGrid; j < colSubGrid + 3; j++)
//			if (soduko[i][j] != '.') array[soduko[i][j] - 1] = 0;
//	}
//}
//
//
//int checkPossibleNumForCell(char* soduko, int row, int col)
//{
//	int cnt = 0, k;
//	char returnValue;
//	void callCheckRow(char*, int, int);
//	void callCheckRow(char*, int, int);
//	void callCheckSubGrid(char*, int, int, int);
//
//
//	//	allocate a 9cell array initiated by 'ones':
//	int flagArray[9] = { 1,1,1,1,1,1,1,1,1 };
//	callCheckRow(soduko, flagArray, row);
//	callCheckCol(soduko, flagArray, col);
//	callCheckSubGrid(soduko, flagArray, row, col);
//
//
//	//	count all 'one' values at flagArray[];
//	//	if cnt == 1 => find the value and return its index; ow => return 0;
//	for (k = 0; k < 9; k++)
//	{
//		cnt += flagArray[k];
//		if (flagArray[k] == 1) returnValue = k + 1;
//	}
//	if (cnt == 1) return returnValue;
//	else return 0;
//}
//
//
////	This funcion checks the status of the soduko martix; if soduko[9][9] is still not full => will return 0, ow => will return 1;
//int sodukoStatus(char* soduko[])
//{
//	int i, j;
//	for (i = 0; i < 9; i++)
//	{
//		for (j = 0; j < 9; j++)
//		{
//			if (soduko[i][j] == '.') return 0;
//			else continue;
//		}
//	}
//	return 1;
//}
//
//
////	This function solves the soduko if possible, ow => will output an appropriate impossible message
//char* callSolver(char soduko)
//{
//	char* solverOutput; int assigningWasMade = 0;
//	int sodukoStatus(char); int solved = 0;
//	do
//	{
//		//	going over the matrix from top-left side and start filling it
//		int i, j; checkPossibleNumForCell;
//		for (i = 0; i < 9; i++)
//		{
//			for (j = 0; j < 9; j++)
//			{
//				if (soduko[i][j] != '.') continue;
//				else
//				{
//					//				check possibilities for that cell
//					checkPossibleNumForCell(soduko, i, j);
//
//
//					//				check if cell has deterministic value; 
//					//				if yes => assign soduko[][] = returnValue; ow => continue;
//					if (checkPossibleNumForCell != 0)
//					{
//						soduko[i][j] = checkPossibleNumForCell;
//						assigningWasMade = 1;
//					}
//					else continue;
//				}
//			}
//		}
//		if (solved = assigningWasMade && sodukoStatus(soduko)) break;
//	} while (assigningWasMade);
//
//
//	//	one step before returning to caller; here we will deter whether the soduko was solved or not;
//	switch (solved = assigningWasMade && sodukoStatus(soduko))
//	{
//	case 0:
//	{
//		printf("Sudoku	puzzle	is	too	hard	for	me	to	solve\n");
//		break;
//	}
//	case 1:
//		return soduko[9][9];
//	}
//	return 0;
//}
//
//
//
//
////	char *solution = 0;
////
////	return solution;
////}
//
//
//
//
////Checker's part
//
//
//int sumRowChecker(char *soduko, int i)
//{
//	int sum = 0, j;
//	for (j = 0; j<9; j++)
//		sum += soduko[i][j];
//	if (sum != 45) return 0;
//	else return 1;
//}
//
//
//int sumColChecker(char *soduko, int j)
//{
//	int sum = 0, i;
//	for (i = 0; i<9; i++)
//		sum += soduko[i][j];
//	if (sum != 45) return 0;
//	else return 1;
//}
//
//
//int sumSubGridChecker(char *soduko, int i, int j)
//{
//	int sum = 0, row, col;
//	for (row = i; row < i + 3; row++)
//	{
//		for (col = j; col < j + 3; col++)
//			sum += soduko[i][j];
//		if (sum != 45) return 0;
//		else return 1;
//	}
//}
//
//
//void printRowDuplications(char *soduko, int* rowDuplicationArray)
//{
//	int k, i, j;
//	for (k = 0; k < 9; k++)
//	{
//		if (!rowDuplicationArray[k])
//		{
//			for (i = 0; i < 9; i++)
//			{
//				for (j = i + 1; j < 9; j++)
//				{
//					if (soduko[k][i] == soduko[k][j])
//					{
//						printf("Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[k][i], k, i, k, j);
//						// how do we make double break; cause we just printed duplication and it gurantees us it's only once in a row for the same digit
//						// break
//					}
//				}
//
//
//			}
//		}
//	}
//}
//
//
//void printColDuplications(char *soduko, int* colDuplicationArray)
//{
//	int k, i, j;
//	for (k = 0; k < 9; k++)
//	{
//		if (!colDuplicationArray[k])
//		{
//			for (i = 0; i < 9; i++)
//			{
//				for (j = i + 1; j < 9; j++)
//				{
//					if (soduko[i][k] == soduko[j][k])
//					{
//						printf("Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[i][k], i, k, j, k);
//						// how do we make double break; cause we just printed duplication and it gurantees us it's only once in a row for the same digit
//						// break
//					}
//				}
//
//
//			}
//		}
//	}
//}
//
//
////not finished yet
//void printSubGridDuplications(char *soduko, int* subGridDuplicationArray)
//{
//	int k, i, j; //translate k (subGrid's num) into correspond coordinate => row,col
//	for (k = 0; k < 9; k++)
//	{
//		if (!subGridDuplicationArray[k])
//		{
//			for (i = row; i < row + 3; i++)
//			{
//				for (j = i + 1; j < 9; j++)
//				{
//					if (soduko[k][i] == soduko[k][j])
//					{
//						printf("Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[k][i], k, i, k, j);
//						// how do we make double break; cause we just printed duplication and it gurantees us it's only once in a row for the same digit
//						// break
//					}
//				}
//
//
//			}
//		}
//	}
//}
//
//
//char* callChecker(char soduko[9][9])
//{
//	int i, j, k;
//	int rowGrid = 3 * i;
//	int colGrid = 3 * j;
//	int sumRowChecker(char, int); void printRowDuplications(char, int[]);
//	int sumColChecker(char, int); void printColDuplications(char, int[]);
//	int sumSubGridChecker(char, int, int); void printSubGridDuplications(char, int[]);
//	int isValid = sumRowChecker && sumColChecker && sumSubGridChecker;
//	int rowDuplicationArray[9] = { 1 }, colDuplicationArray[9] = { 1 }, subGridDuplicationArray[9] = { 1 };
//
//
//	//	go first through all special templates individually and add the value contents
//	//	totals of each sum should equal 45 b / c (1 + 2 + 3 + 4... = 45).If not template must contain duplication; This way we can identify duplication quickly;
//
//
//	//	check row's sum indication
//	for (i = 0; i < 9; i++)
//		rowDuplicationArray[i] = sumRowChecker(soduko, i);
//
//
//	//	check col's sum indication
//	for (j = 0; j < 9; j++)
//		colDuplicationArray[j] = sumColChecker(soduko, j);
//
//
//	//	check subGrid's sum indication
//	i = 0;
//	for (rowGrid = 0; rowGrid < 9; i++)
//	{
//		j = 0;
//		for (colGrid = 0; colGrid < 9; j++)
//			// don't know yet how to interpret (row,col) coordinate into subGrid's number where the beggining is from top left
//			subGridDuplicationArray[k] = sumSubGridChecker(soduko, rowGrid, colGrid);
//	}
//
//
//	//	check if there was any error messages by now using 'isValid' function;
//	//	if yes => they were already been printed
//	//	ow => print success
//	if (isValid)
//	{
//		printf("No errors found in given Sudoku puzzle\n");
//		return 0;
//	}
//	else
//	{
//		printf("Found errors in given Sudoku puzzle.\nThe errors are:\n");
//
//
//		if (!sumRowChecker)
//			printRowDuplications(soduko, rowDuplicationArray);
//
//
//		if (!sumColChecker)
//			printColDuplications(soduko, colDuplicationArray);
//
//
//		if (!sumSubGridChecker)
//			printSubGridDuplications(soduko, subGridDuplicationArray);
//	}
//}
//
//
////	char *solution = 0;
////
////	return solution;
////}
//
//
////After modifing the input and reviewing the soduko, needs to write the outputData to file again =>
////int writeDataToOutputFile(char* outputData){}
//
//
//
//
//
