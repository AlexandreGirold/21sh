#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void parse_args(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <file> <command> [args...]\n", argv[0]);
        exit(2);
    }
}

static void redirect_output(const char *filename)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }
    if (dup2(fd, STDOUT_FILENO) < 0)
    {
        perror("dup2");
        close(fd);
        exit(1);
    }
    close(fd);
}

static void execute_command(char *argv[])
{
    execvp(argv[0], argv);
    fprintf(stderr, "%s: command not found\n", argv[0]);
    exit(127);
}

static void handle_child_process(char *argv[])
{
    redirect_output(argv[1]);
    execute_command(&argv[2]);
}

static void handle_parent_process(pid_t pid, char *command)
{
    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("waitpid");
        exit(1);
    }
    if (WIFEXITED(status))
    {
        int exit_status = WEXITSTATUS(status);
        if (exit_status == 127)
        {
            exit(1);
        }
        else
        {
            printf("%s exited with %d!\n", command, exit_status);
            exit(0);
        }
    }
    else
    {
        fprintf(stderr, "Child process did not terminate normally\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        handle_child_process(argv);
    }
    else
    {
        handle_parent_process(pid, argv[2]);
    }

    return 0;
}
