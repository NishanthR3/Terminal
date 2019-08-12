#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "main.h"
#include "commands.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <inttypes.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <errno.h>
#include "jobs.h"



typedef struct command {
        char * input_file;
        char * output_file;
        char * command;
        char ** arguments;
        int write_type;
        int number_of_arguments;
}commandObject;

typedef struct executeVar {
        int isBackground;
        int numberOfCommands;
        int terminalInput;
        int terminalStandardInput;
        int terminalStandardOutput;
        int command_length;
}executeVaraibles;

executeVaraibles shellExecutions;


void intialize(commandObject *val,int SIZE){
        val->output_file = NULL;
        val->input_file = NULL;
        val->write_type = 0;
        val->arguments = malloc(SIZE * sizeof(char *));
        val->number_of_arguments = 0;
}

void set_end(commandObject *val){
        val->output_file = NULL;
        val->input_file = NULL;
        val->write_type = 0;
        val->arguments = NULL;
        val->command = NULL;
}

void shell_intialization() {
        shellExecutions.terminalStandardInput = dup(0);
        shellExecutions.terminalStandardOutput = dup(1);
        shellExecutions.terminalInput = 0;
        shellExecutions.numberOfCommands = 0;
}

int error_handler(int val){
        if(val == 1)
                perror("Memory not allocated");
        else if(val == 2)
                fprintf(stderr, "No Input file given\n");
        else if(val == 3)
                fprintf(stderr, "No Output file given\n");
        else if(val == 4) {
                perror("Command not executed");
                exit(0);
        }
        return 1;
}

void pipe_close(int pipes[]) {
        if(shellExecutions.terminalInput != 0)
                close(shellExecutions.terminalInput);
        close(pipes[1]);
        shellExecutions.terminalInput = pipes[0];
}

void shell_restoration(){
        dup2(shellExecutions.terminalStandardInput, 0);
        dup2(shellExecutions.terminalStandardOutput, 1);
        close(shellExecutions.terminalStandardInput);
        close(shellExecutions.terminalStandardOutput);
}

int commandConverter (char * s,commandObject **commandList){
        int i = 0,SIZE = 1000;
        (*commandList) = malloc(SIZE * sizeof(commandObject));
        char *token = strtok(s,"|");
        while (token != NULL) {
                (*commandList)[i].command = token;
                i++;
                if (i >= SIZE) {
                        SIZE += 100;
                        (*commandList) = realloc(commandList, SIZE * sizeof(commandObject));
                }
                token = strtok(NULL,"|");
        }
        set_end(&(*commandList)[i]);
        int numberOfPipes = i;

        for (int i=0; i<numberOfPipes; ++i) {
                token = (*commandList)[i].command;
                intialize(&(*commandList)[i],SIZE);
                if((*commandList)[i].arguments == NULL)
                        return error_handler(1);

                char *token1 = strtok(token, " \t");
                while(token1 != NULL) {
                        if((strcmp(token1, ">") == 0) || (strcmp(token1, ">>") == 0)) {
                                (*commandList)[i].write_type = (strcmp(token1, ">>") == 0) ?  1 : 0;
                                token1 = strtok(NULL, " \t");
                                if(token1 == NULL)
                                        return error_handler(2);
                                else
                                        (*commandList)[i].output_file = token1;
                        }
                        else if(strcmp(token1, "<") == 0) {
                                token1 = strtok(NULL, " \t");
                                if(token1 == NULL)
                                        return error_handler(3);
                                else
                                        (*commandList)[i].input_file = token1;
                        }
                        else
                                (*commandList)[i].arguments[(*commandList)[i].number_of_arguments++] = token1;
                        token1 = strtok(NULL, " \t");
                }
        }
        return 0;
}

void runCommand (char * command){
        shellExecutions.command_length = strlen(command);
        shellExecutions.isBackground = 0;
        if(shellExecutions.command_length >= 2 && command[shellExecutions.command_length-1] == '&') {
                command[shellExecutions.command_length-1] = '\0';
                shellExecutions.isBackground = 1;
                --shellExecutions.command_length;
        }
        commandObject * commandList;
        if(commandConverter(command,&commandList) == 1)
                return;

        shell_intialization();
        while(commandList[shellExecutions.numberOfCommands].arguments)
                shellExecutions.numberOfCommands++;

        for(int i=0; commandList[i].arguments!=NULL; ++i) {

                if(commandList[i].arguments[0] == NULL)
                        return;

                if(strcmp(commandList[i].arguments[0],"cd") == 0) {
                        cd_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"fg") == 0) {
                        fg_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"setenv") == 0) {
                        setenv_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"unsetenv") == 0) {
                        unsetenv_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"jobs") == 0) {
                        jobs_builtin(commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"kjob") == 0) {
                        kjob_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"bg") == 0) {
                        bg_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"overkill") == 0) {
                        overkill_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        continue;
                }
                else if(strcmp(commandList[i].arguments[0],"quit") == 0) {
                        exit(0);
                        continue;
                }

                int pipes[2];
                pipe(pipes);
                pid_t pid = fork();
                if(pid == 0) {
                        if(shellExecutions.numberOfCommands > 1) {
                                if(i > 0 && commandList[i + 1].arguments != NULL
                                   && shellExecutions.terminalInput != 0) {
                                        dup2(pipes[1], 1);
                                        dup2(shellExecutions.terminalInput, 0);
                                }
                                else if(i == 0 && commandList[i + 1].arguments != NULL
                                        && shellExecutions.terminalInput == 0)
                                        dup2(pipes[1], 1);
                                else
                                        dup2(shellExecutions.terminalInput, 0);
                        }
                        if(commandList[i].output_file != NULL) {
                                int commandOutputFD = (commandList[i].write_type == 0) ?
                                                      open(commandList[i].output_file, O_WRONLY | O_TRUNC | O_CREAT, 0664) :
                                                      open(commandList[i].output_file, O_WRONLY | O_CREAT | O_APPEND, 0664);
                                if(commandOutputFD == -1)
                                        error_handler(4);
                                dup2(commandOutputFD, 1);
                                close(commandOutputFD);
                        }
                        if(commandList[i].input_file != NULL) {
                                int commandInputFD;
                                if((commandInputFD = open(commandList[i].input_file, O_RDONLY) ) == -1)
                                        error_handler(4);
                                dup2(commandInputFD, 0);
                                close(commandInputFD);
                        }
                        if(strcmp(commandList[i].arguments[0],"pwd") == 0)
                                pwd_builtin(commandList[i].number_of_arguments);
                        else if(strcmp(commandList[i].arguments[0],"echo") == 0)
                                echo_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        else if(strcmp(commandList[i].arguments[0],"ls") == 0)
                                ls_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        else if(strcmp(commandList[i].arguments[0],"pinfo") == 0)
                                pinfo_builtin(commandList[i].arguments[1],commandList[i].number_of_arguments);
                        else if(strcmp(commandList[i].arguments[0],"remindme") == 0)
                                remindme_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        else if(strcmp(commandList[i].arguments[0],"clock") == 0)
                                clock_builtin(commandList[i].arguments,commandList[i].number_of_arguments);
                        else{
                                setpgid(0,0);
                                execvp(commandList[i].arguments[0], commandList[i].arguments);
                                error_handler(4);
                        }
                }
                else {
                        if(shellExecutions.isBackground == 0) {
                                add_to_foreground(pid, commandList[i].arguments[0]);
                                tcsetpgrp(0, pid);
                                siginfo_t fgStatus;
                                waitid(P_PID, pid, &fgStatus, (WUNTRACED | WNOWAIT));
                        }
                        if(shellExecutions.isBackground == 1)
                                add_to_background(pid, commandList[i].arguments[0]);
                }
                if(shellExecutions.numberOfCommands > 1)
                        pipe_close(pipes);
        }
        shell_restoration();
        return;
}
