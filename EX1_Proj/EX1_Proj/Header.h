//This is the header file for main.c and includes function declarations of solver.c

//15.11 20:30
// Solver's part

#include <stdio.h>
#include <stdlib.h>


//This function checks feasability of a value for the correspond row
//by passing on the relevant row values and modify flagArrayForPossibleValues[existValue] = '0';
//gets an index row and modifies flagArrayForPossibleValues
void callCheckRow(char soduko[9][9], int array[9], int row)
{
	int j;
	for (j = 0; j < 9; j++)
		if (soduko[row][j] != '.') array[soduko[row][j] - 1] = 0;
}

//This function checks feasability of a value for the correspond col
//by passing on the relevant col values and modify flagArrayForPossibleValues[existValue] = '0';
//gets an index col and modified flagArrayForPossibleValues that deters assignment
void callCheckCol(char soduko[9][9], int array[9], int col)
{
	int i;
	for (i = 0; i < 9; i++)
		if (soduko[i][col] != '.') array[soduko[i][col] - 1] = 0;
}

//This function checks feasability of a value for the correspond subGrid
//by passing on the relevant subGrid values and modify flagArray[existValue] = '0';
//gets an index subGrid and modified flagArray that deters assignment
void callCheckSubGrid(char soduko[9][9], int array[9], int row, int col)
{
	int rowSubgrid = (row / 3) * 3;
	int colSubGrid = (col / 3) * 3;
	int i, j;
	for (i = rowSubgrid; i < rowSubgrid + 3; i++)
	{
		for (j = colSubGrid; j < colSubGrid + 3; j++)
			if (soduko[i][j] != '.')
				array[soduko[i][j] - 1] = 0;
	}
}

//This function gets a cell in soduko and return feasability of assignment;
// whether it has specific num => will return its value
// ow => will return '0';
char checkPossibleNumForCell(char soduko[9][9], int row, int col)
{
	int possibleValueCounter = 0, k;
	char returnValue = '0';
	//	allocate a 9cell array initiated by 'ones':
	int flagArrayForPossibleValues[9] = { 1,1,1,1,1,1,1,1,1 };

	callCheckRow(soduko, flagArrayForPossibleValues, row);
	callCheckCol(soduko, flagArrayForPossibleValues, col);
	callCheckSubGrid(soduko, flagArrayForPossibleValues, row, col);

	//	count all 'one' values at flagArrayForPossibleValues[9];
	//	if cnt == 1 => find the value and return its index; ow => return 0;
	for (k = 0; k < 9; k++)
	{
		possibleValueCounter += flagArrayForPossibleValues[k];
		if (flagArrayForPossibleValues[k] == 1)
			returnValue = k + 1;
	}
	if (possibleValueCounter == 1) 
		return returnValue;
	else return '0';
}

//	This funcion checks the status of the soduko martix; 
//	if soduko[9][9] is still not full => will return 0, ow => will return 1;
int sodukoStatus(char soduko[9][9])
{
	int i, j;
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
		{
			if (soduko[i][j] == '.') return 0;
			else continue;
		}
	}
	// if you got here, it means that soduko matrix is full with numbers, i.e solved
	return 1;
}

// TODO: need to add to solver a parameter for outputFileInstance
//	CallSolver recieves a 2D matrix soduko and solves it if possible, ow => will output an appropriate impossible message
void callSolver(char soduko[9][9])
{
	int solved = 0;
	char cellValue = '0';
	int assigningWasMadeFlag;
	int i, j;
	//char* solutionPtr = NULL;
	do
	{
		//	going over the matrix from top-left side and start filling it with numbers if possible
		for (i = 0; i < 9; i++)
		{
			for (j = 0; j < 9; j++)
			{
				assigningWasMadeFlag = 0;
				if (soduko[i][j] != '.') {
					continue;
				} 

				else
				{
					cellValue = checkPossibleNumForCell(soduko, i, j); //TODO: add assignment variable
					if (cellValue != '0'){
						//Make an assignment for that cell
						soduko[i][j] = cellValue;
						assigningWasMadeFlag = 1;
					}
					else continue;
				}
			}
		}
		if (solved = assigningWasMadeFlag && sodukoStatus(soduko)) break;
	} while (assigningWasMadeFlag);


	//	one step before returning to caller; here we will deter whether the soduko was solved or not;
	switch (solved)
	{
		case 0:
		{
		printf("Sudoku puzzle is too hard for me to solve\n");

		//solutionPtr = "Sudoku puzzle is too hard for me to solve\n";
		break;
		}

		case 1:
		{
		for (i = 0; i < 9; i++){
			for (j = 0; j < 9; j++){ 
				printf("%c ", soduko[i][j]);
				}
			printf("\n");
			}
		//solutionPtr = soduko[9][9];
		break;
		}

	
	return;
	}

}

