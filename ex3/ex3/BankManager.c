/*
ex3 - BankManager.c:
- The program recieves the following arguments from the main module: argc, argv
- This program executes all bookkeeping and technical bank operations for existing bank accounts.
- The program uses synchronization mechanisms and avoids Deadlocks in order to executes different threads that run simultaneously even on the same account.
- The program return a BOOL value, which indicated whether the managing operations done successfully, and log file was written.
*/

/* Libraries: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <tchar.h>
#include <Strsafe.h>
#include <conio.h>
#include <process.h>
#include <sys/stat.h>
#include "BankManager.h"
#include "accountsListUtils.h"
#include <sys/types.h>
#include <direct.h>


/* Function Declarations: */
int CountNumOfCommands(FILE *CommandFile);
int *CountLengthOfEachCommand(FILE *CommandFile, int TotalNumberOfCommands);
char *readLine(FILE *file, int CommandLength);
char *readCommandLinebyLine(FILE *CommandFile);
struct Parsing {
		char *command;
		unsigned long long AccountNumber;
		float Amount;
		int commandTypePosition;
};
struct Parsing ParseLineIntoCommand(char *);


int executeBankManager(int argc, char *argv[]) {

	/* Internal Declarations: */
	FILE *CommandFile = NULL, *RunTime_LogFile = NULL; 
	char *CommandFileName = NULL, *BalanceReportFileName = NULL, *RunTime_LogFileName = NULL, *CommandType = NULL, *LineString = NULL;
	int TotalNumberOfCommands = 0, i = 0, *CommandLengthArray = NULL;
	unsigned long long int AccountNumber = 0;
	long long Amount = 0, CurrentBalance = 0;
	struct Parsing parsingFields;

	// Start of Program
	CommandFileName = argv[1];
	BalanceReportFileName = argv[2];
	RunTime_LogFileName = argv[3];

	// Verify that the number of command line argument is correct
	if (argc != 4) {
		printf("Number of Command line Arguments isn't compatible,  error %ul\n", GetLastError());
		exit(1);
	}

	// open CommandFile by getting CommandFileName as an argument
	CommandFile = fopen(CommandFileName, "r");
	if (CommandFile == NULL) {
		perror("Error: ");
		printf("Could not open CommandFile, error %ul\n", GetLastError());
		exit(1);
	}

	// open RunTime_LogFile by getting RunTimeLogFileName as an argument
	RunTime_LogFile = fopen(RunTime_LogFileName, "w");
	if (RunTime_LogFile == NULL) {
		printf("failed to open RunTime_LogFile, error %ul\n", GetLastError()); 
		exit(1);
	}

	// go over 'CommandFile', count all commands & assign CommandLengthArray
	TotalNumberOfCommands = CountNumOfCommands(CommandFile);
	CommandLengthArray = CountLengthOfEachCommand(CommandFile, TotalNumberOfCommands);

	// go over 'CommandFile' and read commands line by line
	//for (i = 0; i < TotalNumberOfCommands; i++) {
		//LineString = readLine(CommandFile, CommandLengthArray[i]);

	do { 
		LineString = readCommandLinebyLine(CommandFile);
		printf("The command is %s\n", LineString);
		parsingFields = ParseLineIntoCommand(LineString);

		switch (parsingFields.commandTypePosition) {
		case createAccountCmd:
			printf("%lli %.2f\n", parsingFields.AccountNumber, parsingFields.Amount);
			//createNewAccount(parameters);
			//TEST:
			break;

		case closeAccountCmd:
			printf("%lli\n", parsingFields.AccountNumber);
			//closeExistedAccount(AccountNumber);
			//TEST:
			break;

		case printBalancesCmd:
			printf("Printing Account Balances if exists\n");
			//if (printCurrentBalancesInBank() == 0) {
			//	printf("cannot print current Balances in Bank, error %ul\n", GetLastError());
			//}
			break;

		case depositeCmd:
			printf("%lli %.2f\n", parsingFields.AccountNumber, parsingFields.Amount);
			//depositAmountToAccount(AccountNumber, Amount);
			//TEST:
			break;

		case withdrawalCmd:
			printf("%lli %.2f\n", parsingFields.AccountNumber, parsingFields.Amount);
			//withdrawalAmountFromAccount(AccountNumber, Amount);
			//TEST:
			break;
		}

	} while (feof(CommandFile) == 0);
		
	free(LineString);
	fclose(CommandFile);
	fclose(RunTime_LogFile);


	return 0;

}

////test
//for (i = 0; i < TotalNumberOfCommands; i++) {
//	printf("%s\n", LineCommandPtrArray[i]);
//}
//getchar();

/* Function Definitions */

char *readLine(FILE *file, int CommandLength) {
	//This function reads lines only by giving each string's length
	char *LineCommandPtr = NULL;

	LineCommandPtr = (char *)malloc((CommandLength) * sizeof(char));
	if (LineCommandPtr == NULL) {
		printf("LineCommandPtr allocation was failed, error %ul\n", GetLastError());
	}

	if (fgets(LineCommandPtr, CommandLength+1, file) == NULL) {
		printf("reading a command from CommandFile was failed, error %ul\n", GetLastError());
	}

	fgetc(file);
	return LineCommandPtr;

}		

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

	LineString[strlen(LineString) - 1] = '\0';
	return LineString;
}

int CountNumOfCommands(FILE *CommandFile) {
	// fgetc() is inspired by http://stackoverflow.com/questions/12733105/c-function-that-counts-lines-in-file
	int ch;
	int TotalNumberOfCommands = 0;
	while (!feof(CommandFile)) {
		ch = fgetc(CommandFile);
		if ((ch == '\n') || (ch == EOF)) {
			TotalNumberOfCommands++;
		}
	}
	// Resetting pointer to the start of file
	rewind(CommandFile);

	return TotalNumberOfCommands;
}

int *CountLengthOfEachCommand(FILE *CommandFile, int TotalNumberOfCommands) {
	int *CommandLengthArray = NULL, i=0;
	CommandLengthArray = (int *)malloc(TotalNumberOfCommands * sizeof(int));
	if (CommandLengthArray == NULL) {
		printf("CommandLengthArray allocation was failed, error %ul\n", GetLastError());
	}
	for (i = 0; i < TotalNumberOfCommands; i++) {
		CommandLengthArray[i] = 0;			// initialize CommandLengthArray
	}
	// go over CommandLengthArray and save each command's length
	for (i = 0; i<TotalNumberOfCommands - 1; i++) {
		while (fgetc(CommandFile) != '\n') {
			CommandLengthArray[i]++;
		}
	}
	if (i == TotalNumberOfCommands - 1) {
		while (fgetc(CommandFile) != EOF) {
			CommandLengthArray[i]++;
		}
	}
	// Resetting pointer to the start of file
	rewind(CommandFile);

	return CommandLengthArray;
}

struct Parsing ParseLineIntoCommand(char *LineString) {

	struct Parsing parsingFields = { NULL };
	int i = 0;
	char *commandsArray[] = { "CreateAccount" , "CloseAccount" , "PrintBalances" , "Deposit", "Withdrawal" };

	parsingFields.command = strtok(LineString, " ");
	for (i = 0; i < 5; i++) {
		if (strcmp(parsingFields.command, commandsArray[i]) == 0) {
			parsingFields.commandTypePosition = i;
		}
	}

	switch (parsingFields.commandTypePosition) {
	case createAccountCmd:
		parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
		parsingFields.Amount = strtof(strtok(NULL, " "),NULL);
		break;

	case closeAccountCmd:
		parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
		break;

	case printBalancesCmd:
		break;

	case depositeCmd:
		parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
		parsingFields.Amount = strtof(strtok(NULL, " "), NULL);
		break;

	case withdrawalCmd:
		parsingFields.AccountNumber = strtol(strtok(NULL, " "), NULL, 0);
		parsingFields.Amount = strtof(strtok(NULL, " "), NULL);
		break;
	}

	return parsingFields;

}


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

/*
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
