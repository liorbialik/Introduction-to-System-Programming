#pragma once
/*
ex3 - Header.h:
- The header file for BankManager.c which contains the execution for all commands in Command file.
- This file contains the definitions, used structs and the decleration which used to call the main
function in BankManager.c
*/

#ifndef BankManager_H
#define BankManager_H

///* Constants: */


/* Libraries: */
#include <Windows.h>


///* Struct Declarations: */
// struct for a single account
typedef struct account {
	unsigned long long accountNumber;
	double initialBalance;
	double currentBalance;
	double totalDepositeSum;
	double totalWithdrawalSum;
	unsigned long long ammountOfDeposits;
	unsigned long long ammountOfWithdrawals;
	// another field for the account's mutex
	struct account *nextInList;
}account;


// struct that will be using the the runtimeLogFile
typedef struct logFile {
	FILE *logFilePtr;
	// another field for the logFile's mutex
}logFile;


// struct for all accounts
typedef struct allAccounts {
	account *accountListHeadPtr;
	unsigned long long totalNumberOfAccounts;
	logFile *runtmieLogFile;
} allAccounts;


// struct for parsing the command line
typedef struct ParsingCommands {
	char *command;
	unsigned long long AccountNumber;
	double Amount;
	int commandTypeIndex;
} ParsingCommands;


/* Function Declarations: */
int executeBankManager(int argc, char *argv[]);


// enumerations for possible commands:
enum inputCommands { createAccountCmd, closeAccountCmd, printBalancesCmd, depositCmd, withdrawalCmd };

#endif#pragma once
#pragma once