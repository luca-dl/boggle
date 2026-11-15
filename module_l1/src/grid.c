#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"


/* Remplit la grille avec les cases passées en argument et renvoie 1 si succès, 0 si une case est invalide. */
int grid_remplir(Grille* g, int lignes, int colonnes, char* cases[]) {
    if (g == NULL || cases == NULL ||
        lignes <= 0 || colonnes <= 0 ||
        lignes * colonnes > 16) {
        return 0;
    }
    /* Alloue la mémoire et initialise les dimensions. */
    if (!grid_init(g, lignes, colonnes)) {
        return 0;
    }
    for (int i = 0; i < lignes * colonnes; i++) {
        const char* c = cases[i];
        /* Toujours vérifier le pointeur avant d'accéder à c[0]. */
        if (c == NULL) {
            grid_free(g);
            return 0;
        }
        /* Case valide : une lettre A-Z seule, ou exactement "QU". */
        int lettre = (
            c[0] >= 'A' &&
            c[0] <= 'Z' &&
            c[1] == '\0'
        );
        int qu = (strcmp(c, "QU") == 0);

        if (!lettre && !qu) {
            grid_free(g);
            return 0;
        }
        strcpy(g->cases[i], c);
    }
    return 1;
}

/* Détecte les voisins d'une lettre avec un indice */
int grid_voisins(const Grille* g, int i, int voisins[8]) {
    if (g == NULL || voisins == NULL) {
        return 0;
    }

    int total = g->lignes * g->colonnes;

    /* Vérifier que l’indice correspond à une case existante. */
    if (i < 0 || i >= total) {
        return 0;
    }

    /* Conversion de l’indice en ligne et colonne. */
    int ligne = i / g->colonnes;
    int colonne = i % g->colonnes;
    int nb = 0;

    /* Parcours des huit déplacements possibles. */
    for (int idx_l = -1; idx_l <= 1; idx_l++) {
        for (int idx_c = -1; idx_c <= 1; idx_c++) {
            /* Ne pas ajouter la case elle-même. */
            if (idx_l == 0 && idx_c == 0) {
                continue;
            }

            int nw_l = ligne + idx_l;
            int nw_c = colonne + idx_c;

            /* Vérifier que le voisin reste dans la grille. */
            if (nw_l >= 0 && nw_l < g->lignes
                && nw_c >= 0 && nw_c < g->colonnes) {
                voisins[nb] = nw_l * g->colonnes + nw_c;
                nb++;
            }
        }
    }

    return nb;
}


int grid_init(Grille* g, int lignes, int colonnes) {
    if (g == NULL || lignes <= 0 || colonnes <= 0) {
        return 0;
    }

    g->lignes = lignes;
    g->colonnes = colonnes;

    int nb_cases = lignes * colonnes;

    g->cases = malloc(nb_cases * sizeof(*g->cases));

    if (g->cases == NULL) {
        g->lignes = 0;
        g->colonnes = 0;
        return 0;
    }

    return 1;
}

/* Vide une grille */
void grid_free(Grille* g) {
    if (g == NULL) {
        return;
    }

    free(g->cases);

    g->cases = NULL;
    g->lignes = 0;
    g->colonnes = 0;
}

/* Vérifie si le mot est possible dans la grille */
static int find_word(const Grille* g, const char* mot, int position_mot, int indice_case, int utilisees[], int chemin[], int profondeur, int* longueur_chemin) {
    /* Refuse une case déjà utilisée dans le chemin en cours. */
    if (utilisees[indice_case]) {
        return 0;
    }

    /* Détermine si la case contient une lettre normale ou un QU */
    const char* contenu = g->cases[indice_case];
    int taille_case = (contenu[1] == '\0') ? 1 : 2;

    /* Vérifie si la case actuelle est au bon endroit dans le mot */
    for (int i = 0; i < taille_case; i++) {
        if (mot[position_mot + i] == '\0' || mot[position_mot + i] != contenu[i]) {
            return 0;
        }
    }
    /* Si la case correspond, on la marque et on l'ajoute au chemin. */
    utilisees[indice_case] = 1;
    chemin[profondeur] = indice_case;

    /* On décale à la prochaine lettre */
    int prochaine_position = position_mot + taille_case;

    /* Si le mot est terminé */
    if (mot[prochaine_position] == '\0') {
        *longueur_chemin = profondeur + 1;
        return 1;
    }

    /* Récupère les voisins */
    int voisins[8];
    int nb_voisins = grid_voisins(g, indice_case, voisins);

    /* On regarde chaque voisin et si la lettre est bonne alors la recursivité agit */
    for (int i = 0; i < nb_voisins; i++) {
        if (find_word(g, mot, prochaine_position, voisins[i], utilisees, chemin, profondeur + 1, longueur_chemin)) {
            return 1;
        }
    }

    /* Si la case n emène à rien alors on la rend, c'est à dire qu'on la laisse pour possiblement un autre moment dans le mot */
    utilisees[indice_case] = 0;
    return 0;
}

int grid_find_path(const Grille* g,const char* mot,int chemin[],int* longueur_chemin) {
    /* Vérfie si tout est valide */
    if (g == NULL || g->cases == NULL || mot == NULL || mot[0] == '\0' || chemin == NULL || longueur_chemin == NULL) {
        return 0;
    }

    int nb_cases = g->lignes * g->colonnes;
    int* utilisees = calloc((size_t)nb_cases, sizeof(int));

    if (utilisees == NULL) {
        return 0;
    }

    *longueur_chemin = 0;

    for (int i = 0; i < nb_cases; i++) {
        if (find_word(g,mot,0,i,utilisees,chemin, 0,longueur_chemin)) {
            free(utilisees);
            return 1;
        }
    }

    free(utilisees);
    return 0;
}
