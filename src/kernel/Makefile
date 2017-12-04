LDFLAGS = -mi386pe
CFLAGS = -m32 -ffreestanding

all: script.ld startup.o stdlib.o main.o memory_manager.o interrupts.o tty.o
	ld $(LDFLAGS) -T script.ld -o kernel.bin startup.o stdlib.o main.o memory_manager.o interrupts.o tty.o
	objcopy kernel.bin -O binary
startup.o: startup.i386.asm
	fasm startup.i386.asm startup.o
stdlib.o: stdlib.c stdlib.h
	gcc -c $(CFLAGS) -o stdlib.o stdlib.c
main.o: main.c stdlib.h interrupts.h tty.h
	gcc -c $(CFLAGS) -o main.o main.c
memory_manager.o: memory_manager.c memory_manager.h stdlib.h
	gcc -c $(CFLAGS) -o memory_manager.o memory_manager.c
interrupts.o: interrupts.c interrupts.h stdlib.h
	gcc -c $(CFLAGS) -o interrupts.o interrupts.c
tty.o: tty.c tty.h stdlib.h
	gcc -c $(CFLAGS) -o tty.o tty.c
clean:
	rm -v *.o kernel.bin