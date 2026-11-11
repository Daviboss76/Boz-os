/* sobre-o-sistema - a janela "Sobre o Boz OS": versão, tamanho da
 * imagem, processos ativos, memória e créditos do fork. Atualiza a cada segundo. */
#include "sagui.h"
#include "gui.h"

/* Mini bandeira da Itália para o topo da janela do Boz OS */
static const char *const bandeira_topo[16] = {
    "........PPPPPPPPPPPPPPPP........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXVVFFFFRRXXP...........",
    "........PXXXXXXXXXXXXXXXX.......",
    "........PPPPPPPPPPPPPPPP........",
};

static uint32_t paleta(char c)
{
    switch (c) {
    case 'P': return 0x141210; /* preto */
    case 'X': return 0xE0E0E0; /* moldura branca */
    case 'V': return 0x009246; /* verde */
    case 'F': return 0xF4F5F0; /* branco */
    case 'R': return 0xCE2B37; /* vermelho */
    default: return 0xFF000000;
    }
}

static void informacoes(jan_t *j)
{
    info_versao_t v;
    info_memoria_t mem;
    static info_processo_t p[64];
    info_sistema(INFO_VERSAO, &v, sizeof v);
    info_sistema(INFO_MEMORIA, &mem, sizeof mem);
    int procs = info_sistema(INFO_PROCESSOS, p, 64);
    char t[96];
    g_retangulo(j, 20, 128, 400, 112, COR_FUNDO);
    formatar_texto(t, sizeof t, "Versão:              %s", v.versao);
    g_texto(j, 20, 128, t, COR_TEXTO);
    /* Tamanho em KB com uma casa decimal, só com inteiros. */
    unsigned kb10 = v.imagem_bytes * 10 / 1024, pct10 = v.imagem_bytes / 1474;   /* 1,44 MB = 1474560 bytes */
    formatar_texto(t, sizeof t, "Tamanho da imagem:   %u,%u KB (%u,%u%% de 1,44 MB)", kb10 / 10, kb10 % 10, pct10 / 10, pct10 % 10);
    g_texto(j, 20, 148, t, COR_TEXTO);
    formatar_texto(t, sizeof t, "Processos ativos:    %d", procs);
    g_texto(j, 20, 168, t, COR_TEXTO);
    formatar_texto(t, sizeof t, "Memória livre:       %u MB de %u MB", mem.livre_kb / 1024, mem.gerida_kb / 1024);
    g_texto(j, 20, 188, t, COR_TEXTO);
    formatar_texto(t, sizeof t, "Tela:                %ux%u", v.largura, v.altura);
    g_texto(j, 20, 208, t, COR_TEXTO);
    formatar_texto(t, sizeof t, "Ligado há:           %u min %02u s", v.segundos_ligado / 60, v.segundos_ligado % 60);
    g_texto(j, 20, 228, t, COR_TEXTO);
    jan_mostrar_area(j, 20, 128, 400, 112);
    static bool registrado;
    if (!registrado) registrar("sobre: versao %s, imagem %u bytes, %d processos", v.versao, v.imagem_bytes, procs);
    registrado = true;
}

int principal(int argc, char **argv)
{
    (void)argc; (void)argv;
    jan_t *j = jan_abrir("Sobre o Boz OS", 440, 400);
    if (!j) return 1;
    g_retangulo(j, 0, 0, 440, 110, 0x0B7A3E);
    g_desenho(j, 20, 14, bandeira_topo, 32, 16, paleta, 5);
    g_texto(j, 200, 30, "Boz OS", 0xFFDF4A);
    g_texto(j, 200, 52, "um fork do SaguiOS", 0xFFFFFF);
    g_texto(j, 200, 70, "customizado por Davi Boss", 0xFFFFFF);
    g_texto(j, 20, 256, "Créditos", COR_DESTAQUE);
    g_texto(j, 20, 276, "Boz OS é um sistema derivado (fork)", COR_TEXTO);
    g_texto(j, 20, 294, "utilizando o kernel e arquitetura original", COR_TEXTO);
    g_texto(j, 20, 312, "do SaguiOS, com nova identidade visual,", COR_TEXTO);
    g_texto(j, 20, 330, "temas e adaptações exclusivas.", COR_TEXTO);
    botao_t fechar = { 440 - 110, 360, 90, 26, "Fechar", false };
    botao_desenhar(j, &fechar);
    informacoes(j);
    jan_mostrar(j);
    evento_t e;
    unsigned ultimo = tempo_ms();
    for (;;) {
        if (proximo_evento(j->id, &e, false) == 1) {
            if (e.tipo == EV_FECHAR) break;
            if (botao_tratar(j, &fechar, &e)) break;
            if (e.tipo == EV_TECLA && (e.tecla == '\n' || e.tecla == TECLA_ESC)) break;
            continue;
        }
        if (tempo_ms() - ultimo >= 1000) { ultimo = tempo_ms(); informacoes(j); }
        dormir(50);
    }
    jan_fechar(j);
    return 0;
}

