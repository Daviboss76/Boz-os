/* arte.c - os desenhos da área de trabalho, em pixel art feita neste projeto.
 * Atualizado com o tema da bandeira da Itália para o Boz OS. */
#include "arte.h"

/* Símbolo do Boz OS: Uma Bandeira da Itália estilizada em 32x32.
 * Dividida em três listras verticais (Verde, Branca, Vermelha) com contorno limpo. */
static const char *const sagui[] = {
    "................................",
    "....PPPPPPPPPPPPPPPPPPPPPPPP....",
    "....PXXXXXXXXXXXXXXXXXXXXXXP....",
    "....PXXXXXXXXXXXXXXXXXXXXXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXVVVVVFFFFFFFFRRRRRXXP....",
    "....PXXXXXXXXXXXXXXXXXXXXXXP....",
    "....PXXXXXXXXXXXXXXXXXXXXXXP....",
    "....PPPPPPPPPPPPPPPPPPPPPPPP....",
    "............PPPPPP..............",
    "............PPPPPP..............",
    "..........PPPPPPPPPP............",
    "................................",
    "................................",
};

/* Ícones da área de trabalho, 32x32. */
static const char *const icone_arquivos[] = {   /* pasta */
    "................................",
    "................................",
    "................................",
    "................................",
    "..PPPPPPPPPP....................",
    ".PAAAAAAAAAAP...................",
    ".PAAAAAAAAAAAPPPPPPPPPPPPPPPP...",
    ".PAAAAAAAAAAAAAAAAAAAAAAAAAAAP..",
    ".PAAAAAAAAAAAAAAAAAAAAAAAAAAAP..",
    ".PPPPPPPPPPPPPPPPPPPPPPPPPPPPP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PYYYYYYYYYYYYYYYYYYYYYYYYYYYP..",
    ".PPPPPPPPPPPPPPPPPPPPPPPPPPPPP..",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

static const char *const icone_terminal[] = {   /* monitor com >_ */
    "................................",
    "................................",
    "..PPPPPPPPPPPPPPPPPPPPPPPPPPPP..",
    "..PCCCCCCCCCCCCCCCCCCCCCCCCCCP..",
    "..PCPPPPPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPGGPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPGGPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPGGPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPPGGPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPGGPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPGGPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPGGPPPPGGGGGGGPPPPPPPPPCP..",
    "..PCPPPPPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCPPPPPPPPPPPPPPPPPPPPPPPPCP..",
    "..PCCCCCCCCCCCCCCCCCCCCCCCCCCP..",
    "..PPPPPPPPPPPPPPPPPPPPPPPPPPPP..",
    "..............PCCP..............",
    "..............PCCP..............",
    "..........PPPPPPPPPPPP..........",
    "..........PCCCCCCCCCCP..........",
    "..........PPPPPPPPPPPP..........",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

static const char *const icone_editor[] = {   /* folha com linhas e lápis */
    "................................",
    "......PPPPPPPPPPPPPPPPP.........",
    "......PBBBBBBBBBBBBBBBPP........",
    "......PBBBBBBBBBBBBBBBPBP.......",
    "......PBBBBBBBBBBBBBBBPBBP......",
    "......PBBBBBBBBBBBBBBBPPPPP.....",
    "......PBBCCCCCCCCCCCBBBBBBP.....",
    "......PBBBBBBBBBBBBBBBBBBBP.....",
    "......PBBCCCCCCCCCCCCCCBBBP.....",
    "......PBBBBBBBBBBBBBBBBBBBP.....",
    "......PBBCCCCCCCCCCCCBBBBBP.....",
    "......PBBBBBBBBBBBBBBBBBBBP..PP.",
    "......PBBCCCCCCCCCCCCCCBBBP.PYYP",
    "......PBBBBBBBBBBBBBBBBBBBPPYYP.",
    "......PBBCCCCCCCCCCBBBBBBBPYYP..",
    "......PBBBBBBBBBBBBBBBBBBPYYP...",
    "......PBBCCCCCCCCCCCCCBBPYYP....",
    "......PBBBBBBBBBBBBBBBBPYYP.....",
    "......PBBCCCCCCCCCCCBBPYYP......",
    "......PBBBBBBBBBBBBBBPRRP.......",
    "......PBBBBBBBBBBBBBBPPP........",
    "......PBBBBBBBBBBBBBBBBBP.......",
    "......PBBBBBBBBBBBBBBBBBP.......",
    "......PPPPPPPPPPPPPPPPPPP.......",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

static const char *const icone_calculadora[] = {
    "................................",
    ".......PPPPPPPPPPPPPPPPPP.......",
    ".......PCCCCCCCCCCCCCCCCP.......",
    ".......PCGGGGGGGGGGGGGGCP.......",
    ".......PCGGGGGGGGGPPGPGCP.......",
    ".......PCGGGGGGGGGGPPGGCP.......",
    ".......PCGGGGGGGGGGGGGGCP.......",
    ".......PCCCCCCCCCCCCCCCCP.......",
    ".......PCPPPCPPPCPPPCVVCP.......",
    ".......PCPBPCPBPCPBPCVVCP.......",
    ".......PCPPPCPPPCPPPCVVCP.......",
    ".......PCCCCCCCCCCCCCCCCP.......",
    ".......PCPPPCPPPCPPPCVVCP.......",
    ".......PCPBPCPBPCPBPCVVCP.......",
    ".......PCPPPCPPPCPPPCVVCP.......",
    ".......PCCCCCCCCCCCCCCCCP.......",
    ".......PCPPPCPPPCPPPCVVCP.......",
    ".......PCPBPCPBPCPBPCVVCP.......",
    ".......PCPPPCPPPCPPPCVVCP.......",
    ".......PCCCCCCCCCCCCCCCCP.......",
    ".......PCPPPPPPPCPPPCAACP.......",
    ".......PCPBBBBBPCPBPCAACP.......",
    ".......PCPPPPPPPCPPPCAACP.......",
    ".......PCCCCCCCCCCCCCCCCP.......",
    ".......PPPPPPPPPPPPPPPPPP.......",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

static const char *const icone_sagui3d[] = {
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
    "PPssssssssssssssssssssssssssssPP",
    "PCPssssssssssssssssssssssssssPCP",
    "PCCPssssssssssssssssssssssssPCCP",
    "PCCkPssssssssssssssssssssssPkCCP",
    "PLCkCPssssssssssssssssssssPCkCLP",
    "PLLkCCPssssssssssssssssssPCCkLLP",
    "PLLLLCkPssssssssssssssssPkCLLLLP",
    "PLLLLLkCPssssssssssssssPCkLLLLLP",
    "PCCLLLLCCPssssssssssssPCCLLLLCCP",
    "PCCkCLLLLkPssssssssssPkLLLLCkCCP",
    "PCCkCCkLLLCPLkLLLkLLPCLLLkCCkCCP",
    "PLCkCCkCCLLCkkkkkkkkCLLCCkCCkCLP",
    "PLLLLLkCCkCLkLLLkLLLLCkCCkLLLLLP",
    "PLLLLLLLLLCCkLLLkLLLCCLLLLLLLLLP",
    "PLLLLLLLLLLLkkkkkkkkLLLLLLLLLLLP",
    "PCCkCCkCCkCCLLLkPLLkCCkCCkCCkCCP",
    "PCCkCCkCCkLLLLLPYLLkLLkCCkCCkCCP",
    "PCCkCCLLLLLCkkPYPkkkCLLLLLCCkCCP",
    "PCLLLLLLLkCLLPYPLLkLLCkLLLLLLLCP",
    "PLLLLLLCCkLPLPPLLLkLPLkCCLLLLLLP",
    "PLLLLCkCCLPggGgggggggPLCCkCLLLLP",
    "PLLkCCkLLPgGggggggggggPLLkCCkLLP",
    "PCCkCCLLPGggggggggggGggPLLCCkCCP",
    "PCCkCLLPggggggggggGgggggPLLCkCCP",
    "PCCLLLPgggggggggGggggggggPLLLCCP",
    "PCLLLPggggggggGggggggggggGPLLLCP",
    "PLLLPgggggggGggggggggggGgggPLLLP",
    "PLLPggggggGggggggggggGggggggPLLP",
    "PLPgggggGggggggggggGgggggggggPLP",
    "PPggggGggggggggggGggggggggggGgPP",
    "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
};

static const char *const icone_banana[] = {
    "................................",
    "................................",
    "......................PP........",
    ".....................PEEP.......",
    ".....................PEP........",
    "....................PYYP........",
    "...................PYYYYP.......",
    "..................PYYYYYP.......",
    ".................PYYYYYYP.......",
    "................PYYYYYYYP.......",
    "...............PYYYYYYYAP.......",
    "..............PYYYYYYYAP........",
    ".............PYYYYYYYYAP........",
    "...........PPYYYYYYYYAP.........",
    ".........PPYYYYYYYYYAP..........",
    ".......PPYYYYYYYYYYAP...........",
    ".....PPYYYYYYYYYYYAP............",
    "...PPYYYYYYYYYYYAAP.............",
    "..PEYYYYYYYYYYAAPP..............",
    "..PEEYYYYYYAAAPP................",
    "...PPAAAAAAPPP..................",
    ".....PPPPPP.....................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

/* Ícone "Sobre": Mini bandeira da Itália para o topo da barra */
static const char *const icone_sobre[] = {
    "................................",
    "................................",
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
    "........PXXXXXXXXXXXXXXXX.......",
    "........PPPPPPPPPPPPPPPP........",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

static const char *const icone_pendrive[] = {
    "................................",
    "................................",
    "................................",
    "...........PPPPPPPPPP...........",
    "...........PCCCCCCCCP...........",
    "...........PCPPCCPPCP...........",
    "...........PCPPCCPPCP...........",
    "...........PCCCCCCCCP...........",
    ".........PPPPPPPPPPPPPP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVBBBBBBVVVP.........",
    ".........PVVVBVVVVBVVVP.........",
    ".........PVVVBBBBBBVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    ".........PVVVVVVVVVVVVP.........",
    "..........PVVVVVVVVVVP..........",
    "...........PPPPPPPPPP...........",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

const desenho_t arte_sagui            = { sagui, 32, 32 };
const desenho_t arte_icone_arquivos   = { icone_arquivos, 32, 32 };
const desenho_t arte_icone_terminal   = { icone_terminal, 32, 32 };
const desenho_t arte_icone_editor     = { icone_editor, 32, 32 };
const desenho_t arte_icone_calculadora = { icone_calculadora, 32, 32 };
const desenho_t arte_icone_banana     = { icone_banana, 32, 32 };
const desenho_t arte_icone_sagui3d    = { icone_sagui3d, 32, 32 };
const desenho_t arte_icone_sobre      = { icone_sobre, 32, 32 };
const desenho_t arte_icone_pendrive   = { icone_pendrive, 32, 32 };

/* Paleta estendida com as cores da bandeira da Itália (Verde, Branco e Vermelho) */
bool arte_cor(char letra, uint32_t *cor)
{
    switch (letra) {
    case 'P': *cor = 0x141210; return true;   /* preto (contorno) */
    case 'X': *cor = 0xE0E0E0; return true;   /* moldura branca interna */
    case 'V': *cor = 0x009246; return true;   /* Verde da bandeira italiana */
    case 'F': *cor = 0xF4F5F0; return true;   /* Branco da bandeira italiana */
    case 'R': *cor = 0xCE2B37; return true;   /* Vermelho da bandeira italiana */
    case 'E': *cor = 0x3C2D23; return true;   
    case 'M': *cor = 0x6E553C; return true;   
    case 'C': *cor = 0x968C7D; return true;   
    case 'c': *cor = 0xBEB4A5; return true;   
    case 'B': *cor = 0xF5F5F0; return true;   
    case 'b': *cor = 0xFFFFFF; return true;   
    case 'Y': *cor = 0xFFD52E; return true;   
    case 'A': *cor = 0xD9A400; return true;   
    case 'G': *cor = 0x3CDC6E; return true;   
    case 'k': *cor = 0x5A564E; return true;   
    case 'L': *cor = 0x8F8A7C; return true;   
    case 's': *cor = 0x7EC8E3; return true;   
    case 'g': *cor = 0x0B7A3E; return true;   
    default: return false;
    }
}

