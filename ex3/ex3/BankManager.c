/*
ex3 - BankManager.c:
- The program recieves the following arguments from the main module: argc, argv
- This program executes all bookkeeping and technical bank operations for existing bank accounts.
- The program uses synchronization mechanisms and avoids Deadlocks in order to executes different threads that run simultaneously even on the same account.
- The program return a BOOL value, which indicated whether the managing operations done successfully, and log file was written.
*/

/* Definitions: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'

/* Libraries: */
#include <stdio.h>
#include "BankManager.h"
#include "accountsListUtils.h"

/* Function Declarations: */
char *readCommandLinebyLine(FILE *CommandFile);
ParsingCommands ParseLineIntoCommand(char *);

int executeBankManager(int argc, char *argv[]) {

	/* Internal Declarations: */
	FILE *CommandFile = NULL, *runTimeLogFile = NULL;
	char *CommandFileName = NULL, *BalanceReportFileName = NULL, *RunTimeLogFileName = NULL, *CommandType = NULL, *LineString = NULL;
	int TotalNumberOfCommands = 0, i = 0, *CommandLengthArray = NULL;
	unsigned long long int AccountNumber = 0;
	long long Amount = 0, CurrentBalance = 0;
	ParsingCommands parsingFields;
	allAccounts newAccountsList;
	logFile newRunTimeLogFile;
	 

	// Start of Program
	CommandFileName = argv[1];
	BalanceReportFileName = argv[2];
	RunTimeLogFileName = argv[3];

	// Verify that the number of command line argument is correct
	if (argc != 4) {
		printf("Number of Command line Arguments isn't compatible,  error %ul\n", GetLastError());
		exit(1);
	}

	// initialize the account list struct
	if (!initializeNewAccountsList(&newAccountsList, &newRunTimeLogFile)){
		printf("accounts initialization failed!\n");
		exit(1);
	}
	else
		printf("accounts initialized successfully\n");

	// open CommandFile by getting CommandFileName as an argument
	CommandFile = fopen(CommandFileName, "r");
	if (CommandFile == NULL) {
		perror("Error: ");
		printf("Could not open CommandFile, error %ul\n", GetLastError());
		exit(1);
	}

	// open RunTime_LogFile by getting RunTimeLogFileName as an argument
	newAccountsList.runtmieLogFile->logFilePtr = fopen(RunTimeLogFileName, "w");
	if (newAccountsList.runtmieLogFile->logFilePtr == NULL) {
		printf("failed to open RunTime_LogFile, error %ul\n", GetLastError()); 
		exit(1);
	}

	// go over 'CommandFile' and read commands line by line until EOF
	do { 
		LineString = readCommandLinebyLine(CommandFile);
		printf("The command is %s\n", LineString);
		parsingFields = ParseLineIntoCommand(LineString);

		switch (parsingFields.commandTypeIndex) {
			case createAccountCmd:
				printf("account number %lli, current balance of %.2f\n", parsingFields.AccountNumber, parsingFields.Amount);
				if (!addNewAccountToList(&newAccountsList, parsingFields.AccountNumber, parsingFields.Amount)) {
					printf("cannot create %lli as a new account to list, error %ul\n", parsingFields.AccountNumber, GetLastError());
				}
				break;

			case closeAccountCmd:
				printf("account number %lli\n", parsingFields.AccountNumber);
				removeAccountFromList(&newAccountsList, parsingFields.AccountNumber);
				break;

			case printBalancesCmd:
				printf("Printing Account Balances if exists\n");
				if (!printCurrentBalances(&newAccountsList)) {
					printf("Account list is empty\n");
				}
				break;

			case depositCmd:
				printf("account number %lli, amount to deposit of %.2f\n", parsingFields.AccountNumber, parsingFields.Amount);
				depositOrWithdrawalAmountToAccount(&newAccountsList, parsingFields.AccountNumber, parsingFields.Amount, parsingFields.commandTypeIndex);
				break;

			case withdrawalCmd:
				printf("account number %lli, amount to withdraw of %.2f\n", parsingFields.AccountNumber, parsingFields.Amount);
				depositOrWithdrawalAmountToAccount(&newAccountsList, parsingFields.AccountNumber, parsingFields.Amount, parsingFields.commandTypeIndex);
				break;
		}

	} while (!feof(CommandFile));

	// writing data accounts into balance report file
	if (!printBalanceReport(&newAccountsList, BalanceReportFileName)) {
		printf("cannot print into balance report file, error %ul\n", GetLastError());
		exit(1);
	}
	
	fprintf(newAccountsList.runtmieLogFile->logFilePtr, "Program successfully finished running. Exiting.");

	free(LineString);
	fclose(CommandFile);
	fclose(newAccountsList.runtmieLogFile->logFilePtr);

	return 0;

}


/* Function Definitions */

char *readCommandLinebyLine(FILE *CommandFile) {

	char *LineString = NULL;
	int buffer = 100;			// '13' for the longest command, '20' for the longest int variable type in c ('10' each)
	LineString = (char *)malloc((buffer) * sizeof(char));
	if (LineString == NULL) {
		printf("LineCommandPtr allocation was failed, error %ul\n", GetLastError());
	}

	if (fgets(LineString, buffer, CommandFile) == NULL) {
		printf("reading a command from CommandFile was failed, error %ul\n", GetLastError());
	}

	if (!feof(CommandFile)) {
		LineString[strlen(LineString) - 1] = '\0';
	}
	return LineString;
}

ParsingCommands ParseLineIntoCommand(char *LineString) {

	ParsingCommands parsingFields = { NULL };
	int i = 0;
	char *commandsArray[] = { "CreateAccount" , "CloseAccount" , "PrintBalances" , "Deposit", "Withdrawal" };

	parsingFields.command = strtok(LineString, " ");
	for (i = 0; i < 5; i++) {
		if (strcmp(parsingFields.command, commandsArray[i]) == 0) {
			parsingFields.commandTypeIndex = i;
		}
	}

	switch (parsingFields.commandTypeIndex) {
		case createAccountCmd:
			parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
			parsingFields.Amount = strtod(strtok(NULL, " "),NULL);
			break;

		case closeAccountCmd:
			parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
			break;

		case printBalancesCmd:
			break;

		case depositCmd:
			parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
			parsingFields.Amount = strtod(strtok(NULL, " "), NULL);
			break;

		case withdrawalCmd:
			parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
			parsingFields.Amount = strtod(strtok(NULL, " "), NULL);
			break;
		}

	return parsingFields;

}

bool printBalanceReport(allAccounts *accountsListPtr, char *BalanceReportFileName) {
	
	FILE *BalanceReportFile = NULL;
	account *currentAccountPtr = NULL;
	printf("Printing final balance report\n");
	
	// open the balance report file:
	BalanceReportFile = fopen(BalanceReportFileName, "w");
	if (BalanceReportFile == NULL) {
	perror("Error: ");
	printf("Could not open balance report file, error %ul\n", GetLastError());
	exit(1);
		
	}
	
	fprintf(BalanceReportFile, "Summary of balances in bank accounts:\n");
	fprintf(BalanceReportFile, "Bank Account #, Current Balance, Initial Balance, Total Deposited, Total Withdrawal, # of Deposits, # of Withdrawals\n");
	if (accountsListPtr->accountListHeadPtr != NULL) {
		for (currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {
			
				fprintf(BalanceReportFile, "%llu,%.2f,%.2f,%.2f,%.2f,%llu,%llu\n",
					currentAccountPtr->accountNumber,
					currentAccountPtr->currentBalance,
					currentAccountPtr->initialBalance,
					currentAccountPtr->totalDepositeSum,
					currentAccountPtr->totalWithdrawalSum,
					currentAccountPtr->ammountOfDeposits,
					currentAccountPtr->ammountOfWithdrawals);
			
		}
		
	}
	
	fclose(BalanceReportFile);
	return true;
	
}

/*
// initialize a new allAccounts.
// open CommandFile 
// read CommandFile line by line:
// while (CommandFileLine != EOF){
//     - parse CommandFileLine into a command
//     - check what command is it (switch-case using the enums)
//	   - execute the relevant command in a new thread:
//     switch(command)
//	      * createAccountCmd:
//             # check that all other threads closed
//             # execute createAccount()
//             # check return value :
//                    success => 
//                    
//	      * closeAccountCmd:
//             # check that all other threads closed
//             # execute closeAccount()
//	      * printBalancesCmd:
//             # check that all other threads closed
//             # execute printBalances()
//	      * depositeCmd:
//             # execute deposite()
//	      * withdrawalCmd:
//             # execute withdrawal()
// }
// printBalanceReport(accountsList, runTimeLogFileName);
// print into log file the final line
// free all handlers and memory allocations

Algorithm flow:

1. Check if argc != 4 
	if yes plot error and exit program

2. Open command file argv[1] for "r" mode.

3. go over the command file and do:
	a. count all commands (lines) to execute. Note the number you reached as TotCommands.
	b. create an array of pointers where each cell holds the ADD of the beggining of a string, the command to executes (Use existing function from ex2) ????
	b. go over the file again and fetch each line's first word.
	c. check the command using switch case condition of the first word.
	d. call the appropriate function to execute the command.


4. The following operations:
	a. CreateAccount commands-
		1) Check whether the bank account already exist.
		2) if yes => print appropriate message and continue to next command.
		3) else => create account in the array:
			a) realloc another cell of struct type
			b) update the size of the array.
			c) fill the relevant field respectively.

	b. CloseAccount commands-
		1) Check whether the bank account exists.
		2) if no => Account isn't exist on DataBase, thus print appropriate message and continue to next command.
		3) else => close account in the array:
			a) delete a cell in the array. 
			b) decrement the size of the array by one.

	c. PrintBalances commands-
			1) Print two automatic messages.
			2) check whether the BankAccountArray is empty.
			3) if yes => continue to next command.
			3) else => 
				a) sort BankAccountArray in ascending order acccording to AccountNumber field.
				b) go over the array and print "BankAccount XXX" & "Current Balance YYY" fields respectively.

	d. Deposit commands-
				1) Check whether the bank account exists in array.
				2) if no => Account isn't exist on DataBase, thus print appropriate message and continue to next command.
				3) else => call to DepositFunction(BankAccountArray, AccountNumber, Amount). The function does:
					a) Match the AccountNmuber argument with its correspond field 
					b) Takes out CurrentBalance field of the matched AccountNumber.
					c) Sums the CurrentBalance with the Amount argument.
					d) Updates the CurrentBalance back into its field.
					e) Inc by one TotDepositNumber field of that Account
					f) Adds the amount the TotDepositAmount field.
				4) print successful message and continue to next command.

	e. Withdrawal commands-
				1) Check whether the bank account exists in array.
				2) if no => Account isn't exist on DataBase, thus print appropriate message and continue to next command.
				3) else => call to WithdrawalFunction(BankAccountArray, AccountNumber, Amount). The function does:
				a) Match the AccountNmuber argument with its correspond field
				b) Takes out CurrentBalance field of the matched AccountNumber.
				c) difference the CurrentBalance with the Amount argument.
				d) Updates the CurrentBalance back into its field.
				e) Inc by one TotWithdrawalNumber field of that Account
				f) Adds the amount the TotWithdrawalAmount field.
				4) print successful message and continue to next command.


5. Output Files:
	a. Runtime_logFile- 
		1) Open Runtime_logFile argv[3] for "w" mode.
		2) Gathers all appropriate messages that were prints during the execution of the program.
		3) Check in an end condition that all commands have executed.
		4) Plot ending message.
		5) Release and close file

	b. Balance Report File-
		1) Open Balance Report File argv[2] for "w" mode at the end of all runnings.
		2) Summs in this file all balances of BankAccounts according to format.
		3) Printed of Bank Accounts should be ploted sorted as well.
		4) Release and close file


Notes:
1. Synchronization mechanisms:
	a. Every time we read a command from Command File, we sould request for MUTEX  for that execution using OpenMutex.
	   If the MUTEX isn't exist, CreatMutex.
	b. Every time a thread wants to executes a command, it should first request for permission to be its ownership using WaitForMultipleObjects.
	c. As long as a command is in Exection section, we cannot allow any other thread to run on the same one, especially for MUTEX on the same BankAccount cell.
	d. Whenever thread has finished executes a command, it must release it using ReleaseMutex, in order to avoid DEADLOCK.
	e. 

2. Create an array of structs for BankAccounts with the following fields:
	a. BankAccountNumber
	b. CurrentBalance.
	c. InitialBalance.
	d. TotDepositNumber.
	e. TotWithdrawalNumber.
	f. TotDepositAmount
	g. TotWithdrawalAmount.

	TODO: Need to check whether Linked list is better to use here then array.
			
*/
