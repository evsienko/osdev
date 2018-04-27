/*
=========================================================================
	entry.cpp

	kernel entry point called from bootloader
=========================================================================
*/

extern void __cdecl  InitializeConstructors ();
extern void __cdecl  Exit ();
extern int	__cdecl  main ();

//! kernel entry point is called by boot loader
void __cdecl  kernel_entry () {

#ifdef ARCH_X86

	// Set registers for protected mode
	_asm {
		cli
		mov ax, 10h
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
	}
#endif //ARCH_X86

	InitializeConstructors();
	main ();
	Exit ();

#ifdef ARCH_X86
	_asm {
		cli
		hlt
	}
#endif

	for (;;);
}
