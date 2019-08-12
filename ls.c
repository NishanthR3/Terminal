#include "main.h"
#include "commands.h"
#include <stdio.h>
#include <pwd.h>
#include <dirent.h>
#include <unistd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>

void ls_func(char * directory_name,char * s1){
        char s[1000];
        if(directory_name != NULL)
                sprintf(s,"%s/%s",directory_name,s1);
        else
                sprintf(s,"%s",s1);
        struct stat buffer;
        if (stat(s,&buffer) == 0)
                if(S_ISDIR(buffer.st_mode)) printf("d");
                else if(S_ISLNK(buffer.st_mode)) printf("s");
                else if(S_ISREG(buffer.st_mode)) printf("-");
        //printf("%u\n",buffer.st_blocks);
        (buffer.st_mode & S_IRUSR) ? printf("r") : printf("-");
        (buffer.st_mode & S_IWUSR) ? printf("w") : printf("-");
        (buffer.st_mode & S_IXUSR) ? printf("x") : printf("-");
        (buffer.st_mode & S_IRGRP) ? printf("r") : printf("-");
        (buffer.st_mode & S_IWGRP) ? printf("w") : printf("-");
        (buffer.st_mode & S_IXGRP) ? printf("x") : printf("-");
        (buffer.st_mode & S_IROTH) ? printf("r") : printf("-");
        (buffer.st_mode & S_IWOTH) ? printf("w") : printf("-");
        (buffer.st_mode & S_IXOTH) ? printf("x") : printf("-");
        printf(" %lu",buffer.st_nlink);
        struct passwd *pw = getpwuid(buffer.st_uid);
        printf(" %s",pw->pw_name);
        struct group *gr = getgrgid(buffer.st_gid);
        printf(" %s",gr->gr_name);
        printf(" %9jd",(intmax_t)buffer.st_size);
        struct tm *tm;
        tm = localtime(&buffer.st_mtime);
        char date[201];
        strftime(date, sizeof(date), nl_langinfo(D_T_FMT), tm);
        printf(" %s ",date);
        printf(" %s",s1);
        printf("\n");
}

void ls_builtin(char ** s,int number_of_arguments){
        DIR *dir;
        struct dirent * contents;
        if (s[1] == NULL) {
                dir = opendir(present_working_directory);
                while((contents = readdir(dir)) != NULL )
                        if(contents->d_name[0] != '.')
                                printf("%s\n",contents->d_name);
        }
        else if((strcmp(s[1],"-la") == 0)||(strcmp(s[1],"-al") == 0)) {
                if(s[2] == NULL) {
                        dir = opendir(present_working_directory);
                        while((contents = readdir(dir)) != NULL ) ls_func(NULL,contents->d_name);
                }
                else{
                        int i = 2;
                        while(s[i] != NULL) {
                                struct stat buffer;
                                if ((stat(s[i],&buffer) == 0)&&(S_ISDIR(buffer.st_mode))) {
                                        dir = opendir(s[i]);
                                        printf("%s:\n",s[i]);
                                        while((contents = readdir(dir)) != NULL )
                                                ls_func(s[i],contents->d_name);
                                }
                                else if(stat(s[i],&buffer) == 0) ls_func(NULL,s[i]);
                                else perror("shell");
                                i++;
                        }
                }
        }
        else if(((s[2] != NULL) && (strcmp(s[1],"-l") == 0) && (strcmp(s[2],"-a") == 0))
                ||((s[2] != NULL) && (strcmp(s[1],"-a") == 0) && (strcmp(s[2],"-l") == 0))) {
                if(s[3] == NULL) {
                        dir = opendir(present_working_directory);
                        while((contents = readdir(dir)) != NULL ) ls_func(NULL,contents->d_name);
                }
                else{
                        int i = 3;
                        while(s[i] != NULL) {
                                struct stat buffer;
                                if ((stat(s[i],&buffer) == 0)&&(S_ISDIR(buffer.st_mode))) {
                                        dir = opendir(s[i]);
                                        printf("%s:\n",s[i]);
                                        while((contents = readdir(dir)) != NULL )
                                                ls_func(s[i],contents->d_name);
                                }
                                else if(stat(s[i],&buffer) == 0) ls_func(NULL,s[i]);
                                else perror("shell");
                                i++;
                        }
                }

        }
        else if(strcmp(s[1],"-a") == 0) {
                if(s[2] == NULL) {
                        dir = opendir(present_working_directory);
                        while((contents = readdir(dir)) != NULL )
                                printf("%s\n",contents->d_name);
                }
                else{
                        int i = 2;
                        while(s[i] != NULL) {
                                struct stat buffer;
                                if ((stat(s[i],&buffer) == 0)&&(S_ISDIR(buffer.st_mode))) {
                                        dir = opendir(s[i]);
                                        printf("%s:\n",s[i]);
                                        while((contents = readdir(dir)) != NULL )
                                                printf("%s\n",contents->d_name);
                                }
                                else if(stat(s[i],&buffer) == 0) ls_func(NULL,s[i]);
                                else perror("shell");
                                i++;
                        }
                }
        }
        else if(strcmp(s[1],"-l") == 0) {
                if(s[2] == NULL) {
                        dir = opendir(present_working_directory);
                        while((contents = readdir(dir)) != NULL )
                                if(contents->d_name[0] != '.')
                                        ls_func(NULL,contents->d_name);
                }
                else{
                        int i = 2;
                        while(s[i] != NULL) {
                                struct stat buffer;
                                if ((stat(s[i],&buffer) == 0)&&(S_ISDIR(buffer.st_mode))) {
                                        dir = opendir(s[i]);
                                        printf("%s:\n",s[i]);
                                        while((contents = readdir(dir)) != NULL )
                                                if(contents->d_name[0] != '.') ls_func(s[i],contents->d_name);
                                }
                                else if(stat(s[i],&buffer) == 0) ls_func(NULL,s[i]);
                                else perror("shell");
                                i++;
                        }
                }
        }
        else{
                int i = 1;
                while(s[i] != NULL) {
                        struct stat buffer;
                        if ((stat(s[i],&buffer) == 0)&&(S_ISDIR(buffer.st_mode))) {
                                dir = opendir(s[i]);
                                printf("%s:\n",s[i]);
                                while((contents = readdir(dir)) != NULL )
                                        if(contents->d_name[0] != '.')
                                                printf("%s\n",contents->d_name);
                                printf("\n");
                        }
                        else if(stat(s[i],&buffer) == 0) printf("%s\n",s[i]);
                        else perror("shell");
                        i++;
                }
        }
        closedir(dir);
        exit(0);
}
