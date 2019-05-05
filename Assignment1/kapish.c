#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

void signalHandler(int sig_num){
    printf("Parent cannot be terminated, please input your command again\n");
}

void remove_new_line(char *cmd) {
    while (*cmd) {
        if (*cmd == '\n') {
            *cmd='\0';
        }
        cmd++;
    }
}

void read_line(char *input){\
    int i;
    remove_new_line(input);
    char *inputToken[4];
    inputToken[0] = strtok(input, " ");
    i = 0;
    while (inputToken[i] != NULL) {
        inputToken[i + 1] = strtok(NULL, " ");
        i++;

    }
    if (strcmp(inputToken[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(inputToken[0], "cd") == 0) {
        char *dir;
        if (inputToken[1] == NULL || strcmp(inputToken[1], "~") == 0) {
            dir = getenv("HOME");
        } else {
            dir = inputToken[1];
        }
        int cd = chdir(dir);
        if (cd == -1) {
            perror(inputToken[0]);
        }
    } else if (strcmp(inputToken[0], "setenv") == 0) {
        if (inputToken[1] == NULL) {
            printf("Missing Variable Name");
        } else {
            if (inputToken[2] == NULL) {
                inputToken[2] = "\0";
            }
            int envSet = setenv(inputToken[1], inputToken[2], 1);
            if (envSet == -1) {
                perror(inputToken[0]);
            }
        }
    } else if (strcmp(inputToken[0], "unsetenv") == 0) {
        if (inputToken[1] == NULL) {
            printf("Missing Variable Name");
        } else {
            int envUnset = unsetenv(inputToken[1]);
            if (envUnset == -1) {
                perror(inputToken[0]);
            }
        }
    } else {
        pid_t child= fork();
        if (child == 0) {
            execvp(inputToken[0], &inputToken[0]);
	    perror(inputToken[0]);
        }else if(child > 0){
	    signal(SIGINT, signalHandler);
            waitpid(child,NULL,0);
        }
	signal(SIGINT, signalHandler);
    }
}

int main() {
    char *fileName = "/.kapishrc";
    char *path = getenv("HOME");
    char* pathName = malloc(strlen(fileName)+strlen(path));
    strcpy(pathName, getenv("HOME"));
    strcat(pathName, fileName);
    fflush(NULL);
    FILE* file = fopen(pathName, "r");
    if(file == NULL){
        printf("File not found\n");
    }else {
        char line[1024];
        while (fgets(line, sizeof(line), file)) {
            printf("? ");
            printf("%s", line);
            read_line(line);
        }
        fclose(file);
    }
    signal(SIGINT, signalHandler);
    free(pathName);
    for(;;) {
	char *input = (char*)malloc(512);
        printf("? ");
        fflush(NULL);
        fgets(input, 512, stdin);
        if(feof(stdin)){
            exit(0);
        }
        read_line(input);
        free(input);
    }
    exit(0);
}
