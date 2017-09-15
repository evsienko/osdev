".\third party\fasm\fasm" src\boot.asm .\bin\boot.bios.bin

".\third party\dd" if=.\bin\boot.bios.bin of=.\bin\boot_sector.bin bs=512 count=1
".\third party\dd" if=.\bin\boot.bios.bin of=.\disk\boot.bin bs=1 skip=512

.\src\tools\make_listfs of=.\bin\disk.img bs=512 size=2880 boot=.\bin\boot_sector.bin src=.\disk