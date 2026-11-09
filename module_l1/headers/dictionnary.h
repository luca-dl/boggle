#ifndef DICTIONNARY_H
#define DICTIONNARY_H

#define NO_INDEX -1

typedef struct {
    char lettre;
    int fils;
    int frere;
    int fin_mot;
} Cellule;

typedef struct {
    Cellule* cellule;
    int nb_mots;
    int capacite;
    int nb_cellules;
} Arbre;

typedef struct {
    int taille_entete;
    int nb_mots;
    int nb_cellules;
    int taille_cellule;
} Entete;

int dico_sort(const char* chemin, Arbre* arbre);

int arbre_init(Arbre* arbre);

void arbre_free(Arbre* arbre);

int arbre_insert(Arbre* arbre, const char* mot);

int arbre_find(const Arbre* arbre, const char* mot);

int lex_write(const Arbre* arbre, const char* chemin);

int lex_read(Arbre* arbre, const char* chemin);

#endif