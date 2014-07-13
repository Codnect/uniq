include Makefile.am


LDFLAGS=-Tlink.ld
LDEMULATION=elf_i386
TARGET=elf32-i386
KERN_OUTPUT=kernel.bin

LIBS = 	libs/string.o \
	libs/ulib.o

DRIVERS = drivers/vga.o

ALLSOURCES = arch/i386/boot.o \
	     init/main.o



SOURCES = $(ALLSOURCES) $(LIBS) $(DRIVERS)

CFLAGS =  -fno-stack-protector -std=c99 -m32 -fno-builtin  -nodefaultlibs  -I"."  -I"include/"

AFLAGS = -felf

all: $(SOURCES) link

clean:
	-rm $(SOURCES) $(KERN_OUTPUT)

link:
	$(LD) -m $(LDEMULATION) $(LDFLAGS) -o $(KERN_OUTPUT) -b $(TARGET) $(SOURCES)
