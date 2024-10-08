.POSIX:
.PHONY: clean run run-img debug launch-gdb all

# Variables
CXX := $(shell if command -v clang++ > /dev/null; then echo clang++; else echo g++; fi)
QEMU = qemu-system-i386
SRC = src/main.cpp src/VGA.cpp
ENTRY = entry.asm
LINKER_SCRIPT = linker.ld
ISO_DATA_DIR = iso/boot
ISO_DIR = iso
OUTPUT_ELF = main.elf
OUTPUT_ISO = main.iso
OBJ = entry.o main.o VGA.o
# Compilation flags
CFLAGS = -c -m32 -Os -fno-builtin -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -Wall -Wextra
ASFLAGS = -f elf32
LD_FLAGS = -m elf_i386 -nostdlib -T $(LINKER_SCRIPT)

# Targets
all: $(OUTPUT_ISO)

# Compile object files from source
%.o: src/%.cpp
	$(CXX) $(CFLAGS) -o $@ $<

%.o: %.asm
	nasm $(ASFLAGS) $< -o $@

# Link object files to create ELF executable
$(OUTPUT_ELF): $(OBJ)
	ld $(LD_FLAGS) -o $@ $(OBJ)

$(OUTPUT_ISO): $(OUTPUT_ELF) iso/boot/grub/grub.cfg
	mkdir -p $(ISO_DATA_DIR)/grub
	cp $< $(ISO_DATA_DIR)
	grub2-mkrescue -v -o $@ $(ISO_DIR)

clean:
	rm -f *.elf *.o $(ISO_DIR)/*.elf *.img *.iso

run: $(OUTPUT_ELF)
	$(QEMU) -kernel $<

debug: $(OUTPUT_DEBUG_ELF)
	$(QEMU) -s -S -kernel $<

launch-gdb:
	gdb $< -ex 'target remote localhost:1234' -ex 'layout src' -ex 'layout regs'

run-img: $(OUTPUT_ISO)
	$(QEMU) -hda $<
