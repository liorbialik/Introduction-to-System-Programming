#ifndef ACCOUNT_LIST_UTILS
#define ACCOUNT_LIST_UTILS

/* Libraries: */
#include "BankManager.h"
#include <stdbool.h>

/* Function Declarations: */
bool initializeNewAccountsList(allAccounts *accountsListPtr, logFile *runtmieLogFilePtr);
bool initializeNewAccount(account *accountPtr, unsigned long long accountNumber, double accountBalance);
bool isAccountInList(allAccounts *accountsListPtr, unsigned long long newAccountNumber);
bool addNewAccountToList(allAccounts *accountsListPtr, unsigned long long newAccountNumber, double newAccountBalance);
bool removeAccountFromList(allAccounts *accountsListPtr, unsigned long long accountNumberToClose);
//bool createAccountBalanceString(account *accountsListPtr);
bool printCurrentBalances(allAccounts *accountsListPtr);
bool depositOrWithdrawalAmountToAccount(allAccounts *accountsListPtr, unsigned long long accountNumber, double amount, int enumCommandTypeIndex);
void updateAccountFieldsForDepositing(currentAccountPtr);
void updateAccountFieldsForWithdrawal(currentAccountPtr);

#endif