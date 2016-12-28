#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'

#include <stdio.h>
#include <stdbool.h>
#include "accountsListUtils.h"
#include "BankManager.h"

int addNewAccountToList(commandArguments *newCommandArguments) {

	account *newAccountPtr = NULL, *currentAccountPtr = NULL;
	unsigned long long newAccountNumberToCreate = newCommandArguments->accountNumber;
	double newAccountBalance = newCommandArguments->amountOfMoney;

	printf("Adding new account number %llu with current balance of %.2f to list\n", 
		newAccountNumberToCreate,
		newAccountBalance);

	// check whether the account number already exists
	if (isAccountInList(newCommandArguments)) {
		printf("!!! Account number %llu already exists. Can't create account. Skipping command. !!!\n", 
			newAccountNumberToCreate);
		fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
			"!!! Account number %llu already exists. Can't create account. Skipping command. !!!\n", 
			newAccountNumberToCreate);
		return 1;
	}

	// allocate memory for the new account:
	newAccountPtr = malloc(sizeof(account));

	if (newAccountPtr == NULL) {
		printf("Memory allocation for new account failed!");
		return 1;
	}

	// creating the new account
	if (!initializeNewAccount(newAccountPtr, newAccountNumberToCreate, newAccountBalance)) {
		printf("Account number %llu initialization Faild!\n", 
			newAccountNumberToCreate);
		return 1;
	}
		
	if (newCommandArguments->accountsListPtr->totalNumberOfAccounts == 0) {
		newCommandArguments->accountsListPtr->accountListHeadPtr = newAccountPtr;
		newCommandArguments->accountsListPtr->totalNumberOfAccounts++;
	}

	if (newAccountNumberToCreate < newCommandArguments->accountsListPtr->accountListHeadPtr->accountNumber) {
		newAccountPtr->nextInList = newCommandArguments->accountsListPtr->accountListHeadPtr;
		newCommandArguments->accountsListPtr->accountListHeadPtr = newAccountPtr;
		newCommandArguments->accountsListPtr->totalNumberOfAccounts++;
	}

	if (newAccountNumberToCreate > newCommandArguments->accountsListPtr->accountListHeadPtr->accountNumber) {
		for (currentAccountPtr = newCommandArguments->accountsListPtr->accountListHeadPtr;
			currentAccountPtr->nextInList != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			if (newAccountNumberToCreate > currentAccountPtr->accountNumber &&
				newAccountNumberToCreate < currentAccountPtr->nextInList->accountNumber) {
				
				newAccountPtr->nextInList = currentAccountPtr->nextInList;
				currentAccountPtr->nextInList = newAccountPtr;
				newCommandArguments->accountsListPtr->totalNumberOfAccounts++;
				break;
			}
		}

		if (currentAccountPtr->nextInList == NULL) {
			currentAccountPtr->nextInList = newAccountPtr;
			newCommandArguments->accountsListPtr->totalNumberOfAccounts++;
		}

	}

	printf("Successfully created bank account number %llu with current balance of %.2f.\n", 
		newAccountNumberToCreate, 
		newAccountBalance);
	fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
		"Successfully created bank account number %llu with current balance of %.2f.\n", 
		newAccountNumberToCreate, 
		newAccountBalance);
	return 0;
}

int removeAccountFromList(commandArguments *newCommandArguments) {
	
	unsigned long long accountNumberToClose = newCommandArguments->accountNumber;

	account *currentAccountPtr = NULL, *temporaryAccountPtr = NULL;
	printf("Removing account number %llu from list\n", 
		accountNumberToClose);
	// check whether the account number already exists
	if (!isAccountInList(newCommandArguments)) {
		printf("!!! Account number %llu doesn't exist. Can’t close account. Skipping command. !!!\n", 
			accountNumberToClose);
		fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
			"!!! Account number %llu doesn't exist. Can’t close account. Skipping command. !!!\n", 
			accountNumberToClose);
		return 0;
	}

	if (newCommandArguments->accountsListPtr->accountListHeadPtr->accountNumber == accountNumberToClose) {
		temporaryAccountPtr = newCommandArguments->accountsListPtr->accountListHeadPtr;
		newCommandArguments->accountsListPtr->accountListHeadPtr = temporaryAccountPtr->nextInList;
		newCommandArguments->accountsListPtr->totalNumberOfAccounts--;
		free(temporaryAccountPtr);
	}

	else {
		for (currentAccountPtr = newCommandArguments->accountsListPtr->accountListHeadPtr;
			currentAccountPtr->nextInList != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			if (currentAccountPtr->nextInList->accountNumber == accountNumberToClose)
				temporaryAccountPtr = currentAccountPtr->nextInList;
				currentAccountPtr->nextInList = temporaryAccountPtr->nextInList;
				newCommandArguments->accountsListPtr->totalNumberOfAccounts--;
				free(temporaryAccountPtr);
				break;
			}
		}

	printf("Successfully closed bank account number %llu.\n", 
		accountNumberToClose);
	fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
		"Successfully closed bank account number %llu.\n", 
		accountNumberToClose);
	return 0;
}

bool isAccountInList(commandArguments *newCommandArguments) {

	printf("Checking if account number %lli is in the accounts list\n", newCommandArguments->accountNumber);

	account *currentAccountPtr = NULL;

	// check if the accounts list is empty:
	if (newCommandArguments->accountsListPtr->accountListHeadPtr == NULL) {
		printf("Account number %lli is NOT in the accounts list\n", 
			newCommandArguments->accountNumber);
		return false;
	}
	else{
		for(currentAccountPtr = newCommandArguments->accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL; 
			currentAccountPtr = currentAccountPtr->nextInList){

			if (newCommandArguments->accountNumber == currentAccountPtr->accountNumber) {
				printf("Account number %lli is in the accounts list\n", 
					newCommandArguments->accountNumber);
				return true;
			}
		}
		printf("Account number %lli is NOT in the accounts list\n", 
			newCommandArguments->accountNumber);
		return false;
	}
}

bool initializeNewAccount(account *accountPtr, unsigned long long accountNumber, double accountBalance) {
	
	printf("Account number %llu initialization \n", accountNumber);
	accountPtr->accountNumber = accountNumber;
	accountPtr->initialBalance = accountBalance;
	accountPtr->currentBalance = accountBalance;
	accountPtr->totalDepositeSum = 0;
	accountPtr->totalWithdrawalSum = 0;
	accountPtr->ammountOfDeposits = 0;
	accountPtr->ammountOfWithdrawals = 0;
	accountPtr->nextInList = NULL;
	accountPtr->accountMutex = CreateMutex(NULL, FALSE, NULL);
	if (accountPtr->accountMutex == NULL)
	{
		printf("CreateMutex for account error: %d\n", GetLastError());
		exit(1);
	}

	return true;
}

int printCurrentBalances(commandArguments *newCommandArguments) {

	account *currentAccountPtr = NULL;

	printf("Current balances in bank accounts are:\nBank Account #,Current Balance\n");
	fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
		"Current balances in bank accounts are:\nBank Account #,Current Balance\n");

	// check if the accounts list is not empty:
	if (newCommandArguments->accountsListPtr->accountListHeadPtr != NULL) {
		for (currentAccountPtr = newCommandArguments->accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			printf("%llu,%.2f\n", 
				currentAccountPtr->accountNumber, 
				currentAccountPtr->currentBalance);
			fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
				"%llu,%.2f\n", 
				currentAccountPtr->accountNumber, 
				currentAccountPtr->currentBalance);
		}
		return 0;
	}
	// account list is empty
	return 1;
}

int depositOrWithdrawalAmountToAccount(commandArguments *newCommandArguments) {

	account *currentAccountPtr = NULL;
	unsigned long long accountNumber = newCommandArguments->accountNumber;
	double amount = newCommandArguments->amountOfMoney;

	printf("Making deposite/withdrawal for account number %llu with current balance of %.2f\n",
		accountNumber,
		amount);

	// checking if accountNumber exists
	if (!isAccountInList(newCommandArguments)) {
		WaitForSingleObject(newCommandArguments->accountsListPtr->runtmieLogFile->logFiltMutex, INFINITE);
		if (newCommandArguments->commandTypeIndex == 3) {
			printf("!!! Unable to deposit %.2f to account number %lli. Account doesn't exist. Skipping command. !!!\n", 
				amount, 
				accountNumber);	
			fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
				"!!! Unable to deposit %.2f to account number %lli. Account doesn't exist. Skipping command. !!!\n", 
				amount, 
				accountNumber);
			ReleaseMutex(newCommandArguments->accountsListPtr->runtmieLogFile->logFiltMutex);
		}
		else if (newCommandArguments->commandTypeIndex == 4) {
			printf("!!! Unable to withdraw %.2f from account number %lli. Account doesn't exist. Skipping command. !!!\n", 
				amount, 
				accountNumber);
			fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
				"!!! Unable to withdraw %.2f from account number %lli. Account doesn't exist. Skipping command. !!!\n", 
				amount, 
				accountNumber);
		}
		ReleaseMutex(newCommandArguments->accountsListPtr->runtmieLogFile->logFiltMutex);
		return 0;
	}
	
	else {
		for (currentAccountPtr = newCommandArguments->accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			WaitForSingleObject(currentAccountPtr->accountMutex, INFINITE);
			if (accountNumber == currentAccountPtr->accountNumber) {
				//check if deposit or withdrawal command and execute correspondingly
				if (newCommandArguments->commandTypeIndex == 3) {
					//Deposit command
					makeDepositing(newCommandArguments, currentAccountPtr);
					break;
				}
				if (newCommandArguments->commandTypeIndex == 4) {
					//Withdrawal command
					makeWithdrawal(newCommandArguments, currentAccountPtr);
					break;
				}
			}
			ReleaseMutex(currentAccountPtr->accountMutex);
		}
	}
	
	return 0;
}

void makeDepositing(commandArguments *newCommandArguments, account *currentAccountPtr) {

	currentAccountPtr->currentBalance += newCommandArguments->amountOfMoney;
	currentAccountPtr->ammountOfDeposits += 1;
	currentAccountPtr->totalDepositeSum += newCommandArguments->amountOfMoney;
	printf("Successfully deposited %.2f to account number %lli.\n", 
		newCommandArguments->amountOfMoney, 
		newCommandArguments->accountNumber);
	
	WaitForSingleObject(newCommandArguments->accountsListPtr->runtmieLogFile->logFiltMutex, INFINITE);
	fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr, 
		"Successfully deposited %.2f to account number %lli.\n", 
		newCommandArguments->amountOfMoney, 
		newCommandArguments->accountNumber);
	ReleaseMutex(newCommandArguments->accountsListPtr->runtmieLogFile->logFiltMutex);
}

void makeWithdrawal(commandArguments *newCommandArguments, account *currentAccountPtr) {

	currentAccountPtr->currentBalance -= newCommandArguments->amountOfMoney;
	currentAccountPtr->ammountOfWithdrawals += 1;
	currentAccountPtr->totalWithdrawalSum += newCommandArguments->amountOfMoney;
	printf("Successfully withdrew %.2f from account number %lli.\n", 
		newCommandArguments->amountOfMoney, 
		newCommandArguments->accountNumber);

	WaitForSingleObject(newCommandArguments->accountsListPtr->runtmieLogFile->logFiltMutex, INFINITE);
	fprintf(newCommandArguments->accountsListPtr->runtmieLogFile->logFilePtr,
		"Successfully withdrew %.2f from account number %lli.\n", 
		newCommandArguments->amountOfMoney, 
		newCommandArguments->accountNumber);
	ReleaseMutex(newCommandArguments->accountsListPtr->runtmieLogFile->logFiltMutex);
}