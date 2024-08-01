#include "after-macro.h"

int begin_assembler(char *fname, char *after_label_fname, Label **label_array, User_Output **out)
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

int first_after_macro_scan(FILE *fp, char *fname, Label **label_array, User_Output **out)
{
	char line[MAX_CHARS_IN_LINE], *p;
	int line_number, instruction_address, return_value, error_return;
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
			return_value = verify_label_unique(line, p, label_array);
			if (return_value)
			{
				log_error(out, fname, line, return_value, line_number, &error_return);
				continue;
			}
			return_value = save_label(line, p, label_array, line_number, instruction_address);
			if (return_value)
			{
				log_error(out, fname, line, return_value, line_number, &error_return);
				continue;
			}
			return_value = verify_line_syntax(p + 1);
			if (return_value)
			{
				log_error(out, fname, line, return_value, line_number, &error_return);
				continue;
			}
			instruction_address += count_words_in_line(p + 1, ",", MAX_WORD_LENGTH);
		}
	}
	/* check if error and return accordingly */
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
	if (!isalpha(word[0]))
	{
		*end = ':';
		return ERROR_LABEL_NOT_BEGIN_WITH_ALPHA;
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

int verify_label_unique(char *line, char *end, Label **label_array)
{
	char word[MAX_WORD_LENGTH];
	int i = 0;
	*end = '\0';
	read_word(line, word);
	*end = ':';
	while ((*label_array)[i].decimal_instruction_address)
		if (!strcmp((*label_array)[i++].name, word))
				return ERROR_LABEL_DUPLICATE;
	return 0;
}

int save_label(char *line, char *end, Label **label_array, int line_number, int instruction_address)
{
	static int next_label_array_index;
	char word[MAX_WORD_LENGTH];
	int error_return;
	Label *tmp;
	if (!label_array)
	{
		next_label_array_index = 0;
		return 0;
	}
	*end = '\0';
	read_word(line, word);
	*end = ':';
	strcpy((*label_array)[next_label_array_index].name, word);
	(*label_array)[next_label_array_index].decimal_instruction_address = instruction_address + line_number;
	tmp = increment_label_array_index(*label_array, ++next_label_array_index, &error_return);
	if (tmp)
	{
		*label_array = tmp;
		return 0;
	}
	return error_return;
}

int verify_line_syntax(char *line)
{
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

Label * increment_label_array_index(Label *label_array, int next_label_index, int *error_return)
{
	Label *temp_label_array = NULL;
	if (next_label_index % LABEL_ARRAY_INIT_SIZE == 0)
		temp_label_array = allocate_label_array_memory(label_array, error_return); 
	return temp_label_array;
}

Label * allocate_label_array_memory(Label *label_array, int *error_return)
{
	static char label_multiplier_factor; /* acts as a multiplier to increase label_array size with jumps of LABELINIT */
	Label *temp_label_array; 
	int last_initialized;
	size_t alloc_size;
	if (!label_array)
	{
		label_multiplier_factor = 1;
		return NULL;
	}
	last_initialized = label_multiplier_factor++ * LABEL_ARRAY_INIT_SIZE;
	alloc_size = last_initialized + LABEL_ARRAY_INIT_SIZE;
	if (label_multiplier_factor > LABEL_ARRAY_SIZE_MULTIPLIER_LIMIT)
	{
		*error_return = ERROR_EXCEEDED_LABEL_ARRAY_LIMIT;
		return NULL; 
	}
	temp_label_array = (Label *)realloc(label_array, alloc_size * sizeof(Label)); /* realloc of macro_array */
	if (!temp_label_array)
	{
		*error_return = ERROR_PROGRAM_MEMORY_ALLOCATION;	
		return NULL; /* couldn't allocate enough memory for reallocation/allocation */
	}
	/* 0 out of newly allocated memory for decimal_instruction_address field (type 0 means there is no content)  */
	for (; last_initialized < alloc_size; ++last_initialized)
		temp_label_array[last_initialized].decimal_instruction_address = 0;
	*error_return = 0;
	return temp_label_array;
}

Label * init_label_array_memory()
{
	Label *temp_label_array; 
	size_t alloc_size = LABEL_ARRAY_INIT_SIZE;
	temp_label_array = (Label *)calloc(alloc_size, sizeof(Label));
	reset_label_array_indices();
	return temp_label_array;
}

void reset_label_array_indices()
{
	allocate_label_array_memory(NULL, NULL);
	save_label(NULL, NULL, NULL, 0, 0);
	return;
}
