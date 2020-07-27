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

void clockExit(int sig){
        exit(0);
}

void error_handler_builtin(int val){
        (val == 0) ? fprintf(stderr,"Too many arguments\n") : fprintf(stderr,"Too many arguments\n");
        exit(1);
}

void echo_builtin(char ** arguments,int number_of_arguments){
        for (int i = 1; arguments[i] != NULL; i++)
                printf("%s ",arguments[i]);
        printf("\n");
        exit(0);
}

void remindme_builtin(char ** arguments,int number_of_arguments){
        if(number_of_arguments > 3)
                error_handler_builtin(0);
        else if(number_of_arguments < 3)
                error_handler_builtin(1);
        if(arguments[1] == NULL) {
                printf("Error\n");
                exit(1);
        }
        int givenTime = atoi(arguments[1]);
        sleep(givenTime);
        printf("\nREMINDER:");
        for(int i = 1; arguments[i] != NULL; i++)
                printf("%s ",arguments[i]);
        printf("\n");
        exit(0);
}

void pwd_builtin(int number_of_arguments){
        if(number_of_arguments > 1)
                error_handler_builtin(0);
        printf("%s\n",present_working_directory);
        exit(0);
}
void cd_builtin(char ** arguments,int number_of_arguments){
        if(number_of_arguments > 2)
                error_handler_builtin(0);
        if(arguments[1] == NULL) {
                if(chdir(home) == -1)
                        perror("shell");
        }
        else{
                if(chdir(arguments[1]) == -1)
                        perror("shell");
        }
}

void clock_builtin(char ** arguments,int number_of_arguments){
        signal(SIGINT,clockExit);
        if(number_of_arguments > 3)
                error_handler_builtin(0);
        else if(number_of_arguments < 3)
                error_handler_builtin(1);
        while (1) {
                char temp1[1000],temp2[1000];
                if(arguments[2] != NULL)
                        sprintf(temp2,"/proc/driver/rtc");
                int fdDriverRTC = open(temp2, O_RDONLY);
                if(fdDriverRTC < 0) {
                        fprintf(stderr, "Failed to open file \"%s\"\n", temp2);
                        exit(1);
                }
                if(read(fdDriverRTC, temp1, 1000) < 0) {
                        fprintf(stderr, "Failed to read file \"%s\"\n",temp1);
                        exit(1);
                }
                char *token2 = strtok(temp1,"\n");
                token2 = strtok(NULL," \n");
                token2 = strtok(NULL," \n");
                char *token1 = strtok(NULL,"\n");
                token1 = strtok(NULL," \n");
                token1 = strtok(NULL," \n");
                printf("\n%s %s\n",token2,token1);
                sleep(atoi(arguments[2]));
        }
        exit(0);
}
