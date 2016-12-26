#include <stdio.h>
#include <stdbool.h>
#include "accountsListUtils.h"
#include "BankManager.h"

bool addNewAccountToList(allAccounts *accountsListPtr, unsigned long long newAccountNumber, double newAccountBalance) {

	account *newAccountPtr = NULL;
	
	// check whether the account number already exists
	if (isAccountInList(accountsListPtr, newAccountNumber)) {
		printf("!!! Account number %llu already exists. Can�t create account. Skipping	command. !!!\n", newAccountNumber);
		// fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "!!! Account number %llu already exists. Can�t create account. Skipping	command. !!!\n", newAccountNumber);
		return true;
	}

	// allocate memory for the new account:
	newAccountPtr = malloc(sizeof(account));
	
	if (newAccountPtr == NULL) {
		printf("Memory allocation for new account failed!");
		return false;
	}

	// creating the new account
	newAccountPtr->accountNumber = newAccountNumber;
	newAccountPtr->initialBalance = newAccountBalance;
	newAccountPtr->currentBalance = newAccountBalance;
	newAccountPtr->totalDepositeSum = 0;
	newAccountPtr->totalWithdrawalSum = 0;
	newAccountPtr->ammountOfDeposits = 0;
	newAccountPtr->ammountOfWithdrawals = 0;
	// another field for the account's mutex
	newAccountPtr->nextInList = NULL;

	if (accountsListPtr->totalNumberOfAccounts == 0) {
		accountsListPtr->accountListHeadPtr = newAccountPtr;
		return true;
	}

	if (newAccountNumber < accountsListPtr->accountListHeadPtr->accountNumber) {
		newAccountPtr->nextInList = accountsListPtr->accountListHeadPtr;
		accountsListPtr->accountListHeadPtr = newAccountPtr;
		return true;
	}

	


	//// search the account list for the correct position for the new account (to preserve ascending order)
	//if (accountsListPtr->totalNumberOfAccounts != 0) {
	//	

	//	//while (accountslistptr->accountlistheadptr->nextinlist != null) {
	//	//	check where accountlisthead->accountnumber < newaccountptr->accountnumber < accountlisthead->nextinlist->accountnumber
	//	//}
	//
	//}
	//
	//else {
	//	accountsListPtr->accountListHeadPtr = newAccountPtr;
	//}


	return true;
}

bool removeAccountFromList(allAccounts *accountsListPtr, unsigned long long accountNumber) {
	return true;
}

bool isAccountInList(allAccounts *accountsListPtr, unsigned long long newAccountNumber) {
	
	printf("Checking if account number %lli is in the accounts list\n", newAccountNumber);

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

bool initializeNewAccountsList(allAccounts *accountsListPtr, logFile *runtmieLogFilePtr) {
	
	printf("Initializing new allAccounts instance\n");
	accountsListPtr->accountListHeadPtr = NULL;
	accountsListPtr->totalNumberOfAccounts = 0;
	accountsListPtr->runtmieLogFile = runtmieLogFilePtr;
	runtmieLogFilePtr->logFilePtr = NULL;
	return true;
}

bool printCurrentBalancesInBank(allAccounts *accountsListPtr) {		//TODO: Need to test

	account *currentAccountPtr = NULL;

	printf("Current balances in bank accounts are:\nBank Account #,Current Balance\n");

	// check if the accounts list is empty:
	if (accountsListPtr->accountListHeadPtr != NULL) {
		for (currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			printf("%lli, %.2f\n", currentAccountPtr->accountNumber, currentAccountPtr->currentBalance);
			//fprintf(RunTime_LogFile, "%lli, %.2f\n", currentAccountPtr->accountNumber, currentAccountPtr->currentBalance);

		}
	}
	return true;
}

bool depositOrWithdrawalAmountToAccount(allAccounts *accountsListPtr, unsigned long long accountNumber, double amount, int enumCommandTypeIndex) {		//TODO: Need to test

	account *currentAccountPtr = NULL;

	//check that account number isn't locked by mutex using WaitForSingleObject function on the relevant thread

	// checking if accountNumber exists
	if (isAccountInList(accountsListPtr, accountNumber) == 0) {
		if (enumCommandTypeIndex == 3) {
			printf("!!! Unable to deposit %.2f to account number %lli. Account doesn't exist. Skipping command\n", amount, accountNumber);
			fprintf(accountsListPtr->runtimeLogFilePtr, "!!! Unable to deposit %.2f to account number %lli. Account doesn't exist. Skipping command\n", amount, accountNumber);
		}
		else if (enumCommandTypeIndex == 4) {
			printf("!!! Unable to withdraw %.2f from account number %lli. Account doesn't exist. Skipping command\n", amount, accountNumber);
			//fprintf(accountsListPtr->runtimeLogFilePtr, "!!! Unable to withdraw %.2f from account number %lli. Account doesn't exist. Skipping command\n", amount, accountNumber);
		}
		return false;
	}
	else {
		 //accountNumber exists
		for (currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			if (accountNumber == currentAccountPtr->accountNumber) {
				//check if deposit or withdrawal command and execute correspondingly
				if (enumCommandTypeIndex == 3) {
					//Deposit command
					updateAccountFieldsForDepositing(accountsListPtr, currentAccountPtr, accountNumber, amount);
					break;
				}
				else if (enumCommandTypeIndex == 4) {
					//Withdrawal command
					updateAccountFieldsForWithdrawal(accountsListPtr, currentAccountPtr, accountNumber, amount);
					break;
				}
			}
		}
	}
	return true;
}

void updateAccountFieldsForDepositing(allAccounts *accountsListPtr, account *currentAccountPtr, unsigned long long accountNumberForDeposit, double amountForDeposit) {

	currentAccountPtr->currentBalance += amountForDeposit;
	currentAccountPtr->ammountOfDeposits += 1;
	currentAccountPtr->totalDepositeSum += amountForDeposit;
	printf("Successfully deposited %.2f to account number %lli\n", amountForDeposit, accountNumberForDeposit);
	fprintf(accountsListPtr->runtimeLogFilePtr, "Successfully deposited %.2f to account number %lli\n", amountForDeposit, accountNumberForDeposit);
}

void updateAccountFieldsForWithdrawal(allAccounts *accountsListPtr, account *currentAccountPtr, unsigned long long accountNumberToWithdrawal, double amountToWithdrawal) {

	currentAccountPtr->currentBalance -= amountToWithdrawal;
	currentAccountPtr->ammountOfWithdrawals += 1;
	currentAccountPtr->totalWithdrawalSum += amountToWithdrawal;
	printf("Successfully withdraw %.2f from account number %lli\n", amountToWithdrawal, accountNumberToWithdrawal);
	fprintf(accountsListPtr->runtimeLogFilePtr, "Successfully withdraw %.2f from account number %lli\n", amountToWithdrawal, accountNumberToWithdrawal);
}