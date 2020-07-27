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

typedef struct proc {
        struct proc * next_process;
        pid_t process_pid;
        char * command_name;
        char * input_file;
        char * output_file;
        int write_type;
        int process_number;
} process;

process * background_process = NULL,* foreground_process = NULL;

void jobs_error_handler(int val){
        if(val == 0)
                fprintf(stderr,"Too many arguments\n");
        else if(val == 1)
                fprintf(stderr,"Too few arguments\n");
}

void childHandler(int sig){
        pid_t pid;
        int status;
        while((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
                process * iterator = background_process,*prev = NULL;
                while(iterator) {
                        if(iterator->process_pid == pid) {
                                if(WIFEXITED(status)) printf("\n[%d] - %s with [%d] pid exited normally\n",
                                                             iterator->process_number,iterator->command_name,iterator->process_pid);
                                else printf("\n[%d] - %s with [%d] pid exited with error\n",
                                            iterator->process_number,iterator->command_name,iterator->process_pid);
                                if(prev) prev->next_process = iterator->next_process;
                                else background_process = iterator->next_process;
                                free(iterator);
                                break;
                        }
                        prev = iterator;
                        iterator = iterator->next_process;
                }
                if(foreground_process != NULL && foreground_process->process_pid == pid) {
                        tcsetpgrp(0,getpgid(0));
                        if(WIFSTOPPED(status))
                                add_to_background(foreground_process->process_pid,
                                                  foreground_process->command_name);
                        else if(WIFEXITED(status)) {
                                process * iterator = foreground_process;
                        }
                        free(iterator);
                        foreground_process = NULL;
                }
        }
}

void killForeground(int sig){
        if(foreground_process != NULL)
                kill(foreground_process->process_pid,SIGKILL);
}

void stopForeground(int sig){
        if(foreground_process != NULL)
                kill(foreground_process->process_pid,SIGTSTP);
}

void jobControl(){
        signal(SIGCHLD,childHandler);
        signal(SIGINT,killForeground);
        signal(SIGTSTP,stopForeground);
        signal(SIGTTOU, SIG_IGN);
}

void add_to_background(pid_t proc_id,char * com_name){
        process * current_process = malloc(sizeof(process));
        current_process->process_pid =  proc_id;
        current_process->next_process = background_process;
        current_process->command_name = malloc(sizeof(char) * (1+strlen(com_name)));
        if(background_process != NULL)
                current_process->process_number = background_process->process_number + 1;
        else
                current_process->process_number = 1;
        sprintf(current_process->command_name,"%s",com_name);
        background_process = current_process;
        printf("[%d] - %d\n",current_process->process_number,current_process->process_pid);
}

void add_to_foreground(pid_t proc_id,char * com_name){
        process * current_process = malloc(sizeof(process));
        current_process->process_pid =  proc_id;
        current_process->next_process = NULL;
        current_process->command_name = malloc(sizeof(char) * (1+strlen(com_name)));
        sprintf(current_process->command_name,"%s",com_name);
        foreground_process = current_process;
}

void setenv_builtin(char ** s,int number_of_arguments){
        if(number_of_arguments > 3) {
                jobs_error_handler(0);
                return;
        }
        else if(number_of_arguments < 2) {
                jobs_error_handler(1);
                return;
        }
        if(number_of_arguments == 3)
                setenv(s[1],s[2],1);
        else
                setenv(s[1],"\0",1);
}

void unsetenv_builtin(char ** s,int number_of_arguments){
        if(number_of_arguments > 2) {
                jobs_error_handler(0);
                return;
        }
        else if(number_of_arguments < 1) {
                jobs_error_handler(1);
                return;
        }
        unsetenv(s[1]);
}

void jobs_builtin(int number_of_arguments){
        if(number_of_arguments > 1) {
                jobs_error_handler(0);
                return;
        }
        process * iterator = background_process;
        while(iterator) {
                char s2[1000],s3[1000];
                sprintf(s3,"/proc/%d/stat",iterator->process_pid);
                int fd = open(s3, O_RDONLY);
                if(fd < 0) {
                        fprintf(stderr, "Failed to open file \"%s\"\n", s3);
                        //exit(1);
                        return;
                }
                if(read(fd, s2, 1000) < 0) {
                        fprintf(stderr, "Failed to read file \"%s\"\n",s3);
                        //exit(1);
                        return;
                }
                char *token2 = strtok(s2," ");
                token2 = strtok(NULL," ");
                token2 = strtok(NULL," ");
                if(strcmp("T",token2) == 0)
                        printf("[%d]\tStopped\t%s[%d]\n",
                               iterator->process_number,
                               iterator->command_name,iterator->process_pid);
                else
                        printf("[%d]\tRunning\t%s[%d]\n",
                               iterator->process_number,
                               iterator->command_name,iterator->process_pid);
                iterator = iterator->next_process;
        }
        //exit(0);
        return;
}

void kjob_builtin(char ** s,int number_of_arguments){
        if(number_of_arguments > 3) {
                jobs_error_handler(0);
                return;
        }
        else if(number_of_arguments < 3) {
                jobs_error_handler(1);
                return;
        }
        process * iterator = background_process;
        while(iterator) {
                if(iterator->process_number == atoi(s[1])) {
                        printf("%d %d\n",iterator->process_number,iterator->process_pid);
                        kill(iterator->process_pid,atoi(s[2]));
                        return;
                }
                iterator = iterator->next_process;
        }
        //exit(0);
        return;
}

void bg_builtin(char ** s,int number_of_arguments){
        if(number_of_arguments > 2) {
                jobs_error_handler(0);
                return;
        }
        else if(number_of_arguments < 2) {
                jobs_error_handler(1);
                return;
        }
        process * iterator = background_process;
        while(iterator) {
                if(iterator->process_number == atoi(s[1])) {
                        kill(iterator->process_pid,SIGCONT);
                        //exit(0);
                        return;
                }
                iterator = iterator->next_process;
        }
        printf("No such background process\n");
        //exit(0);
        return;
}

void fg_builtin(char **arguments,int number_of_arguments){
        if(number_of_arguments > 2) {
                jobs_error_handler(0);
                return;
        }
        else if(number_of_arguments < 2) {
                jobs_error_handler(1);
                return;
        }
        process *iterator = background_process, *prev = NULL;
        while(iterator != NULL) {
                if(iterator->process_number == atoi(arguments[1])) {
                        if(prev == NULL)
                                background_process = iterator->next_process;
                        else
                                prev->next_process = iterator->next_process;
                        break;
                }
                prev = iterator;
                iterator = iterator->next_process;
        }
        if(iterator != NULL) {
                kill(iterator->process_pid, SIGCONT);
                add_to_foreground(iterator->process_pid, iterator->command_name);
                free(iterator);
                tcsetpgrp(0, foreground_process->process_pid);
                siginfo_t fgStatus;
                waitid(P_PID,foreground_process->process_pid, &fgStatus, (WUNTRACED | WNOWAIT) );
        }
        else
                fprintf(stderr, "Error: No job with job number %d\n", atoi(arguments[1]));
}

void overkill_builtin(char ** s,int number_of_arguments){
        process * iterator = background_process;
        while(iterator) {
                kill(iterator->process_pid,SIGKILL);
                iterator = iterator->next_process;
        }
        //exit(0);
        return;
}
