all:	assembler.c macro-expansion.c macro-expansion.h string_utility.c string_utility.h error_handling.c error_handling.h
	gcc -ansi -pedantic -Wall -c error_handling.c -o error_handling.o
	gcc -ansi -pedantic -Wall -c string_utility.c -o string_utility.o
	gcc -ansi -pedantic -Wall -c macro-expansion.c -o macro-expansion.o	
	gcc -ansi -pedantic -Wall assembler.c macro-expansion.o	string_utility.o error_handling.o
