#include "after-macro.h"

int begin_assembler(char *fname, char *after_label_fname, Label *label_array, User_Output **out)
{
	int return_value;
	FILE *fp = fopen(after_label_fname, "r");
	if (!fp)
	{
		return 1;
	}
	return_value = first_after_macro_scan(fp, after_label_fname, label_array, out);
	fclose(fp);
	if (return_value)
		return 1;
	return 0;
}

int first_after_macro_scan(FILE *fp, char *fname, Label *label_array, User_Output **out)
{
	char line[MAX_CHARS_IN_LINE], *p;
	int line_number, instruction_address, return_value, i, error_return;
	line_number = instruction_address = return_value = error_return = 0;
	while (fgets(line, MAX_CHARS_IN_LINE, fp))
	{
		++line_number;
		if ((p = strchr(line, ':')))
		{
			return_value = verify_label_syntax(line, p);
			if (return_value)
			{
				log_error(out, fname, line, return_value, line_number, &error_return);
				continue;
			}
			/*
			save_label(line, p, label_array, line_number, instruction_address);*/
			/*
			if (return_value)
			{
				log_error(out, fname, line, return_value, line_number);
				continue;
			}*/
		}
	}
	/* check if error and return accordingly */
	/*
	i = 0;
	while (label_array[i].decimal_instruction_address)
	{
		printf("label: %s\n", label_array[i].name);
		++i;
	}*/
	return 0;
}

int verify_label_syntax(char *line, char *end)
{
	char word[MAX_WORD_LENGTH];
	int len;
	if (strchr(end + 1, ':'))
		return ERROR_LABEL_MULTIPLE_COLON;
	*end = '\0';
	if (!(len = read_word(line, word)))
	{
		*end = ':';
		return ERROR_LABEL_EMPTY;
	}
	line = skip_blanks(line);
	line += len;
	if (line < end)
	{
		line = skip_blanks(line);
		if (line != end)
		{
			*end = ':';
			return ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON;
		}
	}
	*end = ':';
	return 0;
}

int save_label(char *line, char *end, Label *label_array, int line_number, int instruction_address)
{
	/*char word[MAX_WORD_LENGTH];*/
	*end = '\0';
	return 0;
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
	int last_initialized = label_multiplier_factor++ * LABEL_ARRAY_INIT_SIZE; 
	size_t alloc_size = last_initialized + LABEL_ARRAY_INIT_SIZE;
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
	/* 0 out of newly allocated memory for decimal_instruction_address field (type 0 means there is no content)  */
	for (; last_initialized < alloc_size; ++last_initialized)
		temp_label_array[last_initialized].decimal_instruction_address = 0;
	return temp_label_array;
}
