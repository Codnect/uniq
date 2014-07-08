include Makefile.am


LIBS = #
LDFLAGS=-Tlink.ld
LDEMULATION=elf_i386
TARGET=elf32-i386

ALLSOURCES = arch/i386/boot.o \
	     init/main.o



SOURCES = $(ALLSOURCES) $(LIBS)

CFLAGS =  -fno-stack-protector -std=c99 -m32 -fno-builtin  -nodefaultlibs  -I"."  -I"include/"

AFLAGS = -felf

all: $(ALLSOURCES) link

clean:
	-rm $(ALLSOURCES)

link:
	$(LD) -m $(LDEMULATION) $(LDFLAGS) -o kernel.bin -b $(TARGET) $(ALLSOURCES)

