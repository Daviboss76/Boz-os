// kernel/main.c - Código principal do Kernel do Boz OS

#define VIDEO_MEM 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80

// Estrutura opcional para ler dados do bootinfo se precisar depois
typedef struct {
    unsigned int assinatura;
    unsigned int versao;
    unsigned int disco;
    // ... outros campos podem ser adicionados conforme o bootinfo.inc
} __attribute__((packed)) boot_info_t;

// Função básica para escrever caracteres diretamente na tela (Modo Texto VGA)
void k_print(const char* str, unsigned char color) {
    volatile char* video = (volatile char*) VIDEO_MEM;
    int i = 0;
    
    while (str[i] != '\0') {
        video[i * 2] = str[i];     // Caractere ASCII
        video[i * 2 + 1] = color;  // Atributo de cor (fundo e letra)
        i++;
    }
}

// Ponto de entrada do Kernel chamado diretamente pelo Estágio 2
void kernel_main(unsigned int magic, boot_info_t* boot_info) {
    // Cor 0x0A = Letra Verde Claro (A) com fundo Preto (0)
    unsigned char verde_claro = 0x0A;
    unsigned char branco = 0x0F;

    k_print("================================================================", verde_claro);
    k_print("  Boz OS - Kernel 32-bit Inicializado com Sucesso!              ", branco);
    k_print("================================================================", verde_claro);

    // Loop infinito para travar o kernel rodando
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

