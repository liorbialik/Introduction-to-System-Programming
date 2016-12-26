#ifndef ACCOUNT_LIST_UTILS
#define ACCOUNT_LIST_UTILS

/* Libraries: */
#include "BankManager.h"
#include <stdbool.h>

/* Function Declarations: */
bool initializeNewAccountsList(allAccounts *accountsListPtr, logFile *runtmieLogFile);
bool isAccountInList(allAccounts *accountsListPtr, unsigned long long newAccountNumber);
bool addNewAccountToList(allAccounts *accountsListPtr, unsigned long long newAccountNumber, double newAccountBalance);
bool removeAccountFromList(allAccounts *accountsListPtr, unsigned long long accountNumber);
//bool createAccountBalanceString(account *accountsListPtr);
bool printCurrentBalancesInBank(allAccounts *accountsListPtr);
bool depositAmountToAccount(allAccounts *accountsListPtr, unsigned long long accountNumberForDeposit, double amountForDeposit);
bool WithdrawalAmountFromAccount(allAccounts *accountsListPtr, unsigned long long accountNumberToWithdrawal, double amountToWithdrawal);


#endif