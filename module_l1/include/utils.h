#ifndef UTILS_H
#define UTILS_H

#define OK 0 /* Tout fonctionne */
#define ERR_ARGS 255 /* Mauvais nombre d'arguments */
#define ERR_FI 254 /* Fichier illisible ou introuvable */
#define ERR_MEM 253 /* Problème avec malloc */
#define ERR_FO 252 /* Fichier corrompu ou contenu incorrect */

/* Renvoie le nom de l'auteur du projet */
int dec_auteur(int argc, char* argv[]);

/* Vérifie si la valeur entré est valable */
int lire_entier(const char* nb);

#endif