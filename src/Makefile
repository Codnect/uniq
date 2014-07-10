include Makefile.am


LIBS = #
DRIVERS = drivers/vga.o

LDFLAGS=-Tlink.ld
LDEMULATION=elf_i386
TARGET=elf32-i386

ALLSOURCES = arch/i386/boot.o \
	     init/main.o



SOURCES = $(ALLSOURCES) $(LIBS) $(DRIVERS)

CFLAGS =  -fno-stack-protector -std=c99 -m32 -fno-builtin  -nodefaultlibs  -I"."  -I"include/"

AFLAGS = -felf

all: $(SOURCES) link

clean:
	-rm $(SOURCES)

link:
	$(LD) -m $(LDEMULATION) $(LDFLAGS) -o kernel.bin -b $(TARGET) $(SOURCES)
