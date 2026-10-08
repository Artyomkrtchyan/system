#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 3

int lesPids[N];

void afficherLesPids(int *tab) {
    for (int i = 0; i < N; i++)
        printf("%5d", tab[i]);
    printf("\n");
}

void maFonction(int monNum) {
    printf("Processus fils %d (%d) : lesPid = ",
           monNum, getpid());
    afficherLesPids(lesPids);
}

int main(void) {

    for (int i = 0; i < N; i++)
        lesPids[i] = 0;

    printf("Processus pere (%d) debut : lesPid = ", getpid());
    afficherLesPids(lesPids);

    for (int i = 0; i < N; i++) {
        switch (lesPids[i] = fork()) {
            case -1 : perror("Echec fork : ");
                      exit(1);
            case 0  : maFonction(i);
                      exit(0);
            default : break;
        }
    }

    printf("Processus pere (%d) fin : lesPid = ", getpid());
    afficherLesPids(lesPids);

    return(0);
}