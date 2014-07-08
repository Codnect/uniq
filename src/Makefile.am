PROMPT = " -> " 

LD = @echo "\t \033[1;92m" $(PROMPT) LD "\t \033[0m" $<  && ld  
CC = @echo  "\t \033[1;94m" $(PROMPT) CC "\t \033[0m" $<  && gcc 
ASM = @echo "\t \033[1;93m" $(PROMPT) ASM  "\t \033[0m" $<  && nasm 

.c.o: 

	$(CC) -c $(CFLAGS) -o "$@" "$<"
.s.o:
	$(ASM) $(AFLAGS) $<
	
