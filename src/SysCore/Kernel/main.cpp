/*
=========================================================================
	main.cpp

	main kernel program
=========================================================================
*/

#include <hal.h>
#include "exception.h"
#include "DebugDisplay.h"

/*
*	Our uber-1337 logo
*/
char* logo =
"\
    __  _______  _____\n\
   /  |/  / __ \\/ ___/\n\
  / /|_/ / / / /\\__ \\ Microcomputer Operating System \n\
 / /  / / /_/ /___/ / -------------------------------\n\
/_/  /_/\\____//____/  \n\n";


int _cdecl main () {

	//! Initialize hal driver
	hal_initialize ();

	//! enable all interrupts
	enable ();

	//! install our exception handlers
	setvect (0,(void (__cdecl &)(void))divide_by_zero_fault);
	setvect (1,(void (__cdecl &)(void))single_step_trap);
	setvect (2,(void (__cdecl &)(void))nmi_trap);
	setvect (3,(void (__cdecl &)(void))breakpoint_trap);
	setvect (4,(void (__cdecl &)(void))overflow_trap);
	setvect (5,(void (__cdecl &)(void))bounds_check_fault);
	setvect (6,(void (__cdecl &)(void))invalid_opcode_fault);
	setvect (7,(void (__cdecl &)(void))no_device_fault);
	setvect (8,(void (__cdecl &)(void))double_fault_abort);
	setvect (10,(void (__cdecl &)(void))invalid_tss_fault);
	setvect (11,(void (__cdecl &)(void))no_segment_fault);
	setvect (12,(void (__cdecl &)(void))stack_fault);
	setvect (13,(void (__cdecl &)(void))general_protection_fault);
	setvect (14,(void (__cdecl &)(void))page_fault);
	setvect (16,(void (__cdecl &)(void))fpu_fault);
	setvect (17,(void (__cdecl &)(void))alignment_check_fault);
	setvect (18,(void (__cdecl &)(void))machine_check_abort);
	setvect (19,(void (__cdecl &)(void))simd_fpu_fault);

	//! clear and init display
	DebugClrScr (0x13);
	DebugGotoXY (0,0);
	DebugSetColor (0x19);

	//! render text and logo
	DebugPuts (logo);
	DebugSetColor (0x17);
	DebugPuts ("Weee.... The PIC, PIT, and exception handlers are installed!\n\n");
	DebugPuts ("Hitting any key will fire the default HAL handlers as we do\n");
	DebugPuts ("not have a keyboard driver yet to install one.\n\n");
	DebugPrintf ("Cpu vender: %s \n\n", get_cpu_vender ());

	// Go into a loop--constantly display the current tick count
	for(;;) {
		DebugGotoXY (0,14);
		DebugPrintf ("Current tick count: %i", get_tick_count());
	}

	return 0;
}



