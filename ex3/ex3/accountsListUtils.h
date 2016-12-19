#ifndef ACCOUNT_LIST_UTILS
#define ACCOUNT_LIST_UTILS

/* Libraries: */
#include "BankManager.h"
#include <stdbool.h>

/* Function Declarations: */
bool addNewAccountToList(allAccounts *accountsListPtr, unsigned long int newAccountNumber, long long newAccountBalance);
bool removeAccountFromList(allAccounts *accountsListPtr, unsigned long int accountNumber);
bool createAccountBalanceString(account *accountsListPtr);
bool isAccountInList(allAccounts *accountsListPtr, unsigned long int newAccountNumber);
bool initializeNewAccountsList(allAccounts *accountsListPtr);

bool printCurrentBalancesInBank();

#endif