#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "erreur.h"

char buffer[BUFSIZ];
int tube[2];
int nbLus;

void pere(){
    // orienter le tube: pere producteur
    close(tube[0]);

    //boucler lire au clavier, ecrire dans le tube
    // ce que a ete lu

    while((nbLus = read(0, buffer, BUFSIZ)) > 0){
        if (write(tube[1], buffer, nbLus) != nbLus){
            erreur("pb write", 3);
        }
    }
    close(tube[1]);
    // attendre la fin du fils
    wait(NULL);
}

void fils(){
    // orienter le tube: fils consommateur
    close(tube[1]);
    
    // boucler; lire dans le pipe, ecrire ce que a ete lu sur stdout
    while((nbLus = read(tube[0], buffer, BUFSIZ)) > 0){
        if (write(1, buffer, nbLus) != nbLus){
            erreur("pb write stdout", 4);
        } 
    }
    close(tube[0]); // par critique car exit apres
    exit(0);
}

int main(void){
    // creer le pipe
    if (pipe(tube) == -1){
        erreur("echec pipe", 1);
    }

    // creer le fils
    switch(fork()){
        case -1:
            erreur("echec fork", 2);
        case 0: fils(); // no return
        default:
            break;
    }
    pere();
    return 0;

}

