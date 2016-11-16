#include <stdio.h>
#include <stdlib.h>
#include "Solver.h"
#include "Checker.h"
//#include "Common.h"
#include <ctype.h>


// Platform main() for checking the functions !!!!!!!!!!!!!!!!!!!!!!!!!
int main()
{
	int i, j;
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
		{
			printf("%c ", soduko[i][j]);
			printf("\n");
		}
	}

	printf("\n -- sudoku after -- \n");
	callSolver(soduko);
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 9; j++)
			printf("%c ", soduko[i][j]);
		printf("\n");
	}

	return 0;
}



