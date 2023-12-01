#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // child process
        while (1)
        {
            printf("Hello World!\n");
            sleep(1);
        }
        return 0;
    }
    else if (pid > 0)
    {
        // parent process
        return 0;
    }
    else
    {
        // fork failed
        return -1;
    }
}
