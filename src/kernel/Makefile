all: startup.o main.o script.ld
	ld -T script.ld -o kernel.bin startup.o main.o
	objcopy kernel.bin -O binary
startup.o: startup.i386.asm
	fasm startup.i386.asm startup.o
main.o: main.c
	gcc -c -m32 -ffreestanding -o main.o main.c
clean:
	del -v *.o kernel.bin