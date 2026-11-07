#include <stdio.h>
#include "utils.h"
#include "boggle.h"

int main(int argc, char* argv[]) {
    if (dec_auteur(argc, argv)) return ERR_ARGS;

    int total = score(argv + 1, argc - 1);
    printf("%i\n", total);
    return OK;
}