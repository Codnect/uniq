ENTRY(kentry)
phys = 0x100000;

SECTIONS{

	/*
	 * kernel kod kismi
	 */
	.text phys : AT(phys){
		code = .;
		*(.mboot*)
		*(.text*)
		*(.rodata*)
		. = ALIGN(4096);
	}

	/*
	 * kernel data
	 */
    	.data : AT(phys + (data - code)){
        	data = .;
        	*(.data*)
        	. = ALIGN(4096);
    	}

	/*
	 * statik ve deger verilmemis tanimlamalar
	 */
    	.bss : AT(phys + (bss - code)){
        	bss = .;
        	*(.bss*)
        	. = ALIGN(4096);
    	}
	
	/*
	 * kernel sonu
	 */
    	end = .;

}
