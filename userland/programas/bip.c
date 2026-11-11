/* bip [frequência [duração]] - toca uma nota no alto-falante do PC (B3).
 * Sem argumentos: 880 Hz por 200 ms. "bip escala" ou "bip italia" toca a tarantela italiana. */
#include "sagui.h"

static int numero(const char *s, int *v)
{
    int n = 0;
    if (!*s) return 0;
    for (; *s; s++) {
        if (*s < '0' || *s > '9') return 0;
        n = n * 10 + (*s - '0');
        if (n > 100000) return 0;
    }
    *v = n;
    return 1;
}

int principal(int argc, char **argv)
{
    if (argc > 1 && (compara_texto(argv[1], "escala") == 0 || compara_texto(argv[1], "italia") == 0)) {
        /* Melodia tradicional italiana: Tarantella Napoletana */
        static const nota_t tarantella[] = {
            /* Trecho principal (A-B-C-B-A) */
            {880, 150}, {659, 150}, {880, 150}, {659, 150}, {698, 150}, {659, 150}, {587, 150}, {523, 150},
            {587, 150}, {659, 150}, {698, 150}, {784, 150}, {659, 300},
            
            /* Repetição vibrante */
            {880, 150}, {659, 150}, {880, 150}, {659, 150}, {698, 150}, {659, 150}, {587, 150}, {523, 150},
            {587, 150}, {659, 150}, {587, 150}, {494, 150}, {440, 300}
        };
        tocar(tarantella, 26, BIP_SUBSTITUIR);
        imprimir("bip: tocando Tarantella Italiana!\n");
        dormir(4500);
        return 0;
    }

    int freq = 880, ms = 200;
    if ((argc > 1 && !numero(argv[1], &freq)) || (argc > 2 && !numero(argv[2], &ms)) ||
        freq < 20 || freq > 20000 || ms < 10 || ms > 10000) {
        imprimir_em(ERROS, "uso: bip [frequência 20-20000 Hz [duração 10-10000 ms]] ou bip escala\n");
        return 1;
    }

    bip(freq, ms);
    imprimir("bip: %d Hz por %d ms\n", freq, ms);
    dormir((unsigned)ms);   /* o comando só termina quando o som acaba */
    return 0;
}

