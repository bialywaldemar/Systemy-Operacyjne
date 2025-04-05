#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
#include <sys/types.h>
// #include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        return 1;
    }
    int n = atoi(argv[1]);
    for(int i = 0; i < n; i++){
        pid_t pid = fork();
        if (pid < 0){
            return 1;
        }

        else if (pid == 0){
            printf("Parent PID: %d, Process PID: %d\n", getppid(), getpid());
            return 0;
        }
    }
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("%d\n", n);
    return 0;

}