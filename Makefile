.POSIX:
.PHONY: clean run run-img

main.iso: main.elf
	cp '$<' iso/boot
	grub2-mkrescue -o '$@' iso/

# main.elf is the the multiboot file.
main.elf: entry.o main.o
	ld -m elf_i386 -nostdlib -T linker.ld -o '$@' $^

entry.o: entry.asm
	nasm -f elf32 '$<' -o '$@'

main.o: src/main.cpp
	clang++ -c -m32 -Os -fno-builtin -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -o '$@' -Wall -Wextra '$<'

clean:
	rm -f *.elf *.o iso/boot/*.elf *.img *.iso

run: main.elf
	qemu-system-i386 -kernel '$<'

run-img: main.iso
	qemu-system-i386 -hda '$<'
