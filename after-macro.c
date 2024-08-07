#include "after-macro.h"

int begin_assembler(char *fname, char *after_label_fname, Word **word_array, Label **label_array, User_Output **out)
{
	int return_value;
	FILE *fp = fopen(after_label_fname, "r");
	if (!fp)
	{
		return 1;
	}
	return_value = first_after_macro_scan(fp, after_label_fname, word_array, label_array, out);
	fclose(fp);
	if (return_value)
		return 1;
	return 0;
}

int first_after_macro_scan(FILE *fp, char *fname, Word **word_array, Label **label_array, User_Output **out)
{
	char line[MAX_CHARS_IN_LINE], *line_ptr;
	int line_number, instruction_address, error_return, label_index;
	line_number = instruction_address = 0;
	while (fgets(line, MAX_CHARS_IN_LINE, fp))
	{
		line_ptr = line;
		++line_number;
		if ((line_ptr = strchr(line, ':')))
		{
			after_macro_handle_label(line, line_ptr, line_number, instruction_address, label_array, &error_return, &label_index);
			if (error_return == ERROR_EXCEEDED_LABEL_ARRAY_LIMIT || error_return == ERROR_PROGRAM_MEMORY_ALLOCATION)
			{	/* program runtime critical error */
				log_error(out, fname, line, error_return, line_number, &error_return);
				if (error_return)
						return ERROR_OUTPUT_MEMORY_ALLOCATION;
				return 1;
			}
			if (error_return)
			{	/* source file code error */
				log_error(out, fname, line, error_return, line_number, &error_return);
				if (error_return)
						return ERROR_OUTPUT_MEMORY_ALLOCATION;
			}
			++line_ptr;
			line_ptr = skip_blanks(line_ptr);
		}
		/* easier to assume (and rightly so) that no label is in existence from here on and just save words and their types */
		if (!line_ptr)
			line_ptr = skip_blanks(line);
		/* should save word after word, incrementing instruction_address each time */
		after_macro_save_words(line_ptr, instruction_address, &error_return, word_array);
		if (error_return == ERROR_EXCEEDED_WORD_ARRAY_LIMIT || error_return == ERROR_PROGRAM_MEMORY_ALLOCATION)
		{	/* program runtime critical error */
			log_error(out, fname, line, error_return, line_number, &error_return);
			if (error_return)
					return ERROR_OUTPUT_MEMORY_ALLOCATION;
			return 1;
		}
		if (error_return)
		{	/* source file code error */
			log_error(out, fname, line, error_return, line_number, &error_return);
			if (error_return)
					return ERROR_OUTPUT_MEMORY_ALLOCATION;
		}
		/* after_macro_handle_data_type(line, line_ptr, line_number, instruction_address, label_array, &error_return, &label_index); */
	}
	return 0;
}

void after_macro_handle_label(char *line, char *colon_ptr, int line_number, int instruction_address, Label **label_array, int *error_return, int *stored_label_index)
{
	/* assumes colon_ptr points to ':' in label */
	int return_value;
	char word[MAX_WORD_LENGTH];
	return_value = verify_label_syntax(line, colon_ptr);
	if (return_value)
	{
		*error_return = return_value;
		return;
	}
	return_value = verify_label_unique(line, colon_ptr, label_array);
	if (return_value)
	{
		*error_return = return_value;
		return;
	}
	*colon_ptr = '\0';
	read_word(line, word);
	*colon_ptr = ':';
	if (!verify_not_reserved(word) || get_command_op_code_decimal(word) != -1)
	{
		*error_return = ERROR_LABEL_RESERVED_WORD;
		return;
	}
	/* handle case of "LABEL: .extern" lines, in which we ignore the label */
	read_word(colon_ptr + 1, word);
	if (strcmp(word, ".extern"))
	{
		return_value = save_label(line, colon_ptr, label_array, line_number, instruction_address, stored_label_index);
		if (return_value)
		{
			*error_return = return_value;
			return;
		}
		save_label_data_type(*label_array, *stored_label_index, word);
	}
	*error_return = 0;
}

void after_macro_save_words(char *line, int instruction_address, int *error_return, Word **word_array)
{
	/* assume non blank at *line */
	char command_code, *command_end_ptr, is_command;
	int return_value;
	command_end_ptr = line;
	return_value = after_macro_verify_command_till_arguments(&command_end_ptr, &command_code);
	if (return_value || command_code == BLANK_LINE) {
		*error_return = return_value;
		return;
	}
	/* line command_end_ptr now points to the first blank after the command */
	command_end_ptr = skip_blanks(command_end_ptr);
	if ((is_command = (command_code >= 0 && command_code <= 15)))
	{
		return_value = save_word(instruction_address++, (int)command_code, is_command, word_array);
		if (return_value) {
			*error_return = return_value;
			return;
		}
		return_value = after_macro_save_command_arguments(command_end_ptr, instruction_address, command_code, word_array);
		if (return_value) {
			*error_return = return_value;
			return;
		}
	}
	else {
/*		command_invalid = after_macro_save_declaration_words(line, instruction_address, ? );*/
	}
	if (return_value) 
	{
		*error_return = return_value;
		return;
	}
	/*save_command_word(command_code, op_build, word_array);*/
	*error_return = 0;
}

int after_macro_verify_command_till_arguments(char **line, char *command_code)
{
	/* assume the first character is non blank */
	char word[MAX_WORD_LENGTH];
	int word_len = read_word_delimited(*line, word, ",");
	if (!word_len)
	{
		if (**line == ',')
			return ERROR_COMMA_BEFORE_COMMAND;
		*command_code = BLANK_LINE;
		return 0; /* blank line, no words are saved which is fine */
	}
	*command_code = get_command_op_code_decimal(word);
	if (*command_code == -1)
	{
		/* not a command from the 16 commands available, must be a declaration to be a part of the language */
		if (**line != '.') /* this reduces some of the checks */
			return ERROR_COMMAND_UNKNOWN;
		*command_code = get_declaration_type(word);
		if (!(*command_code))
			return ERROR_COMMAND_UNKNOWN;
		*line += word_len;
		*line = skip_blanks(*line);
		if (**line == ',')
			return ERROR_COMMA_AFTER_COMMAND;
		return 0;
	}
	*line += word_len;
	*line = skip_blanks(*line);
	if (**line == ',')
		return ERROR_COMMA_AFTER_COMMAND;
	return 0;
}

int after_macro_save_command_arguments(char *line, int instruction_address, char opcode, Word **word_array)
{
	/* assume line points to a non ',' character */
	/* assume opcode is between in [0,15] */
	char word[MAX_WORD_LENGTH], number_of_arguments_for_command;
	int word_len;
	if (opcode <= 4)
	{
		/* expect 2 arguments for commands with such opcode */
		word_len = read_word_delimited(line, word, ","); /* must be greater than one length due to assumption */
		save_word(instruction_address++, 0, 0, word_array);
		line += word_len;
		line = skip_blanks(line);
		if (*line != ',')
			return ERROR_BLANK_BETWEEN_ARGUMENTS;
		++line;
		line = skip_blanks(line);
		word_len = read_word_delimited(line, word, ",");
		if (!word_len)
			return ERROR_CONSEQUTIVE_COMMAS;
		save_word(instruction_address, 0, 0, word_array);
		line += word_len;
		line = skip_blanks(line);
		if (*line == ',' && *line != '\n')
		{
			if (read_word_delimited(line, word, ","))
				return ERROR_TOO_MANY_ARGUMENTS;
			return ERROR_COMMA_AFTER_ARGUMENTS;
		}
		return 2;
	}
	else if (opcode <= 13)
	{
		/* expect 1 arguments for commands with such opcode */
		word_len = read_word_delimited(line, word, ","); /* must be greater than one length due to assumption */
		save_word(instruction_address++, 0, 0, word_array);
		line += word_len;
		line = skip_blanks(line);
		if (*line == ',')
			return ERROR_COMMA_AFTER_ARGUMENTS;
		if (read_word_delimited(line, word, ","))
			return ERROR_BLANK_BETWEEN_ARGUMENTS;
		return 1;
	}
	/* expect 0 arguments, due to assumption we already have an issue in syntax */
	return ERROR_TOO_MANY_ARGUMENTS;
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

int save_label(char *line, char *end, Label **label_array, int line_number, int instruction_address, int *stored_label_index)
{
	static int next_label_array_index;
	char word[MAX_WORD_LENGTH];
	Label *tmp;
	int error_return = 0;
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
	*stored_label_index = next_label_array_index;
	tmp = increment_label_array_index(*label_array, ++next_label_array_index, &error_return);
	if (tmp)
		*label_array = tmp;
	return error_return;
}

int save_word(int instruction_address, int value, char is_command, Word **word_array)
{
	static int next_word_array_index;
	/*char word[MAX_WORD_LENGTH];*/
	Word *tmp_for_allocation, *word_array_dereference;
	int error_return = 0;
	if (!word_array)
	{
		next_word_array_index = 0;
		return 0;
	}
	word_array_dereference = *word_array;
	word_array_dereference[next_word_array_index].decimal_instruction_address = instruction_address;
	word_array_dereference[next_word_array_index].is_command = is_command; /* commands will propertly have their structure handles in the second scan */
	word_array_dereference[next_word_array_index].Data.value = value;
	/* save value */
	tmp_for_allocation = increment_word_array_index(word_array_dereference, ++next_word_array_index, &error_return);
	if (tmp_for_allocation)
		*word_array = tmp_for_allocation;
	return error_return;
}

int verify_line_syntax(char *line)
{
	/* assumes line points past a verified syntax label */ 
	char word[MAX_WORD_LENGTH];
	int type;
	line = skip_blanks(line);
	/* should try to read .YYY or <command> */
	if (*line == ',')
		return ERROR_COMMA_BEFORE_COMMAND;
	if (*line != '.')
	{
		read_word(line, word);
		type = get_command_op_code_decimal(word);
	}
	else
	{
		++line;
		read_word(line, word);
		type = get_command_op_code_decimal(word);
		/*type = get_data_type(word);*/
	}
	if (type == -1)
		return ERROR_COMMAND_UNKNOWN;
	return 0;
}

char get_command_op_code_decimal(char *op)
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
	save_label(NULL, NULL, NULL, 0, 0, NULL);
	return;
}

Word * increment_word_array_index(Word *word_array, int next_word_index, int *error_return)
{
	Word *temp_word_array = NULL;
	if (next_word_index % WORD_ARRAY_INIT_SIZE == 0)
		temp_word_array = allocate_word_array_memory(word_array, error_return); 
	return temp_word_array;
}

Word * allocate_word_array_memory(Word *word_array, int *error_return)
{
	static char word_multiplier_factor; /* acts as a multiplier to increase word_array size with jumps of WORDINIT */
	Word *temp_word_array; 
	int last_initialized;
	size_t alloc_size;
	if (!word_array)
	{
		word_multiplier_factor = 1;
		return NULL;
	}
	last_initialized = word_multiplier_factor++ * WORD_ARRAY_INIT_SIZE;
	alloc_size = last_initialized + WORD_ARRAY_INIT_SIZE;
	if (word_multiplier_factor > WORD_ARRAY_SIZE_MULTIPLIER_LIMIT)
	{
		*error_return = ERROR_EXCEEDED_WORD_ARRAY_LIMIT;
		return NULL; 
	}
	temp_word_array = (Word *)realloc(word_array, alloc_size * sizeof(Word)); /* realloc of macro_array */
	if (!temp_word_array)
	{
		*error_return = ERROR_PROGRAM_MEMORY_ALLOCATION;	
		return NULL; /* couldn't allocate enough memory for reallocation/allocation */
	}
	/* 0 out of newly allocated memory for decimal_instruction_address field (type 0 means there is no content)  */
	for (; last_initialized < alloc_size; ++last_initialized)
		temp_word_array[last_initialized].decimal_instruction_address = 0;
	*error_return = 0;
	return temp_word_array;
}

Word * init_word_array_memory()
{
	Word *temp_word_array; 
	size_t alloc_size = WORD_ARRAY_INIT_SIZE;
	temp_word_array = (Word *)calloc(alloc_size, sizeof(Word));
	reset_word_array_indices();
	return temp_word_array;
}

void reset_word_array_indices()
{
	allocate_word_array_memory(NULL, NULL);
	save_word(0, 0, 0, NULL);
	return;
}

void print_labels(Label *label_array)
{
	int i = 0;
	while (label_array[i].decimal_instruction_address)
		printf("%s\n", label_array[i++].name);
}

char save_label_data_type(Label *label_array, int label_index, char *word)
{
	char data_type = get_declaration_type(word);
	(label_array)[label_index].label_type = data_type;
	return data_type;
}

char get_declaration_type(char *word)
{
	if (!strcmp(word, ".data"))
		return TYPE_DATA;
	else if (!strcmp(word, ".string"))
		return TYPE_STRING;
	else if (!strcmp(word, ".extern"))
		return TYPE_EXTERN;
	else if (!strcmp(word, ".entry"))
		return TYPE_ENTRY;
	return 0;
}
