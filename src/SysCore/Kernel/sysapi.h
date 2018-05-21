
#include <hal.h>
#include "DebugDisplay.h"

#define MAX_SYSCALL 3

void* _syscalls[] = {

	DebugPrintf
};

int idx = 0;
void* fnct;

_declspec(naked)
void syscall_dispatcher () {

	_asm mov [idx], eax

	//! bounds check
	if (idx>=MAX_SYSCALL)
		_asm iretd

	//! get service
	fnct = _syscalls[idx];

	//! and call service
	_asm {
		push edi
		push esi
		push edx
		push ecx
		push ebx
		call fnct
		add esp, 20
		iretd
	}
}

//! from idt.h
#define I86_IDT_DESC_RING3		0x60

void syscall_init () {

	//! install interrupt handler!
	setvect (0x80, syscall_dispatcher, I86_IDT_DESC_RING3);
}

