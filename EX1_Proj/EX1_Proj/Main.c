#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

extern int errno;

FILE* getInputFileData(char* inputFilName) {}
//{
//	FILE *file;
//	file = fopen(inputFilName, "r");
//	if (file == NULL)
//	{
//		fprintf(stderr, "Value of errno: %d\n", errno);
//		fprintf(stderr, "Error opening the file: %s\n", strerror(errno));
//	}
//	return file;
//}

//int writeDataToOutputFile(char* outputData){}
//
//char* callSolver(char *data)
//{
//	char *solution = 0;
//
//	return solution;
//}
//
//char* callChecker(char *data)
//{
//	char *solution = 0;
//
//	return solution;
//}



int main(int argc, char *argv[]){
	
	FILE *inputfile;
	char *runMode, *inputFileName;
	char* data[11][11] = {0}; // TODO: need to add a #define ROWS/COLS in .h file

	// TODO: check if the arguments are givent at run time or needed inside the program
	runMode = argv[1]; 
	inputFileName = argv[2];
	// open the file from argv[1]
	inputfile = getInputFileData(inputFileName);
	// parse file to manegable format (without the seperations?, create structures?)
	// check running mode (argv[0]) and call the relevant function (0=>solver, 1=>checker)
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