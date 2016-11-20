/*
Common.c:
- Functions that are used by both solver and checker are here
Tomer Shahar 301359410, Lior Bialik 301535316
*/


// functions declerations:
void callCheckRow(char soduko[9][9], int flagArrayForPossibleValues[9], int row);
void callCheckCol(char soduko[9][9], int flagArrayForPossibleValues[9], int col);
void callCheckSubGrid(char soduko[9][9], int flagArrayForPossibleValues[9], int row, int col);


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