//
// Created by lior on 09/11/16.
//
#include <errno.h>
#include <stdlib.h>
#include "errorHandling.h"


// TODO: maybe add another error handling module?
void printErrorAndExitProgram(int errorNumber, char errorStr): {
    fprintf(stderr, "Value of errorNumber: %d\n", errorNumber);
    fprintf(stderr, "Error opening the file: %s\n", strerror(errorNumber));
    exit(1);
}