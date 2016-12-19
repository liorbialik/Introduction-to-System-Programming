#include <stdio.h>
#include <stdbool.h>
#include "BankManager.h"

bool addNewAccountToList(account *accountListHead, unsigned long int newAccountNumber, long long newAccountBalance) {
	
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
	if (accountListHead = !NULL) {
		while (accountListHead->nextInList != NULL) {
			//check where accountListHead->accountNumber < newAccountPtr->accountNumber < newAccountPtr->nextInList->accountNumber
		}
	
	}
	
	else {
		accountListHead = newAccountPtr;
	}

	return true;
}

bool removeAccountFromList(allAccounts *accountsList, unsigned long int accountNumber) {

}

bool createAccountBalanceString(account *accountToPrint) {

}
