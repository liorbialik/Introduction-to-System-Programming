//14.11 13:30

#include <stdio.h>
#include <stdlib.h>
#include "Main.h"

// Solver's part

//This function checks feasability of a value for the correspond row
//by passing on the relevant row values and modify flagArrayForPossibleValues[existValue] = '0';
//gets an index row and modifies flagArrayForPossibleValues
void callCheckRow(char soduko[9][9], int array[9], int row)
{
	for (int j = 0; j < 9; j++)
		if (soduko[row][j] != '.') array[soduko[row][j] - 1] = 0;
}

//This function checks feasability of a value for the correspond col
//by passing on the relevant col values and modify flagArrayForPossibleValues[existValue] = '0';
//gets an index col and modified flagArrayForPossibleValues that deters assignment
void callCheckCol(char soduko[9][9], int array[9], int col)
{
	for (int i = 0; i < 9; i++)
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
// ow => will return 0;
int checkPossibleNumForCell(char soduko[9][9], int row, int col)
{
	int cnt = 0, k;
	int returnValue = 0; //TODO: convert it to char
						 //	allocate a 9cell array initiated by 'ones':
	int* flagArrayForPossibleValues[9] = { 1,1,1,1,1,1,1,1,1 };

	callCheckRow(soduko, flagArrayForPossibleValues, row);
	callCheckCol(soduko, flagArrayForPossibleValues, col);
	callCheckSubGrid(soduko, flagArrayForPossibleValues, row, col);

	//	count all 'one' values at flagArrayForPossibleValues[9];
	//	if cnt == 1 => find the value and return its index; ow => return 0;
	for (k = 0; k < 9; k++)
	{
		cnt += *flagArrayForPossibleValues[k];
		if (flagArrayForPossibleValues[k] == 1)
			returnValue = k + 1;
	}
	if (cnt == 1) return returnValue;
	else return 0;
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
char* callSolver(char soduko)
{
	int sodukoStatus(char); int solved = 0;
	int assigningWasMadeFlag;
	int i, j;
	int checkPossibleNumForCell(char, int, int);
	do
	{
		//	going over the matrix from top-left side and start filling it with numbers if possible
		for (i = 0; i < 9; i++)
		{
			for (j = 0; j < 9; j++)
			{
				assigningWasMadeFlag = 0;
				if (soduko[i][j] != '.') continue;
				else
				{
					checkPossibleNumForCell(soduko, i, j); //TODO: add assignment variable 
					if (checkPossibleNumForCell != 0)
					{
						//Make an assignment for that cell
						soduko[i][j] = checkPossibleNumForCell;
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
		printf("Sudoku puzzle is too hard for me to solve\n"); // TODO: return a char *soultionPtr in each case
		break;
	}
	case 1:
		return soduko[9][9];
	}
	return 0;
}


//	char *solution = 0;
//
//	return solution;
//}


//Checker's part

//This function gets a pointer for a row, by going over the row calc its values and return its sum;
int sumRowChecker(char *soduko, int i)
{
	int sum = 0, j;
	for (j = 0; j<9; j++)
		sum += soduko[i][j];
	if (sum != 45) return 0;
	else return 1;
}

//This function gets a pointer for a col, by going over the col calc its values and return its sum;
int sumColChecker(char *soduko, int j)
{
	int sum = 0, i;
	for (i = 0; i<9; i++)
		sum += soduko[i][j];
	if (sum != 45) return 0;
	else return 1;
}

//This function gets a pointer for a subGrid, by going over the subGrid calc its values and return its sum;
int sumSubGridChecker(char *soduko, int i, int j)
{
	int sum = 0, row, col;
	for (row = i; row < i + 3; row++)
	{
		for (col = j; col < j + 3; col++)
			sum += soduko[i][j];
		if (sum != 45) return 0;
		else return 1;
	}
}

//This function ellaborates row errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and rowDuplicationArray which points on the revealed errors;
// and prints the error messages
void printRowDuplications(char *soduko, int* rowDuplicationArray)
{
	int k, i, j;
	for (k = 0; k < 9; k++)
	{
		if (!rowDuplicationArray[k])
		{
			for (i = 0; i < 9; i++)
			{
				for (j = i + 1; j < 9; j++)
				{
					if (soduko[k][i] == soduko[k][j])
					{
						printf("Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[k][i], k, i, k, j); // TODO: add this line into a char array + add +1 tp coordinate on print
																												   // how do we make double break; cause we just printed duplication and it gurantees us it's only once in a row for the same digit
																												   // break
					}
				}

			}
		}
	}
}

//This function ellaborates col errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and colDuplicationArray which points on the revealed errors;
// and prints the error messages
void printColDuplications(char *soduko, int* colDuplicationArray)
{
	int k, i, j;
	for (k = 0; k < 9; k++)
	{
		if (!colDuplicationArray[k])
		{
			for (i = 0; i < 9; i++)
			{
				for (j = i + 1; j < 9; j++)
				{
					if (soduko[i][k] == soduko[j][k])
					{
						printf("Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[i][k], i, k, j, k);
						// how do we make double break; cause we just printed duplication and it gurantees us it's only once in a row for the same digit
						// break
					}
				}

			}
		}
	}
}

//This function ellaborates subGrid errors that were indicated at the given Soduko;
//It gets as a parameter the soduko matrix and subGridDuplicationMatrix which points on the revealed errors;
// and prints the error messages
void printSubGridDuplications(char *soduko, int* subGridDuplicationMatrix)
{
	int row, col, i, j; //translate k (subGrid's num) into correspond coordinate => row,col
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (!subGridDuplicationMatrix[i][j])
			{
				//Thought about unparsing the subGrid matrix into array buffer and then
				//call printRowDuplications function ???
				row = 3 * i; col = 3 * j;
				for (i = row; i < row + 3; i++)
				{
					for (j = col + 1; j < 9; j++)
					{
						if (soduko[k][i] == soduko[k][j])
						{
							printf("Line error: digit %c appears at (%d,%d) and (%d,%d)\n", soduko[k][i], k, i, k, j);
							// how do we make double break; cause we just printed duplication and it gurantees us it's only once in a row for the same digit
							// break
						}
					}

				}
			}
		}
	}

	//This function checks the correctness of a given soduko[9][9];
	// it gets a given soduko and if it's correct => returns that there are no errors
	//ow => will output the errors
	char* callChecker(char soduko[9][9])
	{
		int i, j, k;
		int rowGrid = 3 * i;
		int colGrid = 3 * j;
		int sumRowFlag = 1, sumColFlag = 1, sumSubGridFlag = 1;
		int sumRowChecker(char, int); void printRowDuplications(char, int[]);
		int sumColChecker(char, int); void printColDuplications(char, int[]);
		int sumSubGridChecker(char, int, int); void printSubGridDuplications(char, int[]);
		int isValid = sumRowFlag && sumColFlag && sumSubGridFlag;
		int rowDuplicationArray[9] = { 1 }, colDuplicationArray[9] = { 1 },
			int subGridDuplicationMatrix[3][3] = { { 1,1,1 },{ 1,1,1 },{ 1,1,1 } };


		//	go first through all special templates individually and add the value contents
		//	totals of each sum should equal 45 b / c (1 + 2 + 3 + 4... = 45).
		//  If not template must contain duplication; This way we can identify duplication quickly;

		//	check row's sum indication
		for (i = 0; i < 9; i++)
		{
			rowDuplicationArray[i] = sumRowChecker(soduko, i);
			sumRowFlag = sumRowFlag && rowDuplicationArray[i];
		}

		//	check col's sum indication
		for (j = 0; j < 9; j++)
		{
			colDuplicationArray[j] = sumColChecker(soduko, j);
			sumColFlag = sumColFlag && colDuplicationArray[j];
		}

		//	check subGrid's sum indication
		for (i = 0; rowGrid < 9; i++)
		{
			for (j = 0; colGrid < 9; j++)
			{
				subGridDuplicationMatrix[i][j] = sumSubGridChecker(soduko, rowGrid, colGrid);
				sumSubGridFlag = sumSubGridFlag && subGridDuplicationMatrix[i][j];
			}
		}

		// check if all soduko is valid; if yes print no error message;
		// ow => call error templates functions by order and print the errors;
		if (isValid)
		{
			printf("No errors found in given Sudoku puzzle\n");
			return 0;
		}
		else
		{
			printf("Found errors in given Sudoku puzzle.\nThe errors are:\n");

			if (!sumRowFlag)
				printRowDuplications(soduko, rowDuplicationArray);

			if (!sumColFlag)
				printColDuplications(soduko, colDuplicationArray);

			if (!sumSubGridFlag)
				printSubGridDuplications(soduko, subGridDuplicationArray);
		}
	}

	//	char *solution = 0;
	//
	//	return solution;
	//}

	//Checker alternative
	// for every cel

	char* altchecker(char soduko) {
		int i, j, solutionErrorFlag = 0;
		int* flagArrayForPossibleValues[9] = { 1,1,1,1,1,1,1,1,1 };
		int checkPossibleNumForCell(char, int, int);
		do
		{
			//	going over the matrix from top-left side and start checking for errors
			for (i = 0; i < 9; i++)
			{
				for (j = 0; j < 9; j++)
				{
					callCheckRow(soduko, flagArrayForPossibleValues, i);
					callCheckCol(soduko, flagArrayForPossibleValues, j);
					callCheckSubGrid(soduko, flagArrayForPossibleValues, i, j);

				}
			}
			if (solved = solutionErrorFlag && sodukoStatus(soduko)) break;
		} while (solutionErrorFlag == 0);
	}


	//After modifing the input and reviewing the soduko, needs to write the outputData to file again =>
	//int writeDataToOutputFile(char* outputData){}


	// Platform main() for checking the functions !!!!!!!!!!!!!!!!!!!!!!!!!!

	int flag = 0;               /* 1 - Error, 0 - Success */

								//int printNotValid(char *array, int type, int errorLocation);
								//int check(char *array);
								//void printUsage();


	int main()
	{
		int i, j;
		char* callSolver(char);
		char soduko[9][9] = {
			{ '.','.','1','7','.','6','3','.','.' },
			{ '.','.','3','4','.','2','8','.','.' },
			{ '2','.','.','.','.','.','.','4','.' },
			{ '.','9','.','1','.','3','.','4','.' },
			{ '1','.','.','.','8','.','.','.','3' },
			{ '.','4','.','6','.','7','.','9','.' },
			{ '8','.','.','.','.','.','.','.','.' },
			{ '.','.','6','2','.','1','4','.','.', },
			{ '.','.','9','8','.','.','2','.','.' }
		};

		/* Printing the matrix */

		printf("\n -- sudoku before -- \n");
		for (i = 0; i < 9; i++)
		{
			for (j = 0; j < 9; j++)
				printf("%c ", soduko[i][j]);
			printf("\n");
		}

		printf("\n -- sudoku after -- \n");
		callSolver(soduko);
		for (i = 0; i < 9; i++)
		{
			for (j = 0; j < 9; j++)
				printf("%c ", soduko[i][j]);
			printf("\n");
		}

	}

	// Lior's Part !!!!!!!!!!!!!!!!!!!!!!!!!!

	int main(int argc, char *argv[]) {

		FILE *infile;
		char *runMode, *inputFileName;
		char* data[11][11] = { 0 }; // TODO: need to add a #define ROWS/COLS in .h file

									//Not sure if you convert the txt into string at that point ??
									//Communal to both modes
									//char* getInputFileData(char* inputFilName)
									//{
									//	use the function f.gets to open the txt file and convert it to string
									//	return buffer of chars filled with input data
									//}

									// TODO: check if the arguments are givent at run time or needed inside the program
		runMode = argv[1];
		inputFileName = argv[2];
		// open the file from argv[1]
		// parse file to manegable format (without the seperations?, create structures?)
		// check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)

		//before calling the functions we must parse the buffer into a 9X9 matrix named soduko[][]
		//this will save us to do it twice inside each function

		//switch (runMode)
		//{
		//case 0:
		//	callSolver(data);
		//	break;
		//case 1:
		//	callChecker(data);
		//	break;
		//}
		// reparse the data into the solutions style
		// write the reparsed-date into the output file (argv[2])
		getchar();
		return 0;
	}



