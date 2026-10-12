#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t compteur = 0;

void traiterSigint(int sig) {
    compteur++;
}

int main(void){
    struct sigaction action;

    action.sa_handler = traiterSigint;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, NULL) < 0){
        perror("sigaction");
        exit(1);
    }

    int dernierAffiche = 0;

    while(compteur < 3) {
        if (compteur != dernierAffiche){
            dernierAffiche = compteur;
            printf("\nSIGINT recu (%d/3), je continue a boucler\n", compteur);
        }
    }

    printf("3 SIGINT recus : je meurs! \n");
    return 0;
}