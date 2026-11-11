/* ps (processos) - lista os processos. */
#include "sagui.h"

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    static info_processo_t v[64];
    static const char *estados[] = { "livre", "pronto", "executando", "bloqueado", "zumbi" };
    int n = info_sistema(INFO_PROCESSOS, v, 64);
    if (n < 0) { imprimir_em(ERROS, "ps: %s\n", nome_erro(n)); return 1; }
    
    // Cabeçalho atualizado incluindo a coluna de Prioridade
    imprimir("  PID  PAI  PRIOR  ESTADO      MEMÓRIA   CPU (ms)  NOME\n");
    for (int i = 0; i < n; i++)
        imprimir("%5d %4d  %5d  %-10s %5u KB  %8u  %s%s\n", 
                 v[i].pid, v[i].pai, v[i].prioridade, // <-- Adicionada a prioridade aqui
                 v[i].estado <= 4 ? estados[v[i].estado] : "?", 
                 v[i].memoria_kb, v[i].cpu_ms,
                 v[i].nome, v[i].usuario ? "" : " (kernel)");
                 
    imprimir("%d processos\n", n);
    return 0;
}

