OUTPUT_FORMAT(pei-i386)   
ENTRY(_start)

KERNEL_BASE = 0xFFC00000;

SECTIONS {
 	.text KERNEL_BASE : {
 		KERNEL_CODE_BASE = .;
		*(.text)
		*(.code)
		*(.rodata*)
	}
	.data ALIGN(0x1000) : {
		KERNEL_DATA_BASE = .;
		*(.data)
  	}
	.bss ALIGN(0x1000) : {
		KERNEL_BSS_BASE = .;
		*(.bss)
	}
	.empty ALIGN(0x1000) - 1 : {
		BYTE(0)
		KERNEL_END = .;
	}
} 