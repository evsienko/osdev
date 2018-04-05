del floppy.img
del bin\Boot1.bin
del bin\KRNLDR.SYS

nasm -f bin src\Stage1\Boot1.asm -o bin\Boot1.bin -i src\Stage1\
nasm -f bin src\Stage2\Stage2.asm -o bin\KRNLDR.SYS -i src\Stage2\
nasm -f bin src\Kernel\Stage3.asm -o bin\KRNL.SYS -i src\Kernel\

tools\fat_imgen-2.2.4\fat_imgen.exe -c -s bin\Boot1.bin -i bin\KRNLDR.SYS -f floppy.img
tools\fat_imgen-2.2.4\fat_imgen.exe -m -i bin\KRNL.SYS -f floppy.img