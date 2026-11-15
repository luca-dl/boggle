#include <stdio.h>
#include "utils.h"
#include "dictionnary.h"

int main(int argc, char* argv[]) {
    Arbre arbre;

    dec_auteur(argc, argv);

    if (argc != 3) {
        return ERR_ARGS;
    }

    /* Vérification d'une possible érreur mémoire */
    if (arbre_init(&arbre) == 0) {
        return ERR_MEM;
    }

    /* Lecture du dico texte */
    if (dico_sort(argv[1], &arbre) < 0) {
        arbre_free(&arbre);
        return ERR_FI;
    }

    /* Écriture du .lex */
    if (lex_write(&arbre, argv[2]) == 0) {
        arbre_free(&arbre);
        return ERR_FO;
    }
    /* Libération de la mémoire */
    arbre_free(&arbre);
    return OK;
}