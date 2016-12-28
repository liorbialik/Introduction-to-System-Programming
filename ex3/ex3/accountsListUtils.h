#ifndef ACCOUNT_LIST_UTILS
#define ACCOUNT_LIST_UTILS

/* Libraries: */
#include "BankManager.h"
#include <stdbool.h>

/* Function Declarations: */
bool initializeNewAccount(account *accountPtr, unsigned long long accountNumber, double accountBalance);
bool isAccountInList(commandArguments *newCommandArguments);
int addNewAccountToList(commandArguments *newCommandArguments);
int removeAccountFromList(commandArguments *newCommandArguments);
int printCurrentBalances(commandArguments *newCommandArguments);
bool printBalanceReport(allAccounts *accountsListPtr, char *BalanceReportFileName);
int depositOrWithdrawalAmountToAccount(commandArguments *newCommandArguments);
void makeDepositing(currentAccountPtr);
void makeWithdrawal(currentAccountPtr);

#endif