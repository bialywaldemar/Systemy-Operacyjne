#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void sigusr1_handler(int signum) {
    printf("Odebrano sygnał SIGUSR1 (%d)\n", signum);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s\n", argv[0]);
        return 0;
    }
    char *arg = argv[1];

    if (strcmp(arg, "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    }

    else if (strcmp(arg, "handler") == 0) {
        signal(SIGUSR1, sigusr1_handler);
    }

    else if (strcmp(arg, "mask") == 0) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }
    }

    else if (strcmp(arg, "none") != 0) {
        fprintf(stderr, "Nieznana opcja: %s\n", arg);
        exit(EXIT_FAILURE);
    }

    printf("wysylanie SIGUSR1 do procesu (PID = %d)\n", getpid());
    raise(SIGUSR1);
    if(strcmp(arg, "mask") == 0) {
        sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("SIGUSR1 jest w zestawie oczekujących sygnałów\n");
        } else {
            printf("SIGUSR1 nie jest w zestawie oczekujących sygnałów\n");
        }
        
    }
    printf("konec\n");
    return 0;
}