#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "erreur.h"

#define N 3

int tube1[2];   /* pere -> fils */
int tube2[2];   /* fils -> pere */
char buffer[BUFSIZ];
int nbLus;

void pere(void){
    int i;

    close(tube1[0]);    /* pere n'ecrit que dans tube1 */
    close(tube2[1]);    /* pere ne lit que dans tube2  */

    for (i = 0; i < N; i++){
        /* envoyer ping */
        if (write(tube1[1], "ping", 5) != 5)      /* 5 = "ping" + '\0' */
            erreur("pb write ping", 3);

        /* lire la reponse pong */
        nbLus = read(tube2[0], buffer, BUFSIZ);
        if (nbLus <= 0)
            erreur("pb read pong", 4);
        printf("Pere a recu : %s\n", buffer);
    }

    close(tube1[1]);    /* EOF pour le fils */
    close(tube2[0]);
    wait(NULL);
}

void fils(void){
    close(tube1[1]);    /* fils ne lit que dans tube1  */
    close(tube2[0]);    /* fils n'ecrit que dans tube2 */

    /* boucle jusqu'a EOF : le fils ne connait pas N */
    while ((nbLus = read(tube1[0], buffer, BUFSIZ)) > 0){
        printf("Fils a recu : %s\n", buffer);
        if (write(tube2[1], "pong", 5) != 5)
            erreur("pb write pong", 5);
    }

    close(tube1[0]);
    close(tube2[1]);
    exit(0);
}

int main(void){
    if (pipe(tube1) == -1) erreur("echec pipe1", 1);
    if (pipe(tube2) == -1) erreur("echec pipe2", 1);

    switch (fork()){
        case -1:
            erreur("echec fork", 2);
        case 0:
            fils();         /* no return */
        default:
            break;
    }
    pere();
    return 0;
}