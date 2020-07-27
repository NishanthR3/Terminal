#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

char * getInput(){
        char *input = NULL;
        ssize_t size = 0;
        getline(&input, &size, stdin);
        char *token = strtok(input,"\n");
        return token;
}

char **format(char *input, char* delim){
        int size = 50, i = 0;
        char **args = malloc(size * sizeof(char*));
        char *token = strtok(input, delim);
        while (token != NULL) {
                args[i] = token;
                i++;
                if (i >= size) {
                        size += 100;
                        args = realloc(args, size * sizeof(char*));
                }
                token = strtok(NULL, delim);
        }
        args[i] = NULL;
        return args;
}

void run_command(char *mul_args);
