LDFLAGS = -mi386pe
CFLAGS = -m32 -ffreestanding

all: startup.o stdlib.o main.o tty.o script.ld
	ld $(LDFLAGS) -T script.ld -o kernel.bin startup.o stdlib.o main.o tty.o
	objcopy kernel.bin -O binary
startup.o: startup.i386.asm
	fasm startup.i386.asm startup.o
stdlib.o: stdlib.c stdlib.h
	gcc -c $(CFLAGS) -o stdlib.o stdlib.c
main.o: main.c tty.h
	gcc -c $(CFLAGS) -o main.o main.c
tty.o: tty.c tty.h stdlib.h
	gcc -c $(CFLAGS) -o tty.o tty.c
clean:
	del -v *.o kernel.bin