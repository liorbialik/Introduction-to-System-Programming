/*
ex3 - Main.c:
- the program recieves the following arguments:
@param Command line: The text file that includes all banking operations the program should perform.
@param Balance report file: The output file of the program. This file sums all balances in bank account.
@param RunTime log file: The log file of the program. This file will centralize the program runtime documentation
- The program manages operational and managable bank accounts.
*/

#include "BankManager.h"
#include "operationExecuter.h"


int main(int argc, char *argv[]) {
	int returnValue = 0;
	char *CommandFileName = NULL, *BalanceReportFileName = NULL, *RunTimeLogFileName = NULL;

	// Verify that the number of command line argument is correct
	if (argc != 4) {
		printf("Number of Command line Arguments isn't compatible,  error %ul\n", GetLastError());
		exit(1);
	}

	CommandFileName = argv[1];
	BalanceReportFileName = argv[2];
	RunTimeLogFileName = argv[3];

	returnValue = executeBankManager(CommandFileName, BalanceReportFileName, RunTimeLogFileName);

	return returnValue;
}