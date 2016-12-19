#include <stdio.h>
#include <stdbool.h>
#include "BankManager.h"

bool addNewAccountToList(allAccounts *accountsListPtr, unsigned long int newAccountNumber, long long newAccountBalance) {
	
	account *newAccountPtr = malloc(sizeof(account));
	if (newAccountPtr = NULL) {
		printf("Memory allocation for new account failed!");
		return false;
	}

	// creating the new account
	newAccountPtr->accountNumber = newAccountNumber;
	newAccountPtr->initialBalance = newAccountBalance;
	newAccountPtr->currentBalance = newAccountBalance;
	newAccountPtr->totalDepositeSum = 0;
	newAccountPtr->totalWithdrawalSum = 0;
	newAccountPtr->ammountOfDeposits = 0;  // TODO: Should we count the account's initial creation as a deposite?
	newAccountPtr->ammountOfWithdrawals = 0;
	// another field for the account's mutex
	newAccountPtr->nextInList = NULL;

	// search the account list for the correct position for the new account (to preserve ascending order)
	if (accountsListPtr->totalNumberOfAccounts != 0) {
		while (accountsListPtr->accountListHeadPtr->nextInList != NULL) {
			//check where accountListHead->accountNumber < newAccountPtr->accountNumber < accountListHead->nextInList->accountNumber
		}
	
	}
	
	else {
		accountsListPtr->accountListHeadPtr = newAccountPtr;
	}


	return true;
}

bool removeAccountFromList(allAccounts *accountsListPtr, unsigned long int accountNumber) {
	return true;
}

bool createAccountBalanceString(account *accountsListPtr) {
	return true;
}

bool isAccountInList(allAccounts *accountsListPtr, unsigned long int newAccountNumber) {
	
	printf("Checking if account number %ld is in the accounts list\n", newAccountNumber);

	account *currentAccountPtr = NULL;

	// check if the accounts list is empty:
	if (accountsListPtr->accountListHeadPtr == NULL)
		return false;

	else{
		for(currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL; 
			currentAccountPtr = currentAccountPtr->nextInList){

			if (newAccountNumber == currentAccountPtr->accountNumber)
				return true;
		}

		return false;
	}
}

bool initializeNewAccountsList(allAccounts *accountsListPtr) {
	
	printf("Initializing new allAccounts instance\n");
	accountsListPtr->accountListHeadPtr = NULL;
	accountsListPtr->totalNumberOfAccounts = 0;
	accountsListPtr->runtimeLogFilePtr = NULL;

	return true;
}