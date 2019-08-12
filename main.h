#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define termi_green "\033[01;32m"
#define termi_blue "\033[01;34m"
#define termi_red "\033[31m"
#define termi_reset "\033[0m"

char present_working_directory[201],home[201];
char directory[201],hostname[201],user[201];

char * getInput();
char **format(char *input, char* delim);
int builtin(char *s);
void fork_exec(char ** args,int bg);
int fork_remid(char * s);
void prompt(char * home,char * directory);
void convertCWD(char * home1,char * cwd);
void runCommand(char * command);
