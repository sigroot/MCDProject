objects = main.o

all: main.S68

main.S68: main.o #$(wildcard *.o)
	m68k-elf-ld -o output/main.S68 -T linker/linker.ld output/main.o

main.o:
	m68k-elf-gcc -Wall src/main.c -m68000 -c -o output/main.o -ffreestanding 

test.o: main.o
	m68k-elf-ld -o output/test.o -T linker/linker.ld output/main.o --oformat elf32-m68k
