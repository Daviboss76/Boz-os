/* area_trabalho.c - o fundo, o mascote, os ícones e a barra superior (H20, H28).
 * Personalizado com tema Azul e Vermelho para o Boz OS. */
#include "gui.h"
#include "arte.h"
#include "console.h"
#include "memoria.h"
#include "processo.h"
#include "relogio.h"
#include "texto.h"
#include "video.h"
#include "disco.h"
#include "sistema.h"

static uint32_t *fundo;
static int larg, alt;
static int minuto_mostrado = -1;

typedef struct {
    const char *nome;
    const char *programa;
    const desenho_t *desenho;
} icone_t;

static const icone_t icones[] = {
    { "Arquivos",    "/bin/arquivos",    &arte_icone_arquivos },
    { "Terminal",    "/bin/terminal",    &arte_icone_terminal },
    { "Editor",      "/bin/editor",      &arte_icone_editor },
    { "Calculadora", "/bin/calculadora", &arte_icone_calculadora },
    { "Banana",      "/bin/banana",      &arte_icone_banana },
    { "Sagui 3D",    "/bin/sagui3d",     &arte_icone_sagui3d },   /* bônus B4 */
    { "Sobre",       "/bin/sobre-o-sistema",   &arte_icone_sobre },
    { "Pendrive",    "/bin/arquivos",    &arte_icone_pendrive },   /* só aparece com um pendrive conectado (H34) */
};
#define QTD_ICONES (int)(sizeof icones / sizeof icones[0])
#define ICONE_PENDRIVE (QTD_ICONES - 1)
static bool pendrive_visivel;
static char nome_pendrive[24] = "Pendrive";

static bool icone_visivel(int i) { return i != ICONE_PENDRIVE || pendrive_visivel; }
#define ICONE_X 20
#define ICONE_Y0 (BARRA_SUPERIOR + 16)
#define ICONE_PASSO 88
#define ICONE_L 96
#define ICONE_A 88

int area_qtd_icones(void) { return QTD_ICONES; }

void area_icone_retangulo(int i, int *x, int *y, int *l, int *a)
{
    *x = ICONE_X; *y = ICONE_Y0 + i * ICONE_PASSO; *l = ICONE_L; *a = ICONE_A;
}

/* Interpolação de um canal em inteiros. */
static uint32_t mistura(uint32_t a, uint32_t b, int num, int den)
{
    uint32_t r = 0;
    for (int s = 0; s <= 16; s += 8) {
        int ca = (int)((a >> s) & 255), cb = (int)((b >> s) & 255);
        r |= (uint32_t)(ca + (cb - ca) * num / den) << s;
    }
    return r;
}

static void texto_com_sombra(superficie_t *s, int x, int y, const char *t, uint32_t cor)
{
    sup_texto(s, x + 1, y + 1, t, 0x101010);
    sup_texto(s, x, y, t, cor);
}

static void desenhar_fundo(void)
{
    superficie_t s;
    sup_iniciar(&s, fundo, larg, alt);
    
    /* Tema de cores: Azul profundo em cima transicionando para Vermelho Vinho embaixo */
    const uint32_t azul_topo = 0x0A192F;
    const uint32_t vermelho_baixo = 0x5C061C;
    
    for (int y = 0; y < alt; y++) {
        uint32_t c = mistura(azul_topo, vermelho_baixo, y, alt);
        uint32_t *p = fundo + y * larg;
        for (int x = 0; x < larg; x++) p[x] = c;
    }
    
    /* O mascote no canto de baixo à direita com assinatura Boz OS */
    int esc = larg >= 1000 ? 5 : 3;
    int mx = larg - 32 * esc - 30, my = alt - 32 * esc - 40;
    sup_desenho(&s, mx, my, &arte_sagui, esc);
    texto_com_sombra(&s, mx + 16 * esc - texto_largura("Boz OS") / 2, my + 32 * esc + 6, "Boz OS", 0xFFFFFF);
    
    /* Ícones na coluna da esquerda com painéis de fundo limpos e modernos */
    for (int i = 0; i < QTD_ICONES; i++) {
        if (!icone_visivel(i)) continue;
        int x, y, l, a;
        area_icone_retangulo(i, &x, &y, &l, &a);
        
        /* Painel translúcido/sutil de fundo para destacar o ícone */
        sup_retangulo(&s, x + 2, y, l - 4, a - 16, 0x18ffffff);
        sup_retangulo(&s, x, y + 2, l, a - 20, 0x18ffffff);
        
        sup_desenho(&s, x + (l - 64) / 2, y + 4, icones[i].desenho, 2);
        const char *nome = i == ICONE_PENDRIVE ? nome_pendrive : icones[i].nome;
        texto_com_sombra(&s, x + (l - texto_largura(nome)) / 2, y + 68, nome, 0xFFFFFF);
    }
}

bool area_iniciar(void)
{
    larg = (int)video_largura();
    alt = (int)video_altura();
    fundo = kmalloc((uint32_t)(larg * alt) * 4);
    if (!fundo) return false;
    desenhar_fundo();
    return true;
}

void area_fundo(superficie_t *s)
{
    sup_copiar(s, 0, 0, fundo, larg, alt, larg);
}

bool area_relogio_mudou(void)
{
    data_hora_t d;
    cmos_ler_data(&d);
    return d.minuto != minuto_mostrado;
}

void area_barra(superficie_t *s)
{
    static const char *semana[] = { "dom", "seg", "ter", "qua", "qui", "sex", "sáb" };
    sup_retangulo(s, 0, 0, larg, BARRA_SUPERIOR, 0x0F111A);
    sup_retangulo(s, 0, BARRA_SUPERIOR - 1, larg, 1, 0xD93838);
    
    desenho_t cabeca = { arte_icone_sobre.linhas + 2, 32, 16 };
    sup_desenho(s, 6, 4, &cabeca, 1);
    sup_texto(s, 44, 4, "Boz OS", 0xFF5555);
    sup_texto(s, 112, 4, "ambiente customizado", 0xA0A0A0);
    
    data_hora_t d;
    cmos_ler_data(&d);
    minuto_mostrado = d.minuto;
    char hora[48];
    snformatar(hora, sizeof hora, "%s %02u/%02u/%u  %02u:%02u", semana[dia_da_semana(d.ano, d.mes, d.dia)],
               d.dia, d.mes, d.ano, d.hora, d.minuto);
    sup_texto(s, larg - texto_largura(hora) - 10, 4, hora, 0xFFFFFF);
    klog("[GUI] relogio: %s\n", hora);
}

int area_icone_em(int x, int y)
{
    for (int i = 0; i < QTD_ICONES; i++) {
        if (!icone_visivel(i)) continue;
        int ix, iy, l, a;
        area_icone_retangulo(i, &ix, &iy, &l, &a);
        if (x >= ix && x < ix + l && y >= iy && y < iy + a) return i;
    }
    return -1;
}

bool area_pendrive(bool presente, const char *rotulo)
{
    char nome[24];
    txt_copia_n(nome, rotulo && rotulo[0] ? rotulo : "Pendrive", sizeof nome);
    if (presente == pendrive_visivel && txt_compara(nome, nome_pendrive) == 0) return false;
    pendrive_visivel = presente;
    txt_copia_n(nome_pendrive, nome, sizeof nome_pendrive);
    desenhar_fundo();
    return true;
}

static disco_t *primeiro_disco(void)
{
    for (int i = 0; i < MAX_DISCOS; i++) {
        disco_t *d = disco_indice(i);
        if (d && d->presente && !d->de_boot) return d;
    }
    return NULL;
}

void area_icone_direito(int i)
{
    if (i != ICONE_PENDRIVE) return;
    disco_t *d = disco_montado();
    if (d) disco_desmontar(d);
    else gui_aviso("O pendrive não está montado: pode remover.");
}

void area_icone_abrir(int i)
{
    if (i < 0 || i >= QTD_ICONES) return;
    if (i == ICONE_PENDRIVE) {
        disco_t *d = primeiro_disco();
        if (!d) return;
        int r = disco_montar(d);
        if (r == ERRO_FORMATO) { gui_aviso("Este pendrive não está em FAT32: formato não suportado."); return; }
        if (r < 0 && r != ERRO_OCUPADO) { gui_aviso("Não consegui montar o pendrive."); return; }
        char *argv[] = { "arquivos", "/usb", NULL };
        int pid = processo_executar("/bin/arquivos", 2, argv, NULL);
        klog("[GUI] icone Pendrive -> /bin/arquivos /usb (pid %d)\n", pid);
        return;
    }
    char *argv[] = { (char *)icones[i].programa + 5, NULL };
    int pid = processo_executar(icones[i].programa, 1, argv, NULL);
    klog("[GUI] icone %s -> %s (pid %d)\n", icones[i].nome, icones[i].programa, pid);
}

