; bootinfo.inc - estrutura de informações repassada pelo estágio 2 ao kernel do Boz OS.
; Fica localizada em 0x5000 (memória baixa livre).

BOOTINFO            equ 0x5000
MAPA_E820           equ 0x5100     ; até 64 entradas de 24 bytes
MAPA_MAX            equ 64
VBE_INFO            equ 0x6000     ; 512 bytes para dados do VBE
VBE_MODO            equ 0x6200     ; 256 bytes para info de modo
BOOTINFO_TAM        equ 192

BOOT_MAGICO         equ 0x5A4F4253 ; 'ZOBS' (Boz System) - assinatura mágica de carga

BI_ASSINATURA       equ 0          ; 'BOBI' (Boz Boot Info) -> 0x49424F42
BI_VERSAO           equ 4
BI_DISCO            equ 8          ; Número do disco de boot (DL)
BI_MAPA_QTD         equ 12
BI_MAPA_END         equ 16
BI_K_INICIO         equ 20
BI_K_BYTES          equ 24
BI_R_INICIO         equ 28
BI_R_BYTES          equ 32
BI_A20              equ 36         ; Status da linha A20
BI_VESA_OK          equ 40
BI_VESA_MODO        equ 44
BI_VESA_LARG        equ 48
BI_VESA_ALT         equ 52
BI_VESA_PITCH       equ 56
BI_VESA_BPP         equ 60
BI_VESA_FB          equ 64         ; Endereço do Framebuffer Linear
BI_VESA_ATIVO       equ 68
BI_SETORES_LIDOS    equ 80
BI_OPCOES           equ 148

