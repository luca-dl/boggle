#include <stdio.h>
#include "utils.h"
#include "dictionnary.h"

/* dictionnary_lookup dico.lex MOT
   Code de sortie : 0 mot présent, 1 préfixe seulement, 2 absent. */
int main(int argc, char* argv[]) {
    dec_auteur(argc, argv);

    /* Exactement 2 arguments : le .lex et le mot */
    if (argc != 3) {
        return ERR_ARGS;
    }

    /* Chargement du .lex (pas besoin d'arbre_init : lex_lire remplit tout) */
    Arbre arbre;
    if (lex_read(&arbre, argv[1]) == 0) {
        return ERR_FI;   /* pas de free : lex_lire ne touche pas l'arbre en cas d'échec */
    }

    /* Recherche : le résultat 0/1/2 est directement le code de sortie */
    int r = arbre_find(&arbre, argv[2]);

    arbre_free(&arbre);
    return r;
}
