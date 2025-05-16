#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Zmienna globalna
int global = 0;

int main(int argc, char *argv[]) {
    printf("Program name: %s\n", argv[0]);

    if (argc != 2) {
        return 1;
    }

    int local = 0;

    // procesy potomny
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 2;
    } else if (pid == 0) {
        //potomny
        printf("child process\n");

        global++;
        local++;

        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);

        execl("/bin/ls", "ls", argv[1], (char *)NULL);

        // Jeśli błąd
        perror("execl failed");
        // return 3;
    } else {
        // rodzic
        int status;
        local=4;
        global = 2;
        printf("parent process\n");

        waitpid(pid, &status, 0);

        printf("parent pid = %d, child pid = %d\n", getpid(), pid);

        if (WIFEXITED(status)) {
            printf("Child exit code: %d\n", WEXITSTATUS(status));
        } else {
            printf("error\n");
        }

        printf("Parent's local = %d, parent's global = %d\n", local, global);

        // return 0;
    }

    printf("KONIEC");
}
