# ---------- Makefile do Boz OS ----------

CC      := i686-linux-gnu-gcc
AS      := nasm
LD      := i686-linux-gnu-ld
PY      := python3

CFLAGS  := -m32 -march=i686 -std=gnu11 -O2 -g \
           -ffreestanding -fno-builtin -nostdlib -fno-pic -fno-pie \
           -fno-stack-protector -fno-asynchronous-unwind-tables \
           -fno-omit-frame-pointer \
           -Wall -Wextra -Werror=implicit-function-declaration \
           -finput-charset=UTF-8 -fexec-charset=UTF-8 \
           -DSAGUI_KERNEL -Ikernel/inc

LDFLAGS := -m elf_i386 -nostdlib --no-dynamic-linker -z noexecstack --no-warn-rwx-segments

B       := build

KERNEL_C   := $(shell find kernel -name '*.c' | sort)
KERNEL_ASM := $(shell find kernel -name '*.asm' | sort)
KERNEL_OBJ := $(patsubst %.c,$(B)/%.o,$(KERNEL_C)) $(patsubst %.asm,$(B)/%.asm.o,$(KERNEL_ASM)) \
              $(B)/gerado/fonte8x16.o
KERNEL_HDR := $(shell find kernel -name '*.h')
ROOTFS     := $(shell find rootfs -type f | sort)

UCFLAGS := -m32 -march=i686 -std=gnu11 -O2 \
           -ffreestanding -fno-builtin -nostdlib -fno-pic -fno-pie \
           -fno-stack-protector -fno-asynchronous-unwind-tables \
           -ffunction-sections -fdata-sections \
           -Wall -Wextra -Werror=implicit-function-declaration \
           -finput-charset=UTF-8 -fexec-charset=UTF-8 \
           -Iuserland/inc -idirafter kernel/inc

ULDFLAGS := -m elf_i386 -nostdlib -T userland/programa.ld --gc-sections -z max-page-size=4096 \
            -z noexecstack --no-warn-rwx-segments

LIBSAGUI_OBJ := $(B)/userland/fonte8x16.o \
                $(patsubst userland/libsagui/%.c,$(B)/userland/libsagui/%.o,$(wildcard userland/libsagui/*.c)) \
                $(patsubst userland/libsagui/%.asm,$(B)/userland/libsagui/%.asm.o,$(wildcard userland/libsagui/*.asm))

USER_HDR  := $(wildcard userland/inc/*.h) kernel/inc/abi.h
PROGRAMAS := $(patsubst userland/programas/%.c,$(B)/userland/bin/%,$(wildcard userland/programas/*.c))
SAGUI3D_OBJ := $(patsubst userland/sagui3d/%.c,$(B)/userland/sagui3d/%.o,$(wildcard userland/sagui3d/*.c))
PROGRAMAS += $(B)/userland/bin/sagui3d
PROG_TESTE := $(patsubst userland/testes/%.c,$(B)/userland/testes/%,$(wildcard userland/testes/*.c))

.PHONY: all vazio vdi test rodar entrega limpar dashboard clean

all: $(B)/boz-os.img

vazio:
	@echo "BozOS: alvo vazio ok"

# ---------- Bootloader ----------
$(B)/boot/mbr.bin: boot/mbr.asm boot/cabecalho.inc
	@mkdir -p $(dir $@)
	$(AS) -f bin -I boot/ -o $@ $<

$(B)/boot/estagio2.bin: boot/estagio2.asm $(wildcard boot/*.inc)
	@mkdir -p $(dir $@)
	$(AS) -f bin -I boot/ -o $@ $<

# ---------- Kernel ----------
$(B)/kernel/%.o: kernel/%.c $(KERNEL_HDR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(B)/kernel/%.asm.o: kernel/%.asm
	@mkdir -p $(dir $@)
	$(AS) -f elf32 -I kernel/arch/ -o $@ $<

$(B)/gerado/fonte8x16.c: tools/fonte/sagui8x16.txt tools/mkfonte.py
	@mkdir -p $(dir $@)
	$(PY) tools/mkfonte.py $< $@

$(B)/gerado/fonte8x16.o: $(B)/gerado/fonte8x16.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(B)/kernel.elf: $(KERNEL_OBJ) kernel/ligacao.ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -T kernel/ligacao.ld -o $@ $(KERNEL_OBJ)

$(B)/kernel.bin: $(KERNEL_OBJ) kernel/ligacao.ld
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) --oformat binary -T kernel/ligacao.ld -o $@ $(KERNEL_OBJ)

# ---------- Programas de Usuário ----------
$(B)/userland/%.o: userland/%.c $(USER_HDR)
	@mkdir -p $(dir $@)
	$(CC) $(UCFLAGS) -c -o $@ $<

$(B)/userland/fonte8x16.o: $(B)/gerado/fonte8x16.c
	@mkdir -p $(dir $@)
	$(CC) $(UCFLAGS) -c -o $@ $<

$(B)/userland/libsagui/%.asm.o: userland/libsagui/%.asm
	@mkdir -p $(dir $@)
	$(AS) -f elf32 -o $@ $<

$(B)/userland/bin/%: $(B)/userland/programas/%.o $(LIBSAGUI_OBJ) userland/programa.ld
	@mkdir -p $(dir $@)
	$(LD) $(ULDFLAGS) -o $@.elf $< $(LIBSAGUI_OBJ)
	i686-linux-gnu-objcopy --strip-all $@.elf $@ 2>/dev/null || cp $@.elf $@

$(B)/userland/sagui3d/%.o: userland/sagui3d/%.c userland/sagui3d/jogo.h $(USER_HDR)
	@mkdir -p $(dir $@)
	$(CC) $(UCFLAGS) -c -o $@ $<

$(B)/userland/bin/sagui3d: $(SAGUI3D_OBJ) $(LIBSAGUI_OBJ) userland/programa.ld
	@mkdir -p $(dir $@)
	$(LD) $(ULDFLAGS) -o $@.elf $(SAGUI3D_OBJ) $(LIBSAGUI_OBJ)
	i686-linux-gnu-objcopy --strip-all $@.elf $@ 2>/dev/null || cp $@.elf $@

$(B)/userland/testes/%: $(B)/userland/testes/%.o $(LIBSAGUI_OBJ) userland/programa.ld
	@mkdir -p $(dir $@)
	$(LD) $(ULDFLAGS) -o $@.elf $< $(LIBSAGUI_OBJ)
	i686-linux-gnu-objcopy --strip-all $@.elf $@ 2>/dev/null || cp $@.elf $@

# ---------- Ramdisk (SGFS) ----------
$(B)/ramdisk.sgfs: $(ROOTFS) $(PROGRAMAS) $(PROG_TESTE) tools/mksgfs.py
	rm -rf $(B)/rootfs
	cp -r rootfs $(B)/rootfs
	$(if $(PROGRAMAS),cp $(PROGRAMAS) $(B)/rootfs/bin/)
	$(if $(PROG_TESTE),mkdir -p $(B)/rootfs/testes && cp $(PROG_TESTE) $(B)/rootfs/testes/)
	$(PY) tools/mksgfs.py $(B)/rootfs $@

# ---------- Imagem Final ----------
$(B)/boz-os.img: $(B)/boot/mbr.bin $(B)/boot/estagio2.bin $(B)/kernel.bin $(B)/kernel.elf $(B)/ramdisk.sgfs
	$(PY) tools/mkimagem.py --mbr $(B)/boot/mbr.bin --estagio2 $(B)/boot/estagio2.bin \
	    --kernel $(B)/kernel.bin --kernel-elf $(B)/kernel.elf \
	    --ramdisk $(B)/ramdisk.sgfs \
	    --saida $@ --vesa
	cd $(B) && sha256sum boz-os.img > boz-os.img.sha256

$(B)/boz-os.vdi: $(B)/boz-os.img
	rm -f $@ $(B)/vdi.raw
	cp $< $(B)/vdi.raw
	truncate -s %1M $(B)/vdi.raw
	qemu-img convert -f raw -O vdi $(B)/vdi.raw $(B)/boz-os.vdi
	rm -f $(B)/vdi.raw
	cd $(B) && sha256sum boz-os.vdi > boz-os.vdi.sha256

vdi: $(B)/boz-os.vdi

test: $(B)/boz-os.img
	$(PY) tests/rodar_todos.py

rodar: $(B)/boz-os.img
	qemu-system-i386 -m 512M -drive format=raw,file=$(B)/boz-os.img

entrega: $(B)/boz-os.img $(B)/boz-os.vdi
	@test -n "$(FASE)" || (echo "use: make entrega FASE=N" && false)
	mkdir -p entrega/fase$(FASE)
	cp $(B)/boz-os.img $(B)/boz-os.vdi entrega/fase$(FASE)/
	cd entrega/fase$(FASE) && sha256sum boz-os.img boz-os.vdi > SHA256SUMS.txt
	@cat entrega/fase$(FASE)/SHA256SUMS.txt

dashboard:
	$(PY) tools/status.py atualizar

limpar: clean

clean:
	rm -rf build/*.bin build/*.elf build/*.img build/*.vdi build/*.sgfs build/rootfs
	rm -f boz-os.img boz-os.vdi

