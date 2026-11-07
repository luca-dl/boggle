#include <string.h>
#include "boggle.h"

/* Renvoie les points rapportés par un seul mot. */
static int points_mot(const char* mot) {
    size_t n = strlen(mot);      /* nombre de lettres */

    if (n < 3)  return 0;
    if (n <= 4) return 1;
    if (n == 5) return 2;
    if (n == 6) return 3;
    if (n == 7) return 5;
    return 11;                  /* 8 lettres et plus */
}

/* Additionne les points de tous les mots. */
int score(char* mots[], int nb) {
    int total = 0;
    for (int i = 0; i < nb ; i++) {
        total += points_mot(mots[i]);
    }
    return total;
}