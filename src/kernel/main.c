#include "stdlib.h"
#include "tty.h"

typedef struct {
	uint64 base;
	uint64 size;
} BootModuleInfo;

void kernel_main(uint8 boot_disk_id, void *memory_map, BootModuleInfo *boot_module_list) {
	init_tty();
	set_text_attr(0x1F);
	clear_screen();
	out_string("Hello world!\n");
}