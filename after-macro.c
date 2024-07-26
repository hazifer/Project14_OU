#include "after-macro.h"

int begin_assembler(char *fname, char *after_label_fname, Label *label_array, User_Output *out)
{
	int return_value;
	FILE *fp = fopen(after_label_fname, "r");
	if (!fp)
		return 1;
	return_value = first_after_macro_scan(fp, after_label_fname, label_array, out);
	if (!return_value)
		return 1;
	fclose(fp);
	return 0;
}

int first_after_macro_scan(FILE *fp, char *fname, Label *label_array, User_Output *out)
{
	char line[MAX_CHARS_IN_LINE], *p;
	int line_number, instruction_address, return_value;
	line_number = instruction_address = return_value = 0;
	while (fgets(line, MAX_CHARS_IN_LINE, fp))
	{
		++line_number;
		if ((p = strchr(line, ':')))
			return_value = save_label(line, p, label_array, line_number, instruction_address, fname, out);
	}
	return 0;
}

int save_label(char *line, char *end, Label *label_array, int line_number, int instruction_address, char *fname, User_Output *out)
{
	char word[MAX_OP_LENGTH];
	*end = '\0';
	if (!read_word(line, word))
	{
		/* not necessarily empty! */
		log_error(out, fname, line, ERROR_LABEL_EMPTY, line_number);
		return 1;
	}
}

int get_command_op_code_decimal(char *op)
{
	int i;
	char *command_set[NUMBER_OF_COMMANDS] = { "mov", "cmp", "add", "sub", 
						  "lea", "clr", "not", "inc", 
						  "dec", "jmp", "bne", "red", 
						  "prn", "jsr", "rts", "stop" };
	for (i = 0; i < NUMBER_OF_COMMANDS; ++i)
		if (!strcmp(op, command_set[i]))
				return i;
	return -1;
}

Label * allocate_label_array_memory(Label *label_array, User_Output *out)
{
	static char label_multiplier_factor; /* acts as a multiplier to increase label_array size with jumps of LABELINIT */
	Label *temp_label_array; 
	size_t alloc_size = ++label_multiplier_factor * LABEL_ARRAY_INIT_SIZE; /* number of Label structs to allocate memory for */
	if (label_multiplier_factor > LABEL_ARRAY_SIZE_MULTIPLIER_LIMIT)
	{
		out->message_type = ERROR_EXCEEDED_LABEL_ARRAY_LIMIT;
		return NULL; 
	}
	if (label_array) 
		temp_label_array = (Label *)realloc(label_array, alloc_size * sizeof(Label)); /* realloc of macro_array */
	else
		temp_label_array = (Label *)malloc(alloc_size * sizeof(Label)); /* malloc incase it wasn't allocated yet */
	if (!temp_label_array)
	{
		out->message_type = ERROR_PROGRAM_MEMORY_ALLOCATION;	
		return NULL; /* couldn't allocate enough memory for reallocation/allocation */
	}
	/* allocation of memory for each struct */
	return temp_label_array;
}
