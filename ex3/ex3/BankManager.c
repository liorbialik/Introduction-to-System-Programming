/*
ex3 - BankManager.c:
- The program recieves the following arguments from the main module: argc, argv
- This program executes all bookkeeping and technical bank operations for existing bank accounts.
- The program uses synchronization mechanisms and avoids Deadlocks in order to executes different threads that run simultaneously even on the same account.
- The program return a BOOL value, which indicated whether the managing operations done successfully, and log file was written.
*/

/* Definitions: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#define MAX_THREAD_HANDLE_ARRAY 64

/* Libraries: */
#include <stdio.h>
#include "BankManager.h"
#include "OperationExecuter.h"

/* Function Declarations: */
char *readCommandLinebyLine(FILE *CommandFile);
int executeCommands(FILE *CommandFile, commandArguments *newCommandArguments);
int parseLineIntoCommandArguments(commandArguments *newCommandArguments, char *LineString);
int initializeStructs(commandArguments *newCommandArguments, allAccounts *newAccountsListPtr, logFile *runtmieLogFilePtr);
int initializeNewAccountsList(allAccounts *accountsListPtr, logFile *runtmieLogFilePtr);
int initializeCommandArguments(commandArguments *newCommandArguments, allAccounts *newAccountsListPtr);
int initializeRuntmieLogFile(logFile *runtmieLogFilePtr);
void executeAcountManagementOperations(commandArguments *newCommandArguments, HANDLE *threadHandleArray, int NumOfHandles);
void executeBankingOperations(commandArguments *newCommandArguments, HANDLE *threadHandleArray, int NumOfHandles);
void addThreadHandleToThreadHandleArray(HANDLE ThreadHandle, HANDLE *threadHandleArray, int NumOfThreads);
HANDLE CreateThreadSimple(
	LPTHREAD_START_ROUTINE StartAddress,
	LPVOID ParameterPtr,
	LPDWORD ThreadIdPtr);


int executeBankManager(char *CommandFileName, char *BalanceReportFileName, char *RunTimeLogFileName) {

	/* Internal Declarations: */
	FILE *CommandFile = NULL, *runTimeLogFile = NULL;
	allAccounts newAccountsList;
	logFile newRunTimeLogFile;
	commandArguments newCommandArguments;

	// initialize all struct
	if (initializeStructs(&newCommandArguments, &newAccountsList, &newRunTimeLogFile)) {
		printf("Structs initialization failed!\n");
		exit(1);
	}
	else
		printf("Structs initialized successfully\n");

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

	// go over 'CommandFile' and execute commands line by line until EOF
	executeCommands(CommandFile, &newCommandArguments);

	// writing data accounts into balance report file
	if (!printBalanceReport(&newAccountsList, BalanceReportFileName)) {
		printf("cannot print into balance report file, error %ul\n", GetLastError());
		exit(1);
	}

	fprintf(newAccountsList.runtmieLogFile->logFilePtr, "Program successfully finished running. Exiting.");

	fclose(CommandFile);
	fclose(newAccountsList.runtmieLogFile->logFilePtr);
	CloseHandle(newAccountsList.runtmieLogFile->logFiltMutex);
	return 0;

}

/* Function Definitions */

int executeCommands(FILE *CommandFile, commandArguments *newCommandArguments) {
	char *LineString = NULL;
	DWORD waitCode = 0, exitCode = 0;
	int i = 0, j = 0, NumOfHandles = 0;

	do {
		LineString = readCommandLinebyLine(CommandFile);
		if (LineString == "eof")
			break;
		printf("The command is %s\n", LineString);
		parseLineIntoCommandArguments(newCommandArguments, LineString);


		if (newCommandArguments->commandTypeIndex >= 0 && newCommandArguments->commandTypeIndex <= 2) {
			//current command is Account Management Opertaions type
			executeAcountManagementOperations(newCommandArguments, threadHandleArray, NumOfHandles);
			NumOfHandles = 0;
		}

		else {
			//current command is Banking Opertaions type
			executeBankingOperations(newCommandArguments, threadHandleArray, NumOfHandles);
			NumOfHandles++;
		}

		free(LineString);
	} while (!feof(CommandFile));

	// check all other threads are closed if there any existing threads
	if (NumOfHandles != 0) {
		waitCode = WaitForMultipleObjects(NumOfHandles, threadHandleArray, TRUE, INFINITE);
		// Safely close all threads and print their exit code:
		for (i = 0; i < NumOfHandles; i++) {
			if (!GetExitCodeThread(threadHandleArray[i], &exitCode)) {
				printf("Problem getting exit code for thread number %d\n", i);
				threadHandleArray[i] = 0;
			}
			else {
				printf("Thread number %d returned exit code %d\n", i, exitCode);
				if (!CloseHandle(threadHandleArray[i])) {
					printf("closing handle failed, error %ul\n", GetLastError());
				}
				threadHandleArray[i] = 0;
			}
		}
	}

	// free dynamic allocations
	free(threadHandleArray);

	return 0;
}

int initializeStructs(commandArguments *newCommandArguments, allAccounts *newAccountsListPtr, logFile *runtmieLogFilePtr) {

	// initialize the runtime log file struct
	if (initializeRuntmieLogFile(runtmieLogFilePtr)) {
		printf("Run time log file initialization failed!\n");
		return 1;
	}
	else
		printf("Run time log file initialized successfully\n");

	// initialize the account list struct
	if (initializeNewAccountsList(newAccountsListPtr, runtmieLogFilePtr)) {
		printf("accounts initialization failed!\n");
		return 1;
	}
	else
		printf("accounts initialized successfully\n");

	// initialize the command arguments struct
	if (initializeCommandArguments(newCommandArguments, newAccountsListPtr)) {
		printf("Command arguments initialization failed!\n");
		return 1;
	}
	else
		printf("Command arguments initialized successfully\n");

	return 0;
}

int initializeRuntmieLogFile(logFile *runtmieLogFilePtr) {
	runtmieLogFilePtr->logFilePtr = NULL;
	runtmieLogFilePtr->logFiltMutex = CreateMutex(NULL, FALSE, NULL);
	if (runtmieLogFilePtr->logFiltMutex == NULL)
	{
		printf("CreateMutex for log file error: %d\n", GetLastError());
		exit(1);
	}
	return 0;
}

int initializeNewAccountsList(allAccounts *accountsListPtr, logFile *runtmieLogFilePtr) {

	printf("Initializing new allAccounts instance\n");
	accountsListPtr->accountListHeadPtr = NULL;
	accountsListPtr->totalNumberOfAccounts = 0;
	accountsListPtr->runtmieLogFile = runtmieLogFilePtr;
	return 0;
}

int initializeCommandArguments(commandArguments *newCommandArguments, allAccounts *newAccountsListPtr) {

	printf("Initializing new CommandArguments instance\n");
	newCommandArguments->accountsListPtr = newAccountsListPtr;
	newCommandArguments->accountNumber = 0;
	newCommandArguments->amountOfMoney = 0;
	newCommandArguments->addNewAccountToListThreadCreationError = NULL;
	newCommandArguments->removeAccountFromListThreadCreationError = NULL;
	newCommandArguments->printCurrentBalancesThreadCreationError = NULL;
	newCommandArguments->depositOrWithdrawalAmountToAccountThreadCreationError = NULL;

	return 0;
}

char *readCommandLinebyLine(FILE *CommandFile) {

	char *LineString = NULL;
	int buffer = 100;								// '13' for the longest command, '20' for the longest int variable type in c ('10' each)
	LineString = (char *)malloc((buffer) * sizeof(char));
	if (LineString == NULL) {
		printf("LineCommandPtr allocation was failed, error %ul\n", GetLastError());
	}

	if (fgets(LineString, buffer, CommandFile) == NULL) {
		printf("cannot read a command from CommandFile, end of file. error %ul\n", GetLastError());
		return "eof";
	}

	if (!feof(CommandFile)) {
		LineString[strlen(LineString) - 1] = '\0';
	}
	return LineString;
}

int parseLineIntoCommandArguments(commandArguments *newCommandArguments, char *LineString) {

	int i = 0;
	char *currentCommand = NULL, *commandsArray[] = { "CreateAccount" , "CloseAccount" , "PrintBalances" , "Deposit", "Withdrawal" };

	currentCommand = strtok(LineString, " ");
	for (i = 0; i < 5; i++) {
		if (strcmp(currentCommand, commandsArray[i]) == 0) {
			newCommandArguments->commandTypeIndex = i;
			break;
		}
	}

	switch (newCommandArguments->commandTypeIndex) {
	case createAccountCmd:
		newCommandArguments->accountNumber = strtol(strtok(NULL, " "), NULL, 0);
		newCommandArguments->amountOfMoney = strtod(strtok(NULL, " "), NULL);
		break;

	case closeAccountCmd:
		newCommandArguments->accountNumber = strtol(strtok(NULL, " "), NULL, 0);
		break;

	case printBalancesCmd:
		break;

	case depositCmd:
		newCommandArguments->accountNumber = strtol(strtok(NULL, " "), NULL, 0);
		newCommandArguments->amountOfMoney = strtod(strtok(NULL, " "), NULL);
		break;

	case withdrawalCmd:
		newCommandArguments->accountNumber = strtol(strtok(NULL, " "), NULL, 0);
		newCommandArguments->amountOfMoney = strtod(strtok(NULL, " "), NULL);
		break;
	}

	return 0;

}

int printBalanceReport(allAccounts *accountsListPtr, char *BalanceReportFileName) {

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
	return TRUE;

}

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE StartAddress, LPVOID ParameterPtr, LPDWORD ThreadIdPtr)
{
	/*
	@ Description: This function creates a thread by calling Win32 Api's CreateThread()
	function, and setting some of the parameters to default value.
	@ Param StartAddress: a pointer to the function that will be run by the thread
	@ Param ParameterPtr: a pointer to the parameter that will be supplied to the
	function run by the thread
	@ Param ThreadIdPtr: return argument: a pointer to a DWORD variable into which
	the function will write the created thread's ID.
	@ Return: The thread handler
	*/
	return CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		StartAddress,    /*  thread function */
		ParameterPtr,    /*  argument to thread function */
		0,               /*  use default creation flags */
		ThreadIdPtr);    /*  returns the thread identifier */
}

void executeAcountManagementOperations(commandArguments *newCommandArguments, HANDLE *threadHandleArray, int NumOfHandles) {

	HANDLE ThreadHandle = 0;
	LPDWORD threadID = 0;
	DWORD waitCode = 0, exitCode = 0;
	int i = 0;
	// possible thread error strings:		
	const char *addNewAccountToListThreadCreationError = "Problem creating 'add New Account To List' thread\n";
	const char *removeAccountFromListThreadCreationError = "Problem creating 'remove Account From List' thread\n";
	const char *printCurrentBalancesThreadCreationError = "Problem creating 'print Current Balances' thread\n";

	// check all other threads are closed if there any existing threads
	if (NumOfHandles != 0) {
		waitCode = WaitForMultipleObjects(NumOfHandles, threadHandleArray, TRUE, INFINITE);
		// Safely close all threads and print their exit code:
		for (i = 0; i < NumOfHandles; i++) {
			if (!GetExitCodeThread(threadHandleArray[i], &exitCode)) {
				printf("Problem getting exit code for thread number %d\n", i);
				threadHandleArray[i] = 0;
			}
			else {
				printf("Thread number %d returned exit code %d\n", i, exitCode);
				if (!CloseHandle(threadHandleArray[i])) {
					printf("closing handle failed, error %ul\n", GetLastError());
				}
				threadHandleArray[i] = 0;
			}
		}
	}

	//threadHandleArray = (HANDLE*)realloc(threadHandleArray, 0 * sizeof(HANDLE));
	//threadHandleArray = NULL;
	free(threadHandleArray);
	threadHandleArray = NULL;


	switch (newCommandArguments->commandTypeIndex) {
	case createAccountCmd:
		ThreadHandle = CreateThreadSimple(
			(LPTHREAD_START_ROUTINE)addNewAccountToList,              /*  thread function */
			newCommandArguments,                                       /*  argument to thread function */
			threadID);                                        /*  returns the thread identifier */
		if (ThreadHandle == NULL) {
			printf("Problem creating 'add New Account To List' thread\n");
			printf("cannot create %lli as a new account to list, error %ul\n", newCommandArguments->accountNumber, GetLastError());
			newCommandArguments->addNewAccountToListThreadCreationError = addNewAccountToListThreadCreationError;
		}
		break;

	case closeAccountCmd:
		ThreadHandle = CreateThreadSimple(
			(LPTHREAD_START_ROUTINE)removeAccountFromList,              /*  thread function */
			newCommandArguments,                                       /*  argument to thread function */
			threadID);                                        /*  returns the thread identifier */
		if (ThreadHandle == NULL) {
			printf("Problem creating 'remove Account From List' thread\n");
			printf("failed to remove account number %lli from list, error %ul\n", newCommandArguments->accountNumber, GetLastError());
			newCommandArguments->removeAccountFromListThreadCreationError = removeAccountFromListThreadCreationError;
		}
		break;

	case printBalancesCmd:
		ThreadHandle = CreateThreadSimple(
			(LPTHREAD_START_ROUTINE)printCurrentBalances,              /*  thread function */
			newCommandArguments,                                       /*  argument to thread function */
			threadID);												  /*  returns the thread identifier */
		if (ThreadHandle == NULL) {
			printf("Problem creating 'print Current Balances' thread\n");
			printf("Failed printing balance, error %ul\n", GetLastError());
			newCommandArguments->printCurrentBalancesThreadCreationError = printCurrentBalancesThreadCreationError;
		}
	}

	// wait until the thread finishes
	WaitForSingleObject(ThreadHandle, INFINITE);

	// Close handle
	if (!CloseHandle(ThreadHandle)) {
		printf("closing handle failed, error %ul\n", GetLastError());
	}

}

void executeBankingOperations(commandArguments *newCommandArguments, HANDLE *threadHandleArray, int NumOfHandles) {

	HANDLE ThreadHandle = 0;
	LPDWORD threadID = 0;
	const char *depositOrWithdrawalAmountToAccountThreadCreationError = "Problem creating 'deposit Or Withdrawal Amount To Account' thread\n";

	Sleep(10);
	// add a new thread to the threads list 
	ThreadHandle = CreateThreadSimple(
		(LPTHREAD_START_ROUTINE)depositOrWithdrawalAmountToAccount,              /*  thread function */
		newCommandArguments,												     /*  argument to thread function */
		threadID);														       /*  returns the thread identifier */
	if (ThreadHandle == NULL) {
		printf("Problem creating 'deposit Or Withdrawal Amount To Account' thread\n");
		printf("Failed to deposite or withdrawal to account number %lli , error %ul\n", newCommandArguments->accountNumber, GetLastError());
		newCommandArguments->depositOrWithdrawalAmountToAccountThreadCreationError = depositOrWithdrawalAmountToAccountThreadCreationError;
	}
	//add threadHandle to ThreadHandleArray
	addThreadHandleToThreadHandleArray(ThreadHandle, threadHandleArray, NumOfHandles);

	// wait until the thread finishes
	//WaitForSingleObject(ThreadHandle, INFINITE);		//TODO: Need to remove line and resolve

}

void addThreadHandleToThreadHandleArray(HANDLE ThreadHandle, HANDLE *threadHandleArray, int NumOfThreads) {

	int i = 0;

	//realloc threadHandleArray by one more size
	HANDLE *tempHandleArray = NULL;
	tempHandleArray = (HANDLE *)malloc((NumOfThreads + 1) * sizeof(HANDLE));
	if (tempHandleArray == NULL) {
		printf("tempHandleArray allocation was failed, error %ul\n", GetLastError());
	}
	// copying threadHandleArray's handles into tempHandleArray
	for (i = 0; i<NumOfThreads; i++) {
		tempHandleArray[i] = threadHandleArray[i];
	}
	// add current ThreadHandle into tempHandleArray's reallocated cell
	tempHandleArray[NumOfThreads] = ThreadHandle;

	threadHandleArray = tempHandleArray;

}
