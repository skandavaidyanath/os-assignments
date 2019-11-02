#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void sigintHandler(int sigNum)
{
    return;
}

int verifyCommand(char* command) {
    return (strcmp(command, "cd") == 0) ||
        (strcmp(command, "echo") == 0) ||
        (strcmp(command, "ls") == 0) ||
        (strcmp(command, "wc") == 0) ||
        (strcmp(command, "quit") == 0);
}

int main(void)
{
    // handle ctrl+c
    signal(SIGINT, sigintHandler);

    char line[2048];
    const char delim[2] = " ";
    while (1)
    {
        int pfd[2];
        pipe(pfd);
        pid_t pid = fork();

        // child process
        if (pid == 0)
        {
            printf("> ");
            scanf("%2047[^\n]s", line);

            // tokenize the line
            char *tokensArr[10];
            char *token = strtok(line, delim);
            int i;
            for (i = 0; token != NULL; i++)
            {
                tokensArr[i] = token;
                token = strtok(NULL, delim);
            }
            tokensArr[i] = NULL;

            // verify command
            if (!verifyCommand(tokensArr[0])) {
                printf("not a valid command. only cd, ls, echo, wc, quit are supported\n");
                exit(0);
            }

            // special case - "quit"
            if (strcmp(tokensArr[0], "quit") == 0)
                // exit with a special code so parent knows to exit
                exit(49);

            // special case - "cd"
            if (strcmp(tokensArr[0], "cd") == 0)
            {
                // if cd has no argument
                if (tokensArr[1] == NULL)
                {
                    system("pwd");
                    exit(0);
                }

                // if cd has an argument i.e. if a directory is given
                else
                {
                    // child process must write data
                    // close the read-descriptor
                    close(pfd[0]);

                    // write the data
                    write(pfd[1], tokensArr[1], 100);

                    // close the write-descriptor
                    close(pfd[1]);

                    // exit with special code so parent knows to read data
                    exit(50);
                }
            }

            // execute any other command normally
            else
            {
                if (execvp(tokensArr[0], tokensArr) < 0)
                    perror("");
                exit(0);
            }
        }

        // parent process
        else
        {
            int status;
            wait(&status);

            // the returned status is encoded in a special way, so decode it
            status /= 255;

            // "quit" was entered
            if (status == 49)
            {
                printf("bye!\n");
                exit(0);
            }

            // "cd" command must be executed by parent process
            else if (status == 50)
            {
                // parent process must read data
                char directory[100];

                // close the write-descriptor
                close(pfd[1]);

                // read the data
                read(pfd[0], directory, 100);

                // change the working directory
                if (chdir(directory) < 0)
                    perror("cd");

                // close the read-descriptor
                close(pfd[0]);
            }
        }
    }
}