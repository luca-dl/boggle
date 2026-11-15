#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    /* Déclaration générale */
    Grille g;
    long L;
    long C;
    int n;
    int trouve;
    int longueur = 0;
    int* chemin = NULL;

    /* Vérifie qu'il y a au moins MOT, L et C avant de lire L et C. */
    if (argc < 4) {
        fprintf(stderr, "Indication : grid_path MOT L C cases...\n");
        return ERR_ARGS;
    }

    /* Convertit L et C en entiers. */
    L = strtol(argv[2], NULL, 10);
    C = strtol(argv[3], NULL, 10);

    /* Refuse des dimensions hors de la plage 1 à 100. */
    if (L < 1 || L > 4 || C < 1 || C > 4) {
        fprintf(stderr, "Erreur : L et C doivent être entre 1 et 4.\n");
        return ERR_ARGS;
    }

    /* Vérifie que le nombre d'arguments correspond exactement à L * C cases. */
    if (argc != 4 + L * C) {
        fprintf(stderr, "Erreur : %ld cases attendues.\n", L * C);
        return ERR_ARGS;
    }

    /* Refuse un mot vide. */
    if (argv[1][0] == '\0') {
        fprintf(stderr, "Erreur : le mot est vide.\n");
        return ERR_ARGS;
    }

    /* Calcule le nombre total de cases. */
    n = (int)(L * C);

    /* Construit la grille à partir des cases passées en argument, à partir de argv[4]. */
    grid_remplir(&g, (int)L, (int)C, argv + 4);

    /* Alloue le tableau qui recevra les indices du chemin. */
    chemin = malloc(n * sizeof(int));
    if (chemin == NULL) {
        grid_free(&g);
        return ERR_MEM;
    }

    /* Lance la recherche du mot dans la grille. */
    trouve = grid_find_path(&g, argv[1], chemin, &longueur);

    /* Si le mot est trouvé, affiche les indices du chemin séparés par des espaces. */
    if (trouve == 1) {
        for (int i = 0; i < longueur; i++) {
            printf("%d", chemin[i]);
            if (i < longueur - 1) {
                printf(" ");
            }
        }
        printf("\n");
    }

    /* Libère le chemin et la grille avant de sortir. */
    free(chemin);
    grid_free(&g);

    /* Renvoie 0 si le mot est trouvé, 1 sinon. */
    return (trouve == 1) ? 0 : 1;
}