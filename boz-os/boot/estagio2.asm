; estagio2.asm - estágio 2 do carregador de boot do Boz OS.
; O MBR nos deixa em 0000:8000 (modo real) com o disco em DL e o cabeçalho em 0x7E00.

bits 16
org 0x8000

%include "cabecalho.inc"
%include "bootinfo.inc"

BUFFER_SEG      equ 0x1000              ; Buffer de leitura em 0x10000 (abaixo de 1MB)
BUFFER_LINEAR   equ 0x10000
BLOCO_SETORES   equ 64                  ; 32 KB por leitura

SEL_CODIGO      equ 0x08
SEL_DADOS       equ 0x10

inicio:
        cli
        xor ax, ax
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov sp, 0x7C00
        sti
        cld
        mov [disco], dl

        ; Inicializar a estrutura BOOTINFO com zeros
        mov di, BOOTINFO
        mov cx, BOOTINFO_TAM / 2
        xor ax, ax
        rep stosw
        mov dword [BOOTINFO + BI_ASSINATURA], 0x49424F42     ; 'BOBI' (Boz Boot Info)
        mov dword [BOOTINFO + BI_VERSAO], 1
        movzx eax, byte [disco]
        mov [BOOTINFO + BI_DISCO], eax

        mov si, msg_titulo
        call escrever

; ---------------------------------------------------------------- [1/4] A20
        call habilitar_a20
        jc .falha_a20
        mov si, msg_a20
        call escrever

; ---------------------------------------------------------------- [2/4] Memória (E820)
        call ler_mapa_memoria
        cmp dword [BOOTINFO + BI_MAPA_QTD], 0
        je .falha_memoria
        mov si, msg_mem
        call escrever

; ---------------------------------------------------------------- [3/4] Kernel
        mov si, msg_kernel
        call escrever
        mov eax, [CAB_ENDERECO + CAB_K_ENDERECO]
        mov [BOOTINFO + BI_K_INICIO], eax
        mov eax, [CAB_ENDERECO + CAB_K_BYTES]
        mov [BOOTINFO + BI_K_BYTES], eax
        mov eax, [CAB_ENDERECO + CAB_K_LBA]
        mov ecx, [CAB_ENDERECO + CAB_K_SETORES]
        mov edi, [CAB_ENDERECO + CAB_K_ENDERECO]
        call carregar_regiao
        mov si, msg_ok
        call escrever

; ---------------------------------------------------------------- [4/4] Modo Protegido (32 bits)
        mov si, msg_pm
        call escrever

        cli
        lgdt [gdt_descritor]
        mov eax, cr0
        or eax, 1
        mov cr0, eax
        jmp SEL_CODIGO:modo_protegido

.falha_a20:
        mov si, msg_falha_a20
        jmp parar
.falha_memoria:
        mov si, msg_falha_mem
        jmp parar

; ============================================================================
; Rotinas de A20
; ============================================================================
a20_ligada:
        pushf
        push ds
        push es
        push si
        push di
        cli
        xor ax, ax
        mov ds, ax
        not ax
        mov es, ax
        mov si, 0x0500
        mov di, 0x0510
        mov dl, [ds:si]
        mov dh, [es:di]
        mov byte [ds:si], 0x00
        mov byte [es:di], 0xFF
        mov al, [ds:si]
        mov [es:di], dh
        mov [ds:si], dl
        pop di
        pop si
        pop es
        pop ds
        popf
        cmp al, 0xFF
        ret

habilitar_a20:
        call a20_ligada
        jne .fim
        ; Porta 0x92 (A20 rápida)
        in al, 0x92
        or al, 0x02
        and al, 0xFE
        out 0x92, al
        call a20_ligada
        jne .fim
        stc
        ret
.fim:
        clc
        ret

; ============================================================================
; Mapa de Memória E820
; ============================================================================
ler_mapa_memoria:
        xor ebx, ebx
        mov di, MAPA_E820
        xor bp, bp
.proxima:
        mov dword [di + 20], 1
        mov eax, 0xE820
        mov edx, 0x534D4150
        mov ecx, 24
        int 0x15
        jc .fim
        cmp eax, 0x534D4150
        jne .fim
        inc bp
        add di, 24
        cmp bp, MAPA_MAX
        jae .fim
        test ebx, ebx
        jnz .proxima
.fim:
        movzx eax, bp
        mov [BOOTINFO + BI_MAPA_QTD], eax
        mov dword [BOOTINFO + BI_MAPA_END], MAPA_E820
        ret

; ============================================================================
; Leitura de Disco e Modo Unreal
; ============================================================================
ler_setores:
        push di
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
        mov si, msg_falha_leitura
        jmp parar
.ok:
        pop di
        ret

carregar_regiao:
.laco:
        test ecx, ecx
        jz .fim
        mov edx, ecx
        cmp edx, BLOCO_SETORES
        jbe .tam_ok
        mov edx, BLOCO_SETORES
.tam_ok:
        push eax
        push ecx
        push edx
        mov [dap_qtd], dx
        mov word [dap_destino], 0
        mov word [dap_destino + 2], BUFFER_SEG
        mov [dap_lba], eax
        mov dword [dap_lba + 4], 0
        push edi
        call ler_setores
        pop edi
        pop edx

        call entrar_unreal
        mov esi, BUFFER_LINEAR
        mov ecx, edx
        shl ecx, 7
        a32 rep movsd

        mov al, '.'
        call escrever_char
        pop ecx
        pop eax
        add eax, edx
        sub ecx, edx
        jmp .laco
.fim:
        ret

entrar_unreal:
        cli
        push ds
        push es
        lgdt [gdt_descritor]
        mov eax, cr0
        or al, 1
        mov cr0, eax
        jmp short .pm
.pm:
        mov bx, SEL_DADOS
        mov ds, bx
        mov es, bx
        and al, 0xFE
        mov cr0, eax
        pop es
        pop ds
        sti
        ret

; ============================================================================
; Funções de Texto
; ============================================================================
escrever:
        push ax
.l:     lodsb
        test al, al
        jz .fim
        call escrever_char
        jmp .l
.fim:   pop ax
        ret

escrever_char:
        push ax
        push bx
        mov ah, 0x0E
        mov bx, 0x0007
        int 0x10
        pop bx
        pop ax
        ret

parar:
        call escrever
.l:     hlt
        jmp .l

; ============================================================================
; Modo Protegido (32 bits)
; ============================================================================
bits 32
modo_protegido:
        mov ax, SEL_DADOS
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov esp, 0x7C00
        mov eax, BOOT_MAGICO
        mov ebx, BOOTINFO
        jmp [CAB_ENDERECO + CAB_K_ENTRADA]

; ============================================================================
; Dados e GDT
; ============================================================================
disco:          db 0

align 4
dap:            db 0x10, 0
dap_qtd:        dw 0
dap_destino:    dw 0, 0
dap_lba:        dd 0, 0

align 8
gdt:
        dq 0
        dw 0xFFFF, 0x0000
        db 0x00, 0x9A, 0xCF, 0x00
        dw 0xFFFF, 0x0000
        db 0x00, 0x92, 0xCF, 0x00
gdt_fim:
gdt_descritor:
        dw gdt_fim - gdt - 1
        dd gdt

msg_titulo:     db "Boz-OS: Estagio 2 iniciado...", 13, 10, 0
msg_a20:        db "[1/3] Linha A20 OK", 13, 10, 0
msg_mem:        db "[2/3] Mapa de memoria mapeado OK", 13, 10, 0
msg_kernel:     db "[3/3] Carregando kernel ", 0
msg_ok:         db " OK", 13, 10, 0
msg_pm:         db "Saltando para modo protegido (32-bit)...", 13, 10, 0
msg_falha_a20:  db "ERRO: Falha ao ativar a linha A20", 0
msg_falha_mem:  db "ERRO: Falha no mapa de memoria E820", 0
msg_falha_leitura: db 13, 10, "ERRO: Falha de leitura de disco", 0

