include Makefile.am


LDFLAGS=-Tlink.ld
LDEMULATION=elf_i386
TARGET=elf32-i386
KERN_OUTPUT=kernel.bin

KERN_ISO=uniq-iso.iso

LIBS = 	libs/string.o \
	libs/ulib.o

DRIVERS = drivers/vga.o \
	  drivers/pit.o \
	  drivers/cmos.o

ALLSOURCES = arch/x86/boot.o \
	     arch/x86/cpu/asm-cpu.o \
	     arch/x86/cpu/int.o \
	     arch/x86/cpu/gdt.o \
	     arch/x86/cpu/idt.o \
	     arch/x86/cpu/isr.o \
	     arch/x86/cpu/regs.o \
             arch/x86/cpu/irq.o \
	     arch/x86/cpu/cpuid.o \
	     init/main.o \
	     kernel/kprintf.o \
	     kernel/kern_debug.o \
	     kernel/panic.o \
	     kernel/version.o \
	     mm/heap.o \
	     mm/mem.o


SOURCES = $(ALLSOURCES) $(LIBS) $(DRIVERS)

CFLAGS =  -fno-stack-protector -std=c99 -m32 -fno-builtin  -nodefaultlibs  -I"."  -I"include/"

AFLAGS = -felf

all: $(SOURCES) link

create-iso:
	mkdir -p iso/boot/grub
	cp ../tools/grub/* iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o $(KERN_ISO) iso

clean:
	-rm $(SOURCES) $(KERN_OUTPUT)

link:
	$(LD) -m $(LDEMULATION) $(LDFLAGS) -o $(KERN_OUTPUT) -b $(TARGET) $(SOURCES)

qemu-run:
	qemu -kernel $(KERN_OUTPUT)
