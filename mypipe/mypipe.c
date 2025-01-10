#include "mypipe.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int create_pipe(int pipefd[2])
{
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }
    return 0;
}

static pid_t fork_process(void)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
    }
    return pid;
}

static void execute_command_with_output(char **argv, int pipefd[2])
{
    // Redirect standard output to the write end of the pipe
    dup2(pipefd[1], STDOUT_FILENO);

    close(pipefd[0]);
    close(pipefd[1]);

    execvp(argv[0], argv);
    perror("execvp");
    exit(1);
}

static void execute_command_with_input(char **argv, int pipefd[2])
{
    // Redirect standard input to the read end of the pipe
    dup2(pipefd[0], STDIN_FILENO);

    close(pipefd[1]);
    close(pipefd[0]);

    execvp(argv[0], argv);
    perror("execvp");
    exit(1);
}

int exec_pipe(char **argv_left, char **argv_right)
{
    int pipefd[2];
    pid_t pid1;
    pid_t pid2;
    int status;

    if (create_pipe(pipefd) == 1)
    {
        return 1;
    }

    pid1 = fork_process();
    if (pid1 == -1)
    {
        return 1;
    }

    if (pid1 == 0)
    {
        execute_command_with_output(argv_left, pipefd);
    }

    pid2 = fork_process();
    if (pid2 == -1)
    {
        return 1;
    }

    if (pid2 == 0)
    {
        execute_command_with_input(argv_right, pipefd);
    }

    // Close both ends of the pipe in the parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both child processes to finish
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);

    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }

    return 1;
}
