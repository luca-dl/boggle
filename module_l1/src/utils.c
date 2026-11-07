#include <string.h>
#include <stdio.h>
#include "utils.h"

/* Renvoie le nom de l'auteur du projet */
int dec_auteur(int argc, char* argv[]){
    /* Renvoie ERR_ARGS et affiche le nom de l'auteur */
    if (argc==2 && strcmp("author", argv[1])==0){
      printf("Auteur: DANTEC LEAL Luca\n");
      return ERR_ARGS;
    }
    /* Sinon retourne OK */
    return OK;
}

/* Renvoie l'entier demandé s'il est entre 1 et 100 */ 
int lire_entier(const char* nb) {
    int val = 0;
    if ( nb[0] == '\0' ) return -1;
    for ( int i = 0 ; nb[i] != '\0' ; i++ ){
        /* Vérifie si c'est un chiffre ou non */
        if ( nb[i] < '0' || nb[i] > '9' ) return -1;
        /* Convertie les char en int */
        val = val * 10 + ( nb[i] - '0');
        if ( val > 100 ) return -1;
    }
    /* Renvoie la valeur si ce n'est pas 0 */
    if ( val == 0 ) return -1;
    return val;
}