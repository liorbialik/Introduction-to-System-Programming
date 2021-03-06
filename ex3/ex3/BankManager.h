#pragma once
/*
ex3 - BankManager.h:
- The header file for BankManager.c which contains the execution for all commands in Command file.
- This file contains the definitions, used structs and the decleration which used to call the main
function in BankManager.c
*/

#ifndef BankManager_H
#define BankManager_H

/* Libraries: */
#include <Windows.h>
#include <stdio.h>


/* Struct Declarations: */

// struct for a single account
typedef struct account {
	unsigned long long accountNumber;
	double initialBalance;
	double currentBalance;
	double totalDepositeSum;
	double totalWithdrawalSum;
	unsigned long long ammountOfDeposits;
	unsigned long long ammountOfWithdrawals;
	HANDLE accountMutex;
	struct account *nextInList;
} account;

// struct that will be using the the runtimeLogFile
typedef struct logFile {
	FILE *logFilePtr;
	HANDLE logFiltMutex;
} logFile;


// struct for all accounts
typedef struct allAccounts {
	account *accountListHeadPtr;
	unsigned long long totalNumberOfAccounts;
	logFile *runtmieLogFile;
} allAccounts;


// struct for the commands execution arguments
typedef struct commandArguments {
	allAccounts *accountsListPtr;
	unsigned long long accountNumber;
	double amountOfMoney;
	int commandTypeIndex;
	const char *addNewAccountToListThreadCreationError;
	const char *removeAccountFromListThreadCreationError;
	const char *printCurrentBalancesThreadCreationError;
	const char *depositOrWithdrawalAmountToAccountThreadCreationError;

} commandArguments;

/* Function Declarations: */
int executeBankManager(char *CommandFileName, char *BalanceReportFileName, char *RunTimeLogFileName);


// enumerations for possible commands:
enum inputCommands { createAccountCmd, closeAccountCmd, printBalancesCmd, depositCmd, withdrawalCmd };

#endif#pragma once
#pragma once