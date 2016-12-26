#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'

#include <stdio.h>
#include <stdbool.h>
#include "accountsListUtils.h"
#include "BankManager.h"

bool addNewAccountToList(allAccounts *accountsListPtr, unsigned long long newAccountNumber, double newAccountBalance) {

	account *newAccountPtr = NULL, *currentAccountPtr = NULL;
	printf("Adding new account number %llu to list\n", newAccountNumber);

	// check whether the account number already exists
	if (isAccountInList(accountsListPtr, newAccountNumber)) {
		printf("!!! Account number %llu already exists. Can't create account. Skipping command. !!!\n", newAccountNumber);
		fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "!!! Account number %llu already exists. Can't create account. Skipping command. !!!\n", newAccountNumber);
		return true;
	}

	// allocate memory for the new account:
	newAccountPtr = malloc(sizeof(account));

	if (newAccountPtr == NULL) {
		printf("Memory allocation for new account failed!");
		return false;
	}

	// creating the new account
	if (!initializeNewAccount(newAccountPtr, newAccountNumber, newAccountBalance)) {
		printf("Account number %llu initialization Faild!\n", newAccountNumber);
		return false;
	}
		
	if (accountsListPtr->totalNumberOfAccounts == 0) {
		accountsListPtr->accountListHeadPtr = newAccountPtr;
		accountsListPtr->totalNumberOfAccounts++;
	}

	if (newAccountNumber < accountsListPtr->accountListHeadPtr->accountNumber) {
		newAccountPtr->nextInList = accountsListPtr->accountListHeadPtr;
		accountsListPtr->accountListHeadPtr = newAccountPtr;
		accountsListPtr->totalNumberOfAccounts++;
	}

	if (newAccountNumber > accountsListPtr->accountListHeadPtr->accountNumber) {
		for (currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr->nextInList != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			if (newAccountNumber > currentAccountPtr->accountNumber && 
				newAccountNumber < currentAccountPtr->nextInList->accountNumber) {
				
				newAccountPtr->nextInList = currentAccountPtr->nextInList;
				currentAccountPtr->nextInList = newAccountPtr;
				accountsListPtr->totalNumberOfAccounts++;
				break;
			}
		}

		if (currentAccountPtr->nextInList == NULL) {
			currentAccountPtr->nextInList = newAccountPtr;
			accountsListPtr->totalNumberOfAccounts++;
		}

	}

	printf("Successfully created bank account number %llu with current balance of %.2f.\n", newAccountNumber, newAccountBalance);
	fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "Successfully created bank account number %llu with current balance of %.2f.\n", newAccountNumber, newAccountBalance);
	return true;
}

bool removeAccountFromList(allAccounts *accountsListPtr, unsigned long long accountNumberToClose) {
	
	account *currentAccountPtr = NULL, *temporaryAccountPtr = NULL;
	printf("Removing account number %llu from list\n", accountNumberToClose);
	// check whether the account number already exists
	if (!isAccountInList(accountsListPtr, accountNumberToClose)) {
		printf("!!! Account number %llu doesn't exist. Can�t close account. Skipping command. !!!\n", accountNumberToClose);
		fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "!!! Account number %llu doesn't exist. Can�t close account. Skipping command. !!!\n", accountNumberToClose);
		return true;
	}

	if (accountsListPtr->accountListHeadPtr->accountNumber == accountNumberToClose) {
		temporaryAccountPtr = accountsListPtr->accountListHeadPtr;
		accountsListPtr->accountListHeadPtr = temporaryAccountPtr->nextInList;
		accountsListPtr->totalNumberOfAccounts--;
		free(temporaryAccountPtr);
	}

	else {
		for (currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr->nextInList != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			if (currentAccountPtr->nextInList->accountNumber == accountNumberToClose)
				temporaryAccountPtr = currentAccountPtr->nextInList;
				currentAccountPtr->nextInList = temporaryAccountPtr->nextInList;
				accountsListPtr->totalNumberOfAccounts--;
				free(temporaryAccountPtr);
				break;
			}
		}

	printf("Successfully closed bank account number %llu.\n", accountNumberToClose);
	fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "Successfully closed bank account number %llu.\n", accountNumberToClose);
	return true;
}

bool isAccountInList(allAccounts *accountsListPtr, unsigned long long newAccountNumber) {

	printf("Checking if account number %lli is in the accounts list\n", newAccountNumber);

	account *currentAccountPtr = NULL;

	// check if the accounts list is empty:
	if (accountsListPtr->accountListHeadPtr == NULL) {
		printf("Account number %lli is NOT in the accounts list\n", newAccountNumber);
		return false;
	}
	else{
		for(currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL; 
			currentAccountPtr = currentAccountPtr->nextInList){

			if (newAccountNumber == currentAccountPtr->accountNumber) {
				printf("Account number %lli is in the accounts list\n", newAccountNumber);
				return true;
			}
		}
		printf("Account number %lli is NOT in the accounts list\n", newAccountNumber);
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

bool initializeNewAccount(account *accountPtr, unsigned long long accountNumber, double accountBalance) {
	
	printf("Account number %llu initialization \n", accountNumber);
	accountPtr->accountNumber = accountNumber;
	accountPtr->initialBalance = accountBalance;
	accountPtr->currentBalance = accountBalance;
	accountPtr->totalDepositeSum = 0;
	accountPtr->totalWithdrawalSum = 0;
	accountPtr->ammountOfDeposits = 0;
	accountPtr->ammountOfWithdrawals = 0;
	// another field for the account's mutex
	accountPtr->nextInList = NULL;
	return true;
}

bool printCurrentBalances(allAccounts *accountsListPtr) {

	account *currentAccountPtr = NULL;

	printf("Current balances in bank accounts are:\nBank Account #,Current Balance\n");
	fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "Current balances in bank accounts are:\nBank Account #,Current Balance\n");

	// check if the accounts list is empty:
	if (accountsListPtr->accountListHeadPtr != NULL) {
		for (currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			printf("%llu,%.2f\n", currentAccountPtr->accountNumber, currentAccountPtr->currentBalance);
			fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "%llu,%.2f\n", currentAccountPtr->accountNumber, currentAccountPtr->currentBalance);
		}
	}
	return true;
}


// TODO: need to move this into bandManager.c
bool printBalanceReport(allAccounts *accountsListPtr, char *BalanceReportFileName) {
	
	FILE *BalanceReportFile = NULL;
	account *currentAccountPtr = NULL;
	printf("Printing finale balance report\n");

	// open the balance report file:
	BalanceReportFile = fopen(BalanceReportFileName, "w");
	if (BalanceReportFile == NULL) {
		perror("Error: ");
		printf("Could not open balance report file, error %ul\n", GetLastError());
		exit(1);
	}

	fprintf(BalanceReportFile, "Summary of balances in bank accounts:\n");
	fprintf(BalanceReportFile, "Bank Account #, Current Balance, Initial Balance, Total Deposited, Total Withdrawal, # of Deposits, # of Withdrawals\n");
	if (accountsListPtr->accountListHeadPtr != NULL) {
		for (currentAccountPtr = accountsListPtr->accountListHeadPtr;
			currentAccountPtr != NULL;
			currentAccountPtr = currentAccountPtr->nextInList) {

			fprintf(BalanceReportFile, "%llu,%.2f,%.2f,%.2f,%.2f,%llu,%llu\n",
				currentAccountPtr->accountNumber,
				currentAccountPtr->currentBalance,
				currentAccountPtr->initialBalance,
				currentAccountPtr->totalDepositeSum,
				currentAccountPtr->totalWithdrawalSum,
				currentAccountPtr->ammountOfDeposits,
				currentAccountPtr->ammountOfWithdrawals);
		}
	}

	fclose(BalanceReportFile);
	return true;
}

bool depositOrWithdrawalAmountToAccount(allAccounts *accountsListPtr, unsigned long long accountNumber, double amount, int enumCommandTypeIndex) {		//TODO: Need to test

	account *currentAccountPtr = NULL;

	//check that account number isn't locked by mutex using WaitForSingleObject function on the relevant thread

	// checking if accountNumber exists
	if (!isAccountInList(accountsListPtr, accountNumber)) {
		if (enumCommandTypeIndex == 3) {
			printf("!!! Unable to deposit %.2f to account number %lli. Account doesn't exist. Skipping command. !!!\n", amount, accountNumber);
			fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "!!! Unable to deposit %.2f to account number %lli. Account doesn't exist. Skipping command. !!!\n", amount, accountNumber);
		}
		else if (enumCommandTypeIndex == 4) {
			printf("!!! Unable to withdraw %.2f from account number %lli. Account doesn't exist. Skipping command. !!!\n", amount, accountNumber);
			fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "!!! Unable to withdraw %.2f from account number %lli. Account doesn't exist. Skipping command. !!!\n", amount, accountNumber);
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
	fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "Successfully deposited %.2f to account number %lli\n", amountForDeposit, accountNumberForDeposit);
}

void updateAccountFieldsForWithdrawal(allAccounts *accountsListPtr, account *currentAccountPtr, unsigned long long accountNumberToWithdrawal, double amountToWithdrawal) {

	currentAccountPtr->currentBalance -= amountToWithdrawal;
	currentAccountPtr->ammountOfWithdrawals += 1;
	currentAccountPtr->totalWithdrawalSum += amountToWithdrawal;
	printf("Successfully withdraw %.2f from account number %lli\n", amountToWithdrawal, accountNumberToWithdrawal);
	fprintf(accountsListPtr->runtmieLogFile->logFilePtr, "Successfully withdraw %.2f from account number %lli\n", amountToWithdrawal, accountNumberToWithdrawal);
}