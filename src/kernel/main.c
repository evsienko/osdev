#include "stdlib.h"
#include "interrupts.h"
#include "tty.h"

typedef struct {
	uint64 base;
	uint64 size;
} BootModuleInfo;

void kernel_main(uint8 boot_disk_id, void *memory_map, BootModuleInfo *boot_module_list) {
	init_interrupts();
	init_tty();
	set_text_attr(15);
	printf("Welcome to MyOS!\n");
	while (true) {
	 	char buffer[256];
	 	out_string("Enter string: ");
	 	in_string(buffer, sizeof(buffer));
	 	printf("You typed: %s\n", buffer);
	}
}