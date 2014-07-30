include Makefile.am


LDFLAGS=-Tlink.ld
LDEMULATION=elf_i386
TARGET=elf32-i386
KERN_OUTPUT=kernel.bin

LIBS = 	libs/string.o \
	libs/ulib.o

DRIVERS = drivers/vga.o \
	  drivers/pit.o

ALLSOURCES = arch/i386/boot.o \
	     arch/i386/cpu/asm-cpu.o \
	     arch/i386/cpu/int.o \
	     arch/i386/cpu/gdt.o \
	     arch/i386/cpu/idt.o \
	     arch/i386/cpu/isr.o \
	     arch/i386/cpu/regs.o \
             arch/i386/cpu/irq.o \
	     init/main.o \
	     kernel/kprintf.o \
	     kernel/kern_debug.o \
	     kernel/panic.o \
	     kernel/version.o


SOURCES = $(ALLSOURCES) $(LIBS) $(DRIVERS)

CFLAGS =  -fno-stack-protector -std=c99 -m32 -fno-builtin  -nodefaultlibs  -I"."  -I"include/"

AFLAGS = -felf

all: $(SOURCES) link

clean:
	-rm $(SOURCES) $(KERN_OUTPUT)

link:
	$(LD) -m $(LDEMULATION) $(LDFLAGS) -o $(KERN_OUTPUT) -b $(TARGET) $(SOURCES)

qemu-run:
	qemu -kernel $(KERN_OUTPUT)
