#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t gagnant = 0;

void f1(void){
    sleep(5);
}

void f2(void){
    sleep(3);
}

void handler(int sig){
    if (gagnant == 0){
        gagnant = sig;
    }
}

void fils(void (*algo), int sigSuccess, int numero, sigset_t *masqueOriginal){
    if (sigprocmask(SIG_SETMASK, masqueOriginal, NULL) < 0) {
        perror("sigprocmask fils");
        exit(1);
    }

    algo();
    printf("Fils %d (%d) : calcul termine\n", numero, getpid());
    
    if (kill(getppid(), sigSuccess) < 0) {
        perror("kill fils");
        exit(1);
    }

    printf("Fils %d (%d) : je termine mon execution", numero, getpid());
    exit(0);
}

int main(void){
    struct sigaction action;
    sigset_t masqueBloque, masqueOriginal;
    pid_t pid1, pid2, perdant;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGUSR1, &action, NULL) < 0 || 
        sigaction(SIGUSR2, &action, NULL) < 0) {
            perror("sigaction");
            exit(1);
    }

    sigemptyset(&masqueBloque);
    sigaddset(&masqueBloque, SIGUSR1);
    sigaddset(&masqueBloque, SIGUSR2);
    if sigprocmask(SIG_BLOCK, &masqueBloque, &masqueOriginal) < 0 {
        perror("sigprocmask");
        exit(1);
    }
    
    switch(pid1 = fork()) {
        case -1: perror("fork 1"); exit(1);
        case 0: fils(f1, SIGUSR1, 1, &masqueOriginal);
    }

    switch(pid2 = fork()) {
        case -1: perror("fork 2"); exit(1);
        case 0: fils(f2, SIGUSR2, 2, &masqueOriginal);
    }

    while(gagnant == 0)
        sigsuspend(&masqueOriginal);

    printf("Pere : le fils %d a fini le premier\n", gagnant == SIGUSR1 ? 1 : 2);

    perdant = (gagnant == SIGUSR1) ? pid2 : pid1;
    if (kill(perdant, SIGTERM) < 0)
        perror("kill perdant");

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    sigprocmask(SIG_SETMASK, &masqueOriginal, NULL);
    printf("Pere : je continue mon traitement\n");
    return 0;
}