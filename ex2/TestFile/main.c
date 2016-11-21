

#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <Strsafe.h>
#include <tchar.h>

char *getFileName(char *path);
char *getFileExtention(char *filename);
char *getOutputFileName(char *fileName, char *outputFileNameEnding);
BOOL GetFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString);
void writeTestResultsToFile(FILE *outputFile, char *fileName, char *fileExtention, DWORD fileSize,
							TCHAR *creationTimeStringPtr, TCHAR *fileLastModifiedTimeString, char *firstFiveCharsPtr);


int main(int argc, char *argv[]) {
	char *filePath = NULL, *outputLogFilePath = NULL, *outputLogFileName = NULL, *fileName = NULL, *fileExtention = NULL,
		*outputFileNameEnding = { "_log.txt" }, *firstFiveCharsPtr = NULL;
	char firstFiveCharsFromFile[6];
	FILE *inputFile = NULL, *outputFile = NULL;
	HANDLE fileHandler;
	DWORD fileSize = 0; 
	LPTSTR *creationTimeString = NULL, *lastAccessedTimeString = NULL;
	FILETIME fileCreationTime, fileLastModifiedTime;
	TCHAR fileCreationTimeString[21], fileLastModifiedTimeString[21];
	TCHAR *creationTimeStringPtr = NULL, *lastModifiedTimeStringPtr = NULL;

	filePath = argv[1]; 
	outputLogFilePath = argv[2];
	fileName = getFileName(filePath);
	outputLogFileName = getOutputFileName(fileName, outputFileNameEnding);
	
	// read input file
	inputFile = fopen(fileName, "r");
	if (inputFile == NULL) {
		printf("Could not open file, error %ul\n", GetLastError()); //TODO: need to add an error handling function
		exit(1);
	}

	// creat log output file
	outputFile = fopen(outputLogFileName, "w");
	if (outputFile == NULL) {
		printf("Input File openning failed!");
		exit(1);
	}

	// getting the file extention
	fileExtention = getFileExtention(fileName);
	

	// getting first five chars from file
	fgets(firstFiveCharsFromFile, 6, inputFile);
	firstFiveCharsPtr = firstFiveCharsFromFile;
	printf("firstFiveCharsFromFile: %s\n", firstFiveCharsFromFile);

	fileHandler = (HANDLE)_get_osfhandle(_fileno(inputFile));
	if ( fileHandler == INVALID_HANDLE_VALUE ) {
		printf("Could not open file, error %ul\n", GetLastError());
		exit( 1 );
	}

	// getting file's size:
	fileSize = GetFileSize( fileHandler, NULL );

	printf("%s size is %d bytes\n", fileName, fileSize); // REMOVE - just for testing
	
	//getting file's creating time and last accessed time
	GetFileTime( fileHandler, &fileCreationTime, NULL, &fileLastModifiedTime );
	
	if (GetFileTimeString(fileCreationTime, fileCreationTimeString)) {
		_tprintf(TEXT("file creation time is: %s\n"), fileCreationTimeString); // REMOVE - just for testing
		creationTimeStringPtr = fileCreationTimeString;
	}

	if (GetFileTimeString(fileLastModifiedTime, fileLastModifiedTimeString)) {
		_tprintf(TEXT("file last accessed time is: %s\n"), fileLastModifiedTimeString); // REMOVE - just for testing
		lastModifiedTimeStringPtr = fileLastModifiedTimeString;
	}

	writeTestResultsToFile(outputFile, fileName, fileExtention, fileSize, creationTimeStringPtr, fileLastModifiedTimeString, firstFiveCharsPtr);

	free( outputLogFileName );
	fclose(inputFile); // this closes both the FILE and the HANDLE of the input file
	return 0;
}


void writeTestResultsToFile(FILE *outputFile, char *fileName, char *fileExtention, DWORD fileSize,
							TCHAR *creationTimeStringPtr, TCHAR *fileLastModifiedTimeString, char *firstFiveCharsPtr) {
	
	
	fprintf(outputFile, "%s\n", fileName);
	fprintf(outputFile, "The file extension of the test file is \".%s\"\n", fileExtention);
	fprintf(outputFile, "The test file size is %d bytes\n", fileSize);
	fprintf(outputFile, "The file was created on %ws\n", creationTimeStringPtr);
	fprintf(outputFile, "The file was last modified on %ws\n", fileLastModifiedTimeString);
	fprintf(outputFile, "The files first 5 bytes are: %s\n", firstFiveCharsPtr);

	fclose(outputFile);
	return;
}


BOOL GetFileTimeString(FILETIME fileCreationTime, LPTSTR bufferForString){
	/* 
	inspired by https://msdn.microsoft.com/en-us/library/windows/desktop/ms724926(v=vs.85).aspx 
	*/

	Sleep(10);

	DWORD timeInStringFormatSize = 21;
	SYSTEMTIME utcTime, localTime;
	DWORD timeToStringFormatResult;
	
	// Convert the last-write time to local time.
	FileTimeToSystemTime( &fileCreationTime, &utcTime );
	SystemTimeToTzSpecificLocalTime( NULL, &utcTime, &localTime );

	// Build a string showing the date and time.
	timeToStringFormatResult = StringCchPrintf( bufferForString, timeInStringFormatSize,
		TEXT("%02d/%02d/%04d  %02d:%02d:%02d"),
		localTime.wMonth, localTime.wDay, localTime.wYear,
		localTime.wHour, localTime.wMinute, localTime.wSecond );

	if (timeToStringFormatResult == S_OK )
		return TRUE;
	else return FALSE;
}


char *getFileName(char *path){
	/*
	inspired by http://stackoverflow.com/questions/5901624/extract-file-name-from-full-path-in-c-using-msvs2005
	*/

	Sleep(10);

	char *filename = strrchr( path, '\\' );
	if ( filename == NULL )
		filename = path;
	else
		filename++;
	return filename;
}


char *getOutputFileName(char *fileName, char *outputFileNameEnding) {
	/*
	inspired by our program from EX1
	*/

	Sleep(10);

	size_t outputFileNameLength = 0;
	char *outputFileName = NULL;

	outputFileNameLength = strlen(fileName) + strlen(outputFileNameEnding) - 4 + 1; // '-4' deducts the '.txt' ending since it appearse twice. '+1' accounts for the '\0' ending
	outputFileName = malloc(sizeof(char) * outputFileNameLength); // the '4' stands for the shared ".txt" 
	if (outputFileName == NULL) {
		printf("outputFileName allocation faild.");
		exit(1);
	}
	strcpy(outputFileName, fileName);
	outputFileName[strlen(outputFileName) - 4] = '\0'; // mark the end of the precious string before the '.txt' ending
	strcat(outputFileName, outputFileNameEnding); // add the correct ending
	return outputFileName;
}


char *getFileExtention(char *filename) {
	/*
	inspired by http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
	*/

	Sleep(10);

	char *dotLocationInFileName = strrchr(filename, '.');
	if (!dotLocationInFileName || dotLocationInFileName == filename)
		return "Error";
	return dotLocationInFileName + 1;
}


// errorHandling(){
//    free all allocations
//    free all threads
//    close all files
//    print the error on the screen -> use GetLastError()
//    write the error into the outputFile
//    in case of an error in one of the threads, should have a dictionary that prints an error into the outputfile for the relevant thread
// }
