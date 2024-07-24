#include "after-macro.h"

int get_command_op_code_decimal(char *word)
{
	int i;
	char *command_set[NUMBER_OF_COMMANDS] = { "mov", "cmp", "add", "sub", 
						  "lea", "clr", "not", "inc", 
						  "dec", "jmp", "bne", "red", 
						  "prn", "jsr", "rts", "stop", };
	for (i = 0; i < NUMBER_OF_COMMANDS; ++i)
		if (!strcmp(word, command_set[i]))
				return i;
	return -1;
}
