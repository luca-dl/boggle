#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dictionnary.h"

/* Nettoie la ligne */
static int clean_line(char* ligne) {
    /* Vérifie si la ligne ne commence pas par un espace*/
    if (ligne[0] == ' ') return 0;

    /* Vérifie quand la ligne se termine et s'arrête ensuite */
    int i = 0;
    while (ligne[i] != '\0' && ligne[i] != ' ' && ligne[i] != '\r' && ligne[i] != '\n'){
        i++;
    }
    ligne[i] = '\0';

    /* Vérifie que la ligne contient quelque chose*/
    if (ligne[0] == '\0') return 0;

    /* Vérifie si les lettres sont en majuscules ou non */
    for (int j = 0; ligne[j] != '\0'; j++) {
        if ( ligne[j] < 'A' || ligne[j] > 'Z') return 0;
    }

    return 1;
}

/* Lit et jette le reste de la ligne en cours, jusqu'au '\n' ou jusqu'à la fin du fichier. */
/* Sert en quelque sorte à sauter une ligne à sa fin */
static void skip_line(FILE* f) {
    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF);
}

/* Lit le dico.txt et le renvoie proprement avec un mot par ligne en majuscule, tel une liste de mots */
int dico_sort(const char* chemin, Arbre* arbre) {
    
    if (arbre == NULL) return -1;

    FILE* f = fopen(chemin, "r");

    /* Arrête si il n'y a rien */
    if (f == NULL) return -1;

    char ligne[256];
    int nb = 0;

    /* Boucle qui "range" la ligne */
    while (fgets(ligne, sizeof ligne, f) != NULL) {
        /* Si fin de ligne alors on passe à la suivante */
        if (strchr(ligne, '\n') == NULL)
            skip_line(f);
        /* Si la ligne est correct on l'affiche et on passe à la suivante */
        if (clean_line(ligne)) {
            if (arbre_insert(arbre, ligne) == 0) {
                fclose(f);
                return -1;
            }
            nb++;
        }
    }

    /* On referme le fichier et on return le nombre de lignes */
    fclose(f);
    return nb;
}

/* Initialise un arbre vide */
int arbre_init(Arbre* arbre) {
    /* Vérifier que le pointeur reçu existe. */
    if (arbre == NULL) {
        return 0;
    }

    /* L’arbre ne contient encore aucun mot ni aucune cellule. */
    arbre->nb_mots = 0;
    arbre->nb_cellules = 0;

    /* Nombre de cellule disponibles au départ. */
    arbre->capacite = 16;

    /* Réserver le tableau de cellule. */
    arbre->cellule = malloc(arbre->capacite * sizeof(Cellule));

    /* Vérifier que malloc a réussi. */
    if (arbre->cellule == NULL) {
        arbre->capacite = 0;
        return 0;
    }

    return 1;
}

/* Vide totalement un arbre */
void arbre_free(Arbre* arbre) {
    /* Aucun arbre reçu : rien à libérer. */
    if (arbre == NULL) {
        return;
    }

    /* Libération du tableau alloué dans arbre_init. */
    free(arbre->cellule);

    /* Le pointeur ne doit plus désigner la mémoire libérée. */
    arbre->cellule = NULL;

    /* L’arbre redevient vide. */
    arbre->nb_mots = 0;
    arbre->nb_cellules = 0;
    arbre->capacite = 0;
}

/* Ajoute une lettre dans une cellule */
static int add_cellule(Arbre* arbre, char lettre) {
    /* Vérifier les paramètres. */
    if (arbre == NULL || arbre->cellule == NULL) {
        return NO_INDEX;
    }

    /* Si le tableau est plein, il faut l’agrandir. */
    if (arbre->nb_cellules == arbre->capacite) {
        /* On multiplie la capacité par 2 */
        int new_capacite = arbre->capacite * 2;

        Cellule* new_cellule = realloc(arbre->cellule, new_capacite * sizeof(Cellule));

        /* realloc peut échouer. */
        if (new_cellule == NULL) return NO_INDEX;

        arbre->cellule = new_cellule;
        arbre->capacite = new_capacite;
    }

    int indice = arbre->nb_cellules;

    /* On remplie ces variables */
    arbre->cellule[indice].lettre = lettre;
    arbre->cellule[indice].fils = NO_INDEX;
    arbre->cellule[indice].frere = NO_INDEX;
    arbre->cellule[indice].fin_mot = 0;

    /* Une cellule en plus */
    arbre->nb_cellules++;

    return indice;
}

/* Trouve une lettre précise ( si elle existe ) avec l'indice du premier frère */
static int find_cellule( const Arbre* arbre, int indice, char lettre) {
    /* Vérification des paramètres. */
    if (arbre == NULL || arbre->cellule == NULL) return NO_INDEX;

    /* Indice actuel */
    int atm = indice;

    /* Boucle qui permet de trouver la lettre tant que il reste des possibilités */
    while (atm != NO_INDEX) {
        if (arbre->cellule[atm].lettre == lettre) {
            return atm;
        }
        /* Si toujours rien on regarde le frère suivant */
        atm = arbre->cellule[atm].frere;
    }

    /* Si on ne trouve rien */
    return NO_INDEX;
}

/* Ajoute un mot dans l'arbre */
int arbre_insert(Arbre* arbre, const char* mot) {

    /* L’arbre et le mot doivent exister. */
    if (arbre == NULL || mot == NULL) {
        return 0;
    }

    /* Le tableau doit avoir été initialisé. */
    if (arbre->cellule == NULL) {
        return 0;
    }

    /* Le mot ne doit pas être vide. */
    if (mot[0] == '\0') {
        return 0;
    }
    /* Définition de variable utiles pour la boucle for */
    int premier_frere = (arbre->nb_cellules == 0) ? NO_INDEX : 0;
    int parent = NO_INDEX;
    int idx_atm = NO_INDEX;

    /* Boucle d'jout du mot */
    for (int i = 0; mot[i] != '\0'; i++) {

        /* Recherche de la lettre actuelle parmi les index de ce niveau */
        idx_atm = find_cellule(arbre, premier_frere, mot[i]);

        /* Si on ne trouve rien, on va donc ajouter une cellule */
        if (idx_atm == NO_INDEX) {

            /* On ajoute les cellules */
            idx_atm = add_cellule(arbre, mot[i]);

            /* Si l'ajout de la cellule échoue */
            if (idx_atm == NO_INDEX) return 0;

            /* Si il n'existe aucune lettre à ce niveau*/
            if (premier_frere == NO_INDEX) {
                /* Si il a un parent */
                if (parent != NO_INDEX) {
                    /* Alors on ajoute l'index du fils*/
                    arbre->cellule[parent].fils = idx_atm;
                }
            } else {
                int dernier = premier_frere;

                /* On va jusqu'au bout des frères */
                while (arbre->cellule[dernier].frere != NO_INDEX) {
                    dernier = arbre->cellule[dernier].frere;
                }

                /* On relie la lettre ajouté à ses frères */
                arbre->cellule[dernier].frere = idx_atm;
            }
        }

        /* On adapte le parent */
        parent = idx_atm;
        /* On descend d'un niveau */
        premier_frere = arbre->cellule[idx_atm].fils;
    }

    /* On ferme le mot */
    if (arbre->cellule[idx_atm].fin_mot == 0) {
        arbre->cellule[idx_atm].fin_mot = 1;
        arbre->nb_mots++;
    }
    return 1;
}

/* Cherche un mot dans l'arbre et renvoie 0 si le mot est présent, 1 s'il est seulement un préfixe, 2 sinon. */
int arbre_find(const Arbre* arbre, const char* mot) {
    /* L'arbre et le mot doivent exister */
    if (arbre == NULL || mot == NULL) {
        return 2;
    }

    /* Le tableau doit avoir été initialisé */
    if (arbre->cellule == NULL) {
        return 2;
    }

    /* Le mot ne doit pas être vide */
    if (mot[0] == '\0') {
        return 2;
    }

    /* Niveau de départ : la racine (0) si l'arbre n'est pas vide */
    int premier_frere = (arbre->nb_cellules == 0) ? NO_INDEX : 0;
    int idx_atm = NO_INDEX;

    /* Descente lettre par lettre, sans création */
    for (int i = 0; mot[i] != '\0'; i++) {
        idx_atm = find_cellule(arbre, premier_frere, mot[i]);

        /* Lettre absente : ni mot ni préfixe */
        if (idx_atm == NO_INDEX) {
            return 2;
        }

        /* On descend d'un niveau */
        premier_frere = arbre->cellule[idx_atm].fils;
    }

    /* Toutes les lettres trouvées : fin_mot tranche entre mot et préfixe */
    if (arbre->cellule[idx_atm].fin_mot == 1) {
        return 0;
    }
    return 1;
}

/* Écrit l'arbre dans un fichier binaire .lex : en-tête puis tableau de cellules.
   Renvoie 1 si succès, 0 sinon. */
int lex_write(const Arbre* arbre, const char* chemin) {
    /* L'arbre doit exister et être initialisé */
    if (arbre == NULL || arbre->cellule == NULL || chemin == NULL) {
        return 0;
    }

    /* Ouverture en binaire : indispensable sous Windows (pas de conversion \n) */
    FILE* f = fopen(chemin, "wb");
    if (f == NULL) {
        return 0;
    }

    /* Remplissage de l'en-tête */
    Entete entete;
    entete.taille_entete = sizeof(Entete);
    entete.nb_mots = arbre->nb_mots;
    entete.nb_cellules = arbre->nb_cellules;
    entete.taille_cellule = sizeof(Cellule);

    /* Écriture de l'en-tête : 1 bloc de sizeof(Entete) octets */
    if (fwrite(&entete, sizeof(Entete), 1, f) != 1) {
        fclose(f);
        return 0;
    }

    /* Écriture des cellules utilisées (pas la capacité entière) */
    size_t nb = (size_t) arbre->nb_cellules;
    if (nb > 0 && fwrite(arbre->cellule, sizeof(Cellule), nb, f) != nb) {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}

/* Charge un fichier .lex dans un arbre (non initialisé) et renvoie 1 si succès, 0 sinon. */
int lex_read(Arbre* arbre, const char* chemin) {
    if (arbre == NULL || chemin == NULL) {
        return 0;
    }

    /* Ouverture en binaire */
    FILE* f = fopen(chemin, "rb");
    if (f == NULL) {
        return 0;
    }

    /* Lecture de l'en-tête */
    Entete entete;
    if (fread(&entete, sizeof(Entete), 1, f) != 1) {
        fclose(f);
        return 0;
    }

    /* Vérification : le fichier doit correspondre à nos structures */
    if (entete.taille_entete != (int) sizeof(Entete)
        || entete.taille_cellule != (int) sizeof(Cellule)
        || entete.nb_cellules < 0 || entete.nb_mots < 0) {
        fclose(f);
        return 0;
    }

    /* Allocation du tableau à la taille exacte (au moins 1 pour éviter malloc(0)) */
    int capacite = (entete.nb_cellules > 0) ? entete.nb_cellules : 1;
    Cellule* cellules = malloc(capacite * sizeof(Cellule));
    if (cellules == NULL) {
        fclose(f);
        return 0;
    }

    /* Lecture des cellules d'un coup */
    size_t nb = (size_t) entete.nb_cellules;
    if (nb > 0 && fread(cellules, sizeof(Cellule), nb, f) != nb) {
        free(cellules);
        fclose(f);
        return 0;
    }

    fclose(f);

    /* Remplissage de l'arbre */
    arbre->cellule = cellules;
    arbre->nb_cellules = entete.nb_cellules;
    arbre->nb_mots = entete.nb_mots;
    arbre->capacite = capacite;
    return 1;
}

