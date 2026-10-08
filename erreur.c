#define    _POSIX_C_SOURCE    200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>  // open

#include "erreur.h"

void erreur(const char *mess, int valExit) {
  // printf("errno = %d\n", errno);
  perror(mess);
  exit(valExit);
}
