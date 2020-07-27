#include "main.h"
#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void convertCWD(char * home1,char * cwd){
        char ans[201];
        int i,j = 0;
        ans[0] = '~';
        if(strcmp(home1,cwd) == 0) {
                cwd[0] = '~';
                cwd[1] = '\0';
                return;
        }
        for(i = 0; home1[i] != '\0'; i++) if(home1[i] != cwd[i]) return;
        for(; cwd[i+j] != '\0'; j++) ans[j+1] = cwd[i+j];
        for(i = 0; cwd[i] != '\0'; i++) cwd[i] = ans[i];
        cwd[i+1] = '\0';
        return;
}

void prompt(char * home,char * directory){
        getcwd(directory,200);
        convertCWD(home,directory);
}

/*
   void sigintHandler(int sig_num){
        prompt(home,directory);
        printf("\n"termi_green "<%s"termi_red "@%s:"termi_blue "%s> "termi_reset,user,hostname,directory);
        fflush(stdout);
        signal(SIGINT, sigintHandler);
   }*/

int main(){
        /*signal(SIGINT, SIG_IGN);
           signal(SIGCHLD,childHandler);*/
        jobControl();
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        getcwd(home,200);
        gethostname(hostname,201);
        sprintf(user,"%s",pw->pw_name);

        if (pw == NULL) {
                printf("Failed\n");
                exit(1);
        }
        while(1) {
                prompt(home,directory);
                printf(termi_green "<%s"termi_red "@%s:"termi_blue "%s> "termi_reset,user,hostname,directory);
                fflush(stdout);
                char * input = getInput();
                char ** mul_args = format(input,";");
                for(int i = 0; mul_args[i] != NULL; i++) {
                        getcwd(present_working_directory,200);
                        runCommand(mul_args[i]);
                        /*for (int j = 0; mul_args[j] != NULL; j++) {
                                char ** args = format(mul_args[i]," \t\n");
                                int size = 0;
                                while(args[size] != NULL) size++;
                                if(size == 0) continue;
                                if(strcmp(args[size-1],"&") == 0) {
                                        args[size-1] = NULL;
                                        fork_exec(args,1);
                                }
                                else if(args[size-1][strlen(args[size-1])-1] == '&') {
                                        args[size-1][strlen(args[size-1])-1] = '\0';
                                        fork_exec(args,1);
                                }
                                else fork_exec(args,0);
                           }*/
                }
        }

        return 0;
}
