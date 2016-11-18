/*This is the header file for common.c
Tomer Shahar 301359410, Lior Bialik 301535316
*/

#ifndef __COMMON_H
#define __COMMON_H

void callCheckRow(char soduko[9][9], int flagArrayForPossibleValues[9], int row);
void callCheckCol(char soduko[9][9], int flagArrayForPossibleValues[9], int col);
void callCheckSubGrid(char soduko[9][9], int flagArrayForPossibleValues[9], int row, int col);

#endif
#pragma once
