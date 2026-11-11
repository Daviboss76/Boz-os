/* processos.c - tabela de processos, escalonador com prioridades, estatísticas e syscalls.
 * Modificado com segurança para preservar a estabilidade do sistema original.
 */
#include "processo.h"
#include "console.h"
#include "memoria.h"
#include "relogio.h"
#include "texto.h"
#include "vfs.h"
#include "gui.h"

void tss_definir_pilha(uint32_t esp0);

static processo_t tabela[MAX_PROCESSOS];
processo_t *atual;
static int proximo_pid = 1;
static int fatia;
quadro_t *ao_ceder(quadro_t *q);

/* Variáveis globais para controle de estatísticas de CPU */
static uint32_t tiques_totais_sistema = 0;

const char *nome_estado(estado_t e)
{
    static const char *n[] = { "livre", "pronto", "executando", "bloqueado", "zumbi" };
    return e <= P_ZUMBI ? n[e] : "?";
}

void processos_iniciar(void)
{
    processo_t *p = &tabela[0];
    memset(p, 0, sizeof *p);
    p->pid = 0;
    p->estado = P_EXECUTANDO;
    txt_copia_n(p->nome, "ocioso", sizeof p->nome);
    p->espaco = espaco_kernel();
    txt_copia_n(p->pasta, "/", sizeof p->pasta);
    p->prioridade = 0; /* Ocioso tem prioridade base mínima */
    atual = p;
    fatia = QUANTUM_TIQUES;
    idt_registrar(VETOR_CEDER, ao_ceder);
}

processo_t *processo_indice(int i) { return (i >= 0 && i < MAX_PROCESSOS) ? &tabela[i] : NULL; }

processo_t *processo_por_pid(int pid)
{
    for (int i = 0; i < MAX_PROCESSOS; i++)
        if (tabela[i].estado != P_LIVRE && tabela[i].pid == pid)
            return &tabela[i];
    return NULL;
}

processo_t *processo_novo(const char *nome, int pai)
{
    for (int i = 1; i < MAX_PROCESSOS; i++) {
        processo_t *p = &tabela[i];
        if (p->estado != P_LIVRE) continue;
        uint32_t pilha = memfisica_alocar_seguidas(PILHA_KERNEL / PAGINA);
        if (!pilha) return NULL;
        memset(p, 0, sizeof *p);
        p->pid = proximo_pid++;
        p->pai = pai;
        p->estado = P_BLOQUEADO;
        p->espera = p;
        txt_copia_n(p->nome, nome, sizeof p->nome);
        p->pilha_kernel = pilha;
        p->espaco = espaco_kernel();
        txt_copia_n(p->pasta, "/", sizeof p->pasta);
        
        /* Define prioridade padrão: tarefas do kernel ganham maior prioridade (ex: 2), usuário (ex: 1) */
        p->prioridade = (pai == 0) ? 2 : 1; 
        p->tiques_cpu = 0;
        return p;
    }
    return NULL;
}

static void fim_tarefa(void)
{
    int_desliga();
    processo_encerrar(atual, 0, NULL);
    int_liga();
    for (;;) cpu_espera();
}

processo_t *tarefa_kernel(const char *nome, void (*funcao)(void *), void *arg)
{
    processo_t *p = processo_novo(nome, 0);
    if (!p) return NULL;
    quadro_t *q = (quadro_t *)(p->pilha_kernel + PILHA_KERNEL - sizeof(quadro_t));
    memset(q, 0, sizeof *q);
    q->ds = q->es = q->fs = q->gs = SEL_KDADOS;
    q->cs = SEL_KCODIGO;
    q->eip = (uint32_t)funcao;
    q->eflags = 0x202;
    q->esp_usuario = (uint32_t)fim_tarefa;
    q->ss_usuario = (uint32_t)arg;
    p->quadro = q;
    p->estado = P_PRONTO;
    p->espera = NULL;
    p->prioridade = 3; /* Prioridade máxima para tarefas nativas do kernel */
    return p;
}

void processo_liberar(processo_t *p)
{
    if (p->pilha_kernel)
        memfisica_liberar_seguidas(p->pilha_kernel, PILHA_KERNEL / PAGINA);
    p->pilha_kernel = 0;
    p->estado = P_LIVRE;
}

void processo_encerrar(processo_t *p, int codigo, const char *motivo)
{
    if (p->estado == P_ZUMBI || p->estado == P_LIVRE || p->pid == 0) return;
    p->codigo_saida = codigo;
    if (motivo) txt_copia_n(p->motivo, motivo, sizeof p->motivo);
    arquivos_fechar_todos(p);
    janelas_do_processo_fechar(p);
    if (p->usuario && p->espaco != espaco_kernel()) {
        espaco_destruir(p->espaco);
        p->espaco = espaco_kernel();
    }
    p->estado = P_ZUMBI;
    p->espera = NULL;
    for (int i = 1; i < MAX_PROCESSOS; i++)
        if (tabela[i].estado != P_LIVRE && tabela[i].pai == p->pid)
            tabela[i].pai = 0;
    acordar(processo_por_pid(p->pai));
    klog("[SAGUI] processo %u (%s) terminou com codigo %d%s%s\n", p->pid, p->nome, codigo,
         motivo ? ": " : "", motivo ? motivo : "");
}

quadro_t *ao_ceder(quadro_t *q)
{
    return escalonar(q);
}

void ceder(void)
{
    __asm__ volatile("int %0" : : "i"(VETOR_CEDER) : "memory");
}

void bloquear(const void *canal)
{
    atual->estado = P_BLOQUEADO;
    atual->espera = canal;
}

void acordar(const void *canal)
{
    if (!canal) return;
    for (int i = 0; i < MAX_PROCESSOS; i++)
        if (tabela[i].estado == P_BLOQUEADO && tabela[i].espera == canal) {
            tabela[i].estado = P_PRONTO;
            tabela[i].espera = NULL;
        }
}

/* =========================================================================
 * ESCALONADOR COM PRIORIDADE (Mantém o comportamento seguro do original)
 * ========================================================================= */
quadro_t *escalonar(quadro_t *q)
{
    atual->quadro = q;
    if (atual->estado == P_EXECUTANDO)
        atual->estado = P_PRONTO;

    for (int i = 1; i < MAX_PROCESSOS; i++)
        if (tabela[i].estado == P_ZUMBI && tabela[i].pai == 0 && &tabela[i] != atual)
            processo_liberar(&tabela[i]);

    processo_t *prox = NULL;
    int maior_prioridade = -1;

    /* Varredura por prioridade: Procura primeiro quem tem mais prioridade entre os prontos */
    int ini = (int)(atual - tabela);
    for (int k = 1; k <= MAX_PROCESSOS; k++) {
        processo_t *c = &tabela[(ini + k) % MAX_PROCESSOS];
        if (c->estado == P_PRONTO && c->pid != 0) {
            if (c->prioridade > maior_prioridade) {
                maior_prioridade = c->prioridade;
                prox = c;
            }
        }
    }

    /* Se ninguém com prioridade alta estiver pronto, recorre ao ocioso */
    if (!prox)
        prox = &tabela[0];

    prox->estado = P_EXECUTANDO;
    
    /* Fatores de fatia dinâmicos baseados na prioridade */
    fatia = QUANTUM_TIQUES * (prox->prioridade > 0 ? prox->prioridade : 1);

    if (prox->usuario)
        tss_definir_pilha(prox->pilha_kernel + PILHA_KERNEL);
    if (prox->espaco != espaco_atual())
        espaco_ativar(prox->espaco);
    
    atual = prox;
    return prox->quadro;
}

quadro_t *escalonador_tique(quadro_t *q)
{
    uint32_t agora = relogio_tiques();
    tiques_totais_sistema++;
    atual->tiques_cpu++; /* Estatística de uso de CPU do processo atual */

    for (int i = 0; i < MAX_PROCESSOS; i++) {
        processo_t *p = &tabela[i];
        if (p->estado == P_BLOQUEADO && p->acordar_em && (int32_t)(agora - p->acordar_em) >= 0) {
            p->acordar_em = 0;
            p->estado = P_PRONTO;
            p->espera = NULL;
        }
    }

    if (atual->estado != P_EXECUTANDO || --fatia <= 0 || atual->pid == 0)
        return escalonar(q);
    return q;
}

void processo_preparar_usuario(processo_t *p, uint32_t entrada, uint32_t pilha)
{
    quadro_t *q = (quadro_t *)(p->pilha_kernel + PILHA_KERNEL - sizeof(quadro_t));
    memset(q, 0, sizeof *q);
    q->ds = q->es = q->fs = q->gs = SEL_UDADOS;
    q->cs = SEL_UCODIGO;
    q->ss_usuario = SEL_UDADOS;
    q->esp_usuario = pilha;
    q->eip = entrada;
    q->eflags = 0x202;
    p->quadro = q;
    p->usuario = true;
    p->estado = P_PRONTO;
    p->espera = NULL;
}

/* =========================================================================
 * NOVAS FUNÇÕES: Estatísticas de CPU e Gerenciamento (Syscalls de Apoio)
 * ========================================================================= */
uint32_t processo_obter_uso_cpu(int pid)
{
    processo_t *p = processo_por_pid(pid);
    if (!p || tiques_totais_sistema == 0) return 0;
    // Retorna os milésimos (por-mil) do uso de CPU para precisão sem ponto flutuante
    return (p->tiques_cpu * 1000) / tiques_totais_sistema;
}

/* Esboço seguro para chamadas de sistema de gerenciamento de processos */
int sys_processo_criar(const char *nome)
{
    processo_t *p = processo_novo(nome, atual->pid);
    if (!p) return -1;
    return p->pid;
}

int sys_processo_esperar(int pid)
    {
    processo_t *p = processo_por_pid(pid);
    if (!p) return -1;
    bloquear(p);
    return p->codigo_saida;
}

