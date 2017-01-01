#ifndef ACCOUNT_LIST_UTILS
#define ACCOUNT_LIST_UTILS

/* Libraries: */
#include "BankManager.h"

/* Function Declarations: */
int initializeNewAccount(account *accountPtr, unsigned long long accountNumber, double accountBalance);
int isAccountInList(commandArguments *newCommandArguments);
int addNewAccountToList(commandArguments *newCommandArguments);
int removeAccountFromList(commandArguments *newCommandArguments);
int printCurrentBalances(commandArguments *newCommandArguments);
int printBalanceReport(allAccounts *accountsListPtr, char *BalanceReportFileName);
int depositOrWithdrawalAmountToAccount(commandArguments *newCommandArguments);
void makeDepositing(commandArguments *newCommandArguments, account *currentAccountPtr);
void makeWithdrawal(commandArguments *newCommandArguments, account *currentAccountPtr);

#endif