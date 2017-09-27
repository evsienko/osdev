call clean.bat

mingw32-make -C src
copy /y src\boot\boot.bios.bin bin\
copy /y src\kernel\kernel.bin bin\
copy /y src\make_listfs\make_listfs.exe bin\

dd if=bin\boot.bios.bin of=bin\boot_sector.bin bs=512 count=1
dd if=bin\boot.bios.bin of=disk\boot.bin bs=1 skip=512

copy /y bin\kernel.bin disk\kernel.bin

bin\make_listfs of=disk.img bs=512 size=2880 boot=bin\boot_sector.bin src=.\disk 
move /y disk.img disk\disk.img

echo "Press Enter to continue..."