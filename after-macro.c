#include "after-macro.h"

int begin_assembler(char *fname, char *after_label_fname, Word **word_array, Label **label_array)
{
	int first_scan_return_value, second_scan_return_value;
	FILE *fp = fopen(after_label_fname, "r");
	if (!fp)
	{
		return 1;
	}
	first_scan_return_value = first_after_macro_scan(fp, after_label_fname, word_array, label_array);
	fclose(fp);
	if (first_scan_return_value == ERROR_TERMINATE_ASSEMBLER)
		return 1;
	if (!first_scan_return_value)
	{
		return 1;
	}
	increment_data_type_labels_address(*label_array, first_scan_return_value);
	second_scan_return_value = 1;
	printf("\nsecond_scan_return_value%d", second_scan_return_value);
	/* in case either scans fail, we don't create the output files */
	return 0;
}

int first_after_macro_scan(FILE *fp, char *fname, Word **word_array, Label **label_array)
{
	char line[MAX_CHARS_IN_LINE], *line_ptr, label_set, error_flag, command_type;
	int line_number, instruction_count, error_return, label_index;
	line_number = error_flag = 0;
	instruction_count = BASE_IC_ADDRESS;
	while (fgets(line, MAX_CHARS_IN_LINE, fp))
	{
		label_set = error_return = 0;
		line_ptr = line;
		command_type = TYPE_CODE;
		++line_number;
		if ((line_ptr = strchr(line, ':')))
		{
			error_return = after_macro_handle_label(line, line_ptr, instruction_count, label_array, &label_index);
			if (error_return == ERROR_EXCEEDED_LABEL_ARRAY_LIMIT || error_return == ERROR_PROGRAM_MEMORY_ALLOCATION)
			{	/* program runtime critical error */
				print_error(fname, line, error_return, line_number);
				return ERROR_TERMINATE_ASSEMBLER;
			}
			else if (error_return == WARN_LABEL_IN_ENTRY_EXTERN_LINE)
				--instruction_count;
			if (error_return)
			{
				error_flag = 1;
				print_error(fname, line, error_return, line_number);
			}
			++line_ptr;
			line_ptr = skip_blanks(line_ptr);
			label_set = 1;
		}
		/* easier to assume (and rightly so) that no label is in existence from here on and just save words and their types */
		if (!line_ptr)
			line_ptr = skip_blanks(line);
		/* should save word after word, incrementing instruction_count each time */
		if (error_return == WARN_LABEL_IN_ENTRY_EXTERN_LINE) /* ignore instruction_count increment for lines of type LABEL: .extern X or LABEL: .entry X */
			after_macro_save_words(line_ptr, instruction_count, &error_return, word_array, label_array, &command_type);
		else
			instruction_count = after_macro_save_words(line_ptr, instruction_count, &error_return, word_array, label_array, &command_type);
		if (error_return)
		{
			if (error_return == ERROR_EXCEEDED_WORD_ARRAY_LIMIT || error_return == ERROR_PROGRAM_MEMORY_ALLOCATION)
			{	/* program runtime critical error */
				print_error(fname, line, error_return, line_number);
				return ERROR_TERMINATE_ASSEMBLER;
			}
			if (error_return != BLANK_LINE)
			{
				error_flag = 1;
				print_error(fname, line, error_return, line_number);
			}
			/* if line was blank without a label, there is no need to increment */
			if (error_return == BLANK_LINE && !label_set)
				continue;
		}
		/* set label type as data if needed */
		if (label_set && (command_type == TYPE_DATA || command_type == TYPE_STRING))
			(*label_array)[label_index].label_type = TYPE_DATA;
	}
	if (error_flag)
		return 0;
	return instruction_count;
}

int after_macro_handle_label(char *line, char *colon_ptr, int instruction_address, Label **label_array, int *stored_label_index)
{
	/* assumes colon_ptr points to ':' in label */
	int return_value;
	char word[MAX_WORD_LENGTH];
	return_value = verify_label_syntax(line, colon_ptr);
	if (return_value)
		return return_value;
	return_value = verify_label_unique(line, colon_ptr, label_array);
	if (return_value)
		return return_value;
	*colon_ptr = '\0';
	read_word(line, word);
	*colon_ptr = ':';
	if (!verify_not_reserved(word) || get_command_op_code_decimal(word) != -1)
		return ERROR_LABEL_RESERVED_WORD;
	/* ignore labels in .entry or .extern lines */
	read_word(colon_ptr + 1, word);
	if (!strcmp(word, ".extern") || !strcmp(word, ".entry"))
		return WARN_LABEL_IN_ENTRY_EXTERN_LINE;
	return_value = save_label(line, colon_ptr, NULL, label_array, instruction_address, stored_label_index);
	if (return_value)
		return return_value;
	return 0;
}

int after_macro_save_words(char *line, int instruction_address, int *error_return, Word **word_array, Label **label_array, char *command_type)
{
	/* assume non blank at *line */
	char command_code, *command_end_ptr;
	int return_value;
	command_end_ptr = line;
	return_value = after_macro_verify_command_till_arguments(&command_end_ptr, &command_code);
	if (return_value) {
		*error_return = return_value;
		return instruction_address;
	}
	if (command_code == BLANK_LINE)
	{
		*error_return = BLANK_LINE;
		return instruction_address;
	}
	if ((command_code >= 0 && command_code <= 15)) /* instruction type command */
	{
		/* save command word */
		return_value = save_word(instruction_address++, (int)command_code, 1, word_array);
		if (return_value) {
			*error_return = return_value;
			return instruction_address;
		}
		/* save argument words */
		instruction_address += after_macro_save_command_arguments(command_end_ptr, instruction_address, command_code, word_array, error_return);
	}
	else /* declaration type command (".data" ".string" etc) */
	{
		if (command_code == TYPE_DATA)
			*command_type = TYPE_DATA;
		instruction_address += after_macro_save_declaration_words(command_end_ptr, instruction_address, command_code, word_array, label_array, error_return);
	}
	return instruction_address;
}

int after_macro_save_declaration_words(char *line, int instruction_address, char opcode, Word **word_array, Label **label_array, int *error_return)
{
	/* assume the first character is a non blank */
	if (opcode == TYPE_STRING)
		return read_string_declaration_data(line, instruction_address, word_array, error_return);
	if (opcode == TYPE_DATA)
		return read_data_declaration_data(line, instruction_address, word_array, error_return);
	if (opcode == TYPE_ENTRY)
		return read_entry_declaration_data(line, instruction_address, label_array, error_return);
	if (opcode == TYPE_EXTERN)
		return read_extern_declaration_data(line, instruction_address, label_array, error_return);
	return 0;
}

/*int verify_label_unique(char *line, char *end, Label **label_array)*/
int read_extern_declaration_data(char *line, int instruction_address, Label **label_array, int *error_return)
{
	/* assume first character is not blank */
	char word[MAX_WORD_LENGTH], *word_ptr;
	int label_index, word_len;
	*error_return = 0;
	word_len = read_word(line, word);
	if (!word_len)
	{
		*error_return = ERROR_EXTERN_EMPTY_LABEL;
		return 0;
	}
	if (!isalpha(word[0]))
	{
		*error_return = ERROR_EXTERN_ILLEGAL_LABEL_NOT_BEGIN_WITH_ALPHA;
		return 0;
	}
	word_ptr = word;
	while (isalpha(*word_ptr) || isdigit(*word_ptr))
		++word_ptr;
	if (*word_ptr)
	{
		*error_return = ERROR_EXTERN_ILLEGAL_LABEL_NAME;
		return 0;
	}
	line += word_len;
	line = skip_blanks(line);
	if (*line && *line != '\n')
	{
		*error_return = ERROR_EXTERN_CHARACTERS_AFTER_LABEL;
		return 0;
	}	
	if ((label_index = is_word_label(word, label_array)) != -1)
	{
		*error_return = ERROR_EXTERN_LABEL_ALREADY_DECLARED;
		return 0;
	}
	save_label(NULL, NULL, word, label_array, instruction_address, &label_index);
	(*label_array)[label_index].label_type = TYPE_EXTERN;
	return 0;
}

int read_entry_declaration_data(char *line, int instruction_address, Label **label_array, int *error_return)
{
	char word[MAX_WORD_LENGTH], *word_ptr;
	int label_index, word_len;
	*error_return = 0;
	word_len = read_word(line, word);
	if (!word_len)
	{
		*error_return = ERROR_ENTRY_EMPTY_LABEL;
		return 0;
	}
	if (!isalpha(word[0]))
	{
		*error_return = ERROR_ENTRY_ILLEGAL_LABEL_NOT_BEGIN_WITH_ALPHA;
		return 0;
	}
	word_ptr = word;
	while (isalpha(*word_ptr) || isdigit(*word_ptr))
		++word_ptr;
	if (*word_ptr)
	{
		*error_return = ERROR_ENTRY_ILLEGAL_LABEL_NAME;
		return 0;
	}
	line += word_len;
	line = skip_blanks(line);
	if (*line && *line != '\n')
	{
		*error_return = ERROR_ENTRY_CHARACTERS_AFTER_LABEL;
		return 0;
	}	
	if ((label_index = is_word_label(word, label_array)) == -1)
	{
		*error_return = ERROR_ENTRY_LABEL_NOT_DECLARED;
		return 0;
	}
	(*label_array)[label_index].label_type = TYPE_ENTRY;
	return 0;
}

int read_data_declaration_data(char *line, int instruction_address, Word **word_array, int *error_return)
{
	/* assume first character is not blank */
	int integer_count, num, sign;
	integer_count = 0;
	if (*line == '\n')
	{
		*error_return = ERROR_EMPTY_INTEGER_LIST;
		return integer_count;
	}
	while (*line)
	{
		num = 0;
		sign = *line == '-' ? -1 : 1;
		if (*line == '-' || *line == '+')
			++line;
		if (!isdigit(*line))
		{
			if (*line == ',')
				*error_return = ERROR_MULTIPLE_COMMAS_IN_INTEGER_LIST;
			else if (*line == '\n')
				*error_return = ERROR_INTERGER_LIST_ENDING_WITH_COMMA;
			else
				*error_return = ERROR_NOT_AN_INTEGER;
			break;
		}
		while (isdigit(*line))
		{
			num = num * 10 + (*line -'0');
			++line;
		}
		*error_return = save_word(instruction_address++, num * sign, 0, word_array);
		if (*error_return)
			break;
		++integer_count;
		/* done reading and saving num into a word, validate syntax before next read */
		line = skip_blanks(line);
		if (*line == '\n')
			break;
		if (*line == ',')
		{
			++line;
			line = skip_blanks(line);
			continue;
		}
		/* '\n' and ',' are the only accepted digits after reading an integer AND skipping blanks
		 * if we are here, there is an issue with the input */
		if (isdigit(*line))
		{
			*error_return = ERROR_BLANKS_BETWEEN_INTEGERS;
			break;
		}
		*error_return = ERROR_NOT_AN_INTEGER;
		break;
	}
	return integer_count;
}

int read_string_declaration_data(char *line, int instruction_address, Word **word_array, int *error_return)
{
	/* assumes line points to non blank or ',' */
	int char_count = 1; /* 1 due to '\0' string terminator */
	if (*line != '"')
	{
		if (*line == '\n')
			*error_return = ERROR_STRING_NO_INPUT;
		else
			*error_return = ERROR_STRING_DECLARATION_NOT_OPENING_WITH_QUOTES;
		return 0;
	}
	++line;
	while (*line && *line != '\n')
	{
		if (*line == '"') /* quotes without escape prior to them signify the end of the string */
			break;
		if (*line == '\\')
			++line;
		*error_return = save_word(instruction_address++, *line, 0, word_array);
		if (*error_return)
			return instruction_address;
		++line, ++char_count;
	}
	if (*line != '"') /* broke out of while before quotes closing */
	{
		*error_return = ERROR_MISSING_QUOTES_END;
		return char_count;
	}
	++line;
	line = skip_blanks(line);
	if (*line != '\n')
	{
		*error_return = ERROR_STRING_DECLARATION_CHARACTERS_AFTER_END_OF_QUOTES;
		return char_count;
	}
	*error_return = save_word(instruction_address++, '\0', 0, word_array);
	return char_count;
}

int after_macro_verify_command_till_arguments(char **line, char *command_code)
{
	/* assume the first character is non blank */
	char word[MAX_WORD_LENGTH];
	int word_len = read_word_delimited(*line, word, " \t,");
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

int after_macro_save_command_arguments(char *line, int instruction_address, char opcode, Word **word_array, int *error_return)
{
	/* assume line points to a non ',' character */
	/* assume opcode is between in [0,15] */
	char word[MAX_WORD_LENGTH];
	int word_len;
	*error_return = 0;
	if (opcode <= 4)
	{
		/* expect 2 arguments for commands with such opcode */
		if (*line == '\n') {
			*error_return = ERROR_MISSING_ARGUMENTS;
			return 2;
		}
		word_len = read_word_delimited(line, word, " \t,"); /* must be greater than one length due to assumption */
		save_word(instruction_address++, 0, 0, word_array);
		line += word_len;
		line = skip_blanks(line);
		if (*line != ',')
		{
			*error_return = ERROR_BLANK_BETWEEN_ARGUMENTS;
			return 2;
		}
		++line;
		line = skip_blanks(line);
		word_len = read_word_delimited(line, word, " \t,");
		if (!word_len)
		{
			*error_return = ERROR_CONSEQUTIVE_COMMAS;
			return 2;
		}
		save_word(instruction_address, 0, 0, word_array);
		line += word_len;
		line = skip_blanks(line);
		if (*line == ',' && *line != '\n')
		{
			if (read_word_delimited(line, word, " \t,"))
			{
				return ERROR_TOO_MANY_ARGUMENTS;
				return 2;
			}
			*error_return = ERROR_COMMA_AFTER_ARGUMENTS;
			return 2;
		}
		return 2;
	}
	else if (opcode <= 13)
	{
		/* expect 1 arguments for commands with such opcode */
		if (*line == '\n')
			return ERROR_MISSING_ARGUMENTS;
		word_len = read_word_delimited(line, word, " \t,"); /* must be greater than one length due to assumption */
		save_word(instruction_address++, 0, 0, word_array);
		line += word_len;
		line = skip_blanks(line);
		if (*line == ',')
		{
			*error_return = ERROR_COMMA_AFTER_ARGUMENTS;
			return 1;
		}
		if (read_word_delimited(line, word, " \t,"))
			*error_return = ERROR_BLANK_BETWEEN_ARGUMENTS;
		return 1;
	}
	/* expect 0 arguments due to assumption there is a word to be read */
	if (*line != '\n')
		*error_return = ERROR_TOO_MANY_ARGUMENTS;
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
	while ((*label_array)[i].address)
		if (!strcmp((*label_array)[i++].name, word))
				return ERROR_LABEL_DUPLICATE;
	return 0;
}

int is_word_label(char *word, Label **label_array)
{
	int i;
	Label *labels_ptr = *label_array;
	i = 0;
	while (labels_ptr[i].address)
		if (!strcmp(labels_ptr[i++].name, word))
				return i;
	return -1;
}

int save_label(char *line, char *end, char *input_label_name, Label **label_array, int instruction_count, int *stored_label_index)
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
	if (!input_label_name) /* input_label_name allows for the function to receive an input name instead of reading one */
	{
		*end = '\0';
		read_word(line, word);
		*end = ':';
	}
	else
		strcpy(word, input_label_name);
	tmp = *label_array;
	strcpy(tmp[next_label_array_index].name, word);
	tmp[next_label_array_index].address = instruction_count;
	tmp[next_label_array_index].label_type = TYPE_CODE; /* defaults to code on set */
	*stored_label_index = next_label_array_index;
	/* handle array index increment */
	tmp = increment_label_array_index(*label_array, ++next_label_array_index, &error_return);
	if (tmp) /* null if there was an issue with allocation, in which case error_return is set to the relevant error */
		*label_array = tmp;
	return error_return;
}

int save_word(int instruction_count, int value, char is_command, Word **word_array)
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
	word_array_dereference[next_word_array_index].code_address = instruction_count;
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
	/* 0 out of newly allocated memory for address field (type 0 means there is no content)  */
	for (; last_initialized < alloc_size; ++last_initialized)
		temp_label_array[last_initialized].address = 0;
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
	save_label(NULL, NULL, NULL, NULL, 0, NULL);
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
	/* 0 out of newly allocated memory for code_address field (type 0 means there is no content)  */
	for (; last_initialized < alloc_size; ++last_initialized)
		temp_word_array[last_initialized].code_address = 0;
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
	while (label_array[i].address)
	{
		printf(	"name: %s addr: %d type: %d\n", 
			label_array[i].name, 
			label_array[i].address,
			label_array[i].label_type
		);
		++i;
	}
}

void increment_data_type_labels_address(Label *label_array, int address_increment)
{
	int i = 0;
	while (label_array[i].address)
	{
		if (label_array[i].label_type == TYPE_DATA)
			label_array[i].address += address_increment + BASE_IC_ADDRESS;
		++i;
	}
}

void print_words(Word *word_array)
{
	int i = 0;
	while (word_array[i].code_address)
	{
		printf(	"%d %d\n", 
			word_array[i].code_address, 
			word_array[i].Data.value
		);
		++i;
	}
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
