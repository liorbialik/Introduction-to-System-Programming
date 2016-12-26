/*
ex3 - Main.c:
- the program recieves the following arguments:
@param Command line: The text file that includes all banking operations the program should perform.
@param Balance report file: The output file of the program. This file sums all balances in bank account.
@param RunTime log file: The log file of the program. This file will centralize the program runtime documentation
- The program manages operational and managable bank accounts.
*/

#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'

/* Libraries: */

#include <stdio.h>
#include "BankManager.h"
#include "accountsListUtils.h"

int main(int argc, char *argv[]) {

	//FILE *runTimeLogFile = NULL;
	//char *runTimeLogFileName = NULL;
	logFile newRunTimeLogFile;
	allAccounts newAccountsList;
	//account duumyAccount;
	//duumyAccount.accountNumber = 123;
	//duumyAccount.initialBalance = 0;
	//duumyAccount.currentBalance = 0;
	//duumyAccount.totalDepositeSum = 0;
	//duumyAccount.totalWithdrawalSum = 0;
	//duumyAccount.ammountOfDeposits = 0;
	//duumyAccount.ammountOfWithdrawals = 0;
	//// another field for the account's mutex
	//duumyAccount.nextInList = NULL;

	unsigned long int dummyAccountNumber = 123;
	double dummyAccountBalance = 999999.99;

	if (!initializeNewAccountsList(&newAccountsList, &newRunTimeLogFile))
		printf("accounts initialization failed!\n");
	else
		printf("accounts initialized successfully\n");

	//runTimeLogFileName = argv[1];
	//// open RunTime_LogFile by getting RunTimeLogFileName as an argument
	//runTimeLogFile = fopen(runTimeLogFileName, "w");
	//if (runTimeLogFile == NULL) {
	//	printf("failed to open RunTime_LogFile, error %ul\n", GetLastError());
	//	exit(1);
	//}
	// newAccountsList.accountListHeadPtr = &duumyAccount;

	if (addNewAccountToList(&newAccountsList, dummyAccountNumber, dummyAccountBalance))
		printf("Successfully executed addNewAccountToList");
	else
		printf("Failed to executed addNewAccountToList");


	getchar();

	return 0;

}