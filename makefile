all:	assembler.c macro-expansion.c macro-expansion.h string-utility.c string-utility.h error-handling.c error-handling.h after-macro.c after-macro.h
	gcc -ansi -pedantic -Wall -c error-handling.c -o error-handling.o
	gcc -ansi -pedantic -Wall -c string-utility.c -o string-utility.o
	gcc -ansi -pedantic -Wall -c macro-expansion.c -o macro-expansion.o	
	gcc -ansi -pedantic -Wall -c after-macro.c -o after-macro.o	
	gcc -ansi -pedantic -Wall assembler.c macro-expansion.o	string-utility.o error-handling.o
