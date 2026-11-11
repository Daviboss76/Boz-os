; cabecalho.inc - formato do cabeçalho de boot do Boz OS (LBA 1 da imagem).

CAB_LBA             equ 1          ; único LBA fixo do projeto
CAB_ENDERECO        equ 0x7E00     ; onde o MBR deixa o cabeçalho na memória
CAB_ASSINATURA_VAL  equ 0x5A4F4253 ; 'BOZS' (Boz System) em little-endian

CAB_ASSINATURA      equ 0          ; u32 'BOZS'
CAB_VERSAO          equ 4          ; u16
CAB_OPCOES          equ 6          ; u16
CAB_E2_LBA          equ 8          ; u32 LBA do estágio 2
CAB_E2_SETORES      equ 12         ; u32
CAB_K_LBA           equ 16         ; u32 LBA do kernel
CAB_K_SETORES       equ 20         ; u32
CAB_K_BYTES         equ 24         ; u32 tamanho exato do kernel.bin
CAB_K_ENDERECO      equ 28         ; u32 endereço físico de carga (1 MB)
CAB_IMG_SETORES     equ 32         ; u32 tamanho total útil
CAB_K_ENTRADA       equ 36         ; u32 endereço de entrada do kernel

ESTAGIO2_ENDERECO   equ 0x8000     ; 32 KB no máximo na memória baixa
ESTAGIO2_MAX_SET    equ 64

