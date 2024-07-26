all:	assembler.c macro-expansion.c macro-expansion.h string-utility.c string-utility.h error-handling.c error-handling.h after-macro.c after-macro.h
	gcc -ansi -pedantic -Wall -c error-handling.c string-utility.c macro-expansion.c after-macro.c	
	gcc -ansi -pedantic -Wall assembler.c error-handling.o string-utility.o macro-expansion.o after-macro.o
