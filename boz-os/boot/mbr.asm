; mbr.asm - estágio 1 do carregador de boot do Boz OS.
;
; A BIOS copia este setor (512 bytes) para 0x7C00 e salta para cá em modo real.
; Descobre o disco (DL), lê o cabeçalho no LBA 1 e carrega o estágio 2 em 0x8000.

bits 16
org 0x7C00

%include "cabecalho.inc"

        jmp short inicio
        nop
        times 0x5A - ($ - $$) db 0

inicio:
        cli
        xor ax, ax
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, 0x7C00
        jmp 0x0000:.cs_zerado
.cs_zerado:
        sti
        cld
        mov [disco], dl                 ; DL = número do disco de boot da BIOS

        mov si, msg_carregando
        call escrever

        ; Verificar suporte a extensões LBA (INT 13h AH=42h)
        mov ah, 0x41
        mov bx, 0x55AA
        mov dl, [disco]
        int 0x13
        jc .sem_lba
        cmp bx, 0xAA55
        jne .sem_lba

        ; Ler o Cabeçalho de boot (LBA 1) para 0x7E00
        mov word [dap_qtd], 1
        mov word [dap_destino], CAB_ENDERECO
        mov dword [dap_lba], CAB_LBA
        call ler_setores
        cmp dword [CAB_ENDERECO + CAB_ASSINATURA], CAB_ASSINATURA_VAL
        jne .sem_cabecalho

        ; Ler o Estágio 2 para 0x8000 baseado nos dados do cabeçalho
        mov eax, [CAB_ENDERECO + CAB_E2_SETORES]
        test eax, eax
        jz .sem_cabecalho
        cmp eax, ESTAGIO2_MAX_SET
        ja .sem_cabecalho
        mov [dap_qtd], ax
        mov word [dap_destino], ESTAGIO2_ENDERECO
        mov eax, [CAB_ENDERECO + CAB_E2_LBA]
        mov [dap_lba], eax
        call ler_setores

        mov dl, [disco]                 ; Passar o disco para o estágio 2 em DL
        jmp 0x0000:ESTAGIO2_ENDERECO

.sem_lba:
        mov si, msg_sem_lba
        jmp parar
.sem_cabecalho:
        mov si, msg_cabecalho
        jmp parar

; ler_setores: lê usando o pacote DAP com 3 tentativas de segurança
ler_setores:
        mov di, 3
.tentar:
        mov si, dap
        mov ah, 0x42
        mov dl, [disco]
        int 0x13
        jnc .ok
        xor ah, ah
        mov dl, [disco]
        int 0x13
        dec di
        jnz .tentar
        mov si, msg_leitura
        jmp parar
.ok:
        ret

; escrever: exibe string terminada em zero via teletipo da BIOS
escrever:
        lodsb
        test al, al
        jz .fim
        mov ah, 0x0E
        mov bx, 0x0007
        int 0x10
        jmp escrever
.fim:
        ret

parar:
        call escrever
.laco:
        hlt
        jmp .laco

disco:          db 0

; Pacote de Endereço de Disco (DAP - INT 13h / AH=42h)
align 4
dap:            db 0x10, 0
dap_qtd:        dw 0
dap_destino:    dw 0, 0                 ; offset, segmento
dap_lba:        dd 0, 0

msg_carregando: db "Boz-OS: carregando...", 13, 10, 0
msg_sem_lba:    db "Erro: BIOS sem INT13 LBA", 0
msg_cabecalho:  db "Erro: cabecalho Boz invalido", 0
msg_leitura:    db "Erro de leitura no disco", 0

        times 0x1B8 - ($ - $$) db 0
id_disco:       dd 0x5A4F4253           ; 'ZOBS' / identificador
                dw 0
tabela_part:    times 64 db 0
                dw 0xAA55

