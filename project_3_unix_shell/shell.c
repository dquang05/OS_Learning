#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void) {
    char *args[MAX_LINE / 2 + 1]; /* Command line arguments */
    char input[MAX_LINE];
    char history[MAX_LINE] = "";
    int should_run = 1; /* Flag to determine when to exit program */

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        /* Read user input */
        if (fgets(input, MAX_LINE, stdin) == NULL) {
            continue;
        }

        /* Remove trailing newline character */
        input[strcspn(input, "\r\n")] = 0;

        /* Handle exit command */
        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }

        /* Ignore empty input */
        if (strlen(input) == 0) {
            continue;
        }

        /* Handle history command (!!) */
        if (strcmp(input, "!!") == 0) {
            if (strlen(history) == 0) {
                printf("No commands in history.\n");
                continue;
            }
            printf("%s\n", history);
            strcpy(input, history);
        } else {
            /* Save command to history */
            strcpy(history, input);
        }

        /* Parse input into arguments */
        int i = 0;
        char *token = strtok(input, " ");
        int background = 0;
        char *redirect_in = NULL;
        char *redirect_out = NULL;
        int pipe_idx = -1;

        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                background = 1;
            } else if (strcmp(token, "<") == 0) {
                token = strtok(NULL, " ");
                redirect_in = token;
            } else if (strcmp(token, ">") == 0) {
                token = strtok(NULL, " ");
                redirect_out = token;
            } else if (strcmp(token, "|") == 0) {
                pipe_idx = i;
                args[i++] = NULL; /* Split args array for pipe */
            } else {
                args[i++] = token;
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL; /* Null-terminate the argument list */

        if (args[0] == NULL) {
            continue;
        }

        /* Fork a child process */
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) {
            /* Child process */

            /* Handle Input Redirection */
            if (redirect_in != NULL) {
                int fd_in = open(redirect_in, O_RDONLY);
                if (fd_in < 0) {
                    perror("Error opening input file");
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            /* Handle Output Redirection */
            if (redirect_out != NULL) {
                int fd_out = open(redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    perror("Error opening output file");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            /* Handle Pipe */
            if (pipe_idx != -1) {
                int fd[2];
                if (pipe(fd) == -1) {
                    perror("Pipe failed");
                    exit(1);
                }

                pid_t pid2 = fork();
                if (pid2 < 0) {
                    perror("Fork 2 failed");
                    exit(1);
                }

                if (pid2 == 0) {
                    /* Grandchild process - writes to the pipe */
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[0]);
                    close(fd[1]);
                    
                    /* Execute left side of the pipe */
                    execvp(args[0], args);
                    perror("Command execution failed");
                    exit(1);
                } else {
                    /* Child process - reads from the pipe */
                    dup2(fd[0], STDIN_FILENO);
                    close(fd[0]);
                    close(fd[1]);
                    waitpid(pid2, NULL, 0); /* Wait for grandchild to finish writing */
                    
                    /* Execute right side of the pipe */
                    execvp(args[pipe_idx + 1], &args[pipe_idx + 1]);
                    perror("Command execution failed");
                    exit(1);
                }
            } else {
                /* Normal execution (no pipe) */
                execvp(args[0], args);
                perror("Command execution failed");
                exit(1);
            }
        } else {
            /* Parent process */
            if (!background) {
                waitpid(pid, NULL, 0);
            }
        }
    }
    return 0;
}