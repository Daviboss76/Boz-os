# Ferramentas de Compilação cruzada x86
ASM = nasm
CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-ld

# Flags de Compilação
CFLAGS = -m32 -ffreestanding -fno-pie -nostdlib -fno-builtin -nostdinc -fno-stack-protector -Wall -O2
LDFLAGS = -m elf_i386 -T kernel/linker.ld --oformat binary

# Pastas
BUILD_DIR = build
BOOT_DIR = boot
KERNEL_DIR = kernel

all: $(BUILD_DIR) boz.img

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 1. Compilar o MBR (Estágio 1 - 512 bytes)
$(BUILD_DIR)/mbr.bin: $(BOOT_DIR)/mbr.asm $(BOOT_DIR)/cabecalho.inc
	$(ASM) -I$(BOOT_DIR)/ -f bin $< -o $@

# 2. Compilar o Estágio 2 (Modo Real -> Unreal Mode)
$(BUILD_DIR)/estagio2.bin: $(BOOT_DIR)/estagio2.asm $(BOOT_DIR)/cabecalho.inc $(BOOT_DIR)/bootinfo.inc
	$(ASM) -I$(BOOT_DIR)/ -f bin $< -o $@

# 3. Compilar o Kernel (C)
$(BUILD_DIR)/kernel_entry.o: $(KERNEL_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

# 4. Linkar o Kernel para 32-bit (1 MB)
$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel_entry.o
	$(LD) $(LDFLAGS) -o $@ $^

# 5. Gerar a Imagem Final do Sistema (Unindo MBR, Estágio 2 e Kernel)
boz.img: $(BUILD_DIR)/mbr.bin $(BUILD_DIR)/estagio2.bin $(BUILD_DIR)/kernel.bin
	@echo "Montando a imagem de disco do Boz OS..."
	dd if=/dev/zero of=$(BUILD_DIR)/$@ bs=1M count=10 status=none
	dd if=$(BUILD_DIR)/mbr.bin of=$(BUILD_DIR)/$@ conv=notrunc status=none
	dd if=$(BUILD_DIR)/estagio2.bin of=$(BUILD_DIR)/$@ seek=2 conv=notrunc status=none
	dd if=$(BUILD_DIR)/kernel.bin of=$(BUILD_DIR)/$@ seek=34 conv=notrunc status=none
	mv $(BUILD_DIR)/$@ .
	@echo "Boz OS compilado com sucesso! Arquivo gerado: boz.img"

# Regra de Limpeza
clean:
	rm -rf $(BUILD_DIR) boz.img
	@echo "Limpeza concluída!"

# Regra para rodar no QEMU
run: boz.img
	qemu-system-i386 -drive format=raw,file=boz.img

