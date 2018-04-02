del floppy.img
del Boot1.bin
del KRNLDR.SYS

nasm -f bin src\Stage1\Boot1.asm -o bin\Boot1.bin
nasm -f bin src\Stage2\Stage2.asm -o bin\KRNLDR.SYS

tools\fat_imgen-2.2.4\fat_imgen.exe -c -s bin\Boot1.bin -i bin\KRNLDR.SYS -f floppy.img

