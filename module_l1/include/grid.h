#ifndef GRID_H
#define GRID_H

typedef struct {
    int lignes;
    int colonnes;
    char (*cases)[3];
} Grille;


int grid_remplir(Grille* g, int lignes, int colonnes, char* cases[]);

int grid_voisins(const Grille* g, int i, int voisins[8]);

int grid_init(Grille* g, int lignes, int colonnes);

void grid_free(Grille* g);

int grid_find_path(const Grille* g,const char* mot,int chemin[],int* longueur_chemin);

#endif
