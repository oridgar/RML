void cli() {
	asm {
		cli
	}
}

void sti() {
	asm {
		sti
	}	
}

void call_int() {
	asm {
		mov al,00h
		int 80h
	}
}

void halt() {
	asm {
		mov al,01h
		int 80h
	}
}

void reboot() {
	asm {
		mov al,02h
		int 80h
	}
}