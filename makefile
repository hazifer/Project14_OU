all:	assembler.c macro-expansion.c macro-expansion.h
	gcc -ansi -pedantic -Wall -c macro-expansion.c -o macro-expansion.o	
	gcc -ansi -pedantic -Wall assembler.c macro-expansion.o	
