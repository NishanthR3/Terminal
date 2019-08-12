#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void pinfo_builtin(char * token1,int number_of_arguments){
        char s2[1000],s3[1000],s4[1000];
        if(token1 != NULL) {
                strcpy(s3,"/proc/");
                strcat(s3,token1);
                strcat(s3,"/stat");
        }
        else sprintf(s3,"/proc/%d/stat",getpid());
        int fd = open(s3, O_RDONLY);
        if(fd < 0) {
                fprintf(stderr, "Failed to open file \"%s\"\n", s3);
                exit(1);
        }
        if(read(fd, s2, 1000) < 0) {
                fprintf(stderr, "Failed to read file \"%s\"\n",s3);
                exit(1);
        }
        char *token2 = strtok(s2," ");
        token2 = strtok(NULL," ");
        token2 = strtok(NULL," ");
        printf("Process State %s\n",token2);
        if(token1 != NULL) {
                strcpy(s3,"/proc/");
                strcat(s3,token1);
                strcat(s3,"/statm");
        }
        else sprintf(s3,"/proc/%d/statm",getpid());
        fd = open(s3, O_RDONLY);
        read(fd, s2, 1000);
        if(fd < 0) {
                fprintf(stderr, "Failed to open file \"%s\"\n", s3);
                exit(1);
        }
        if(read(fd, s2, 1000) < 0) {
                fprintf(stderr, "Failed to read file \"%s\"\n",s3);
                exit(1);
        }
        token2 = strtok(s2," ");
        printf("Virtual Memory %s\n",token2);
        if(token1 != NULL) {
                strcpy(s3,"/proc/");
                strcat(s3,token1);
                strcat(s3,"/exe");
        }
        else sprintf(s3,"/proc/%d/exe",getpid());
        readlink(s3, s4, 1000);
        printf("%s\n",s4);
        exit(0);
}
