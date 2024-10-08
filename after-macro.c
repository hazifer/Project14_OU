#include "after-macro.h"

int begin_assembler(char *input, char *after_label_fname, Word **word_array, Label **label_array)
{
	int return_value;
	char error_flag = 0;
	FILE *fp = fopen(after_label_fname, "r");
	if (!fp)
	{
		printf("Couldn't open file %s, terminating program for input %s\n\n", after_label_fname, input);
		return 1;
	}
	/* begin first scan */
	printf("Beginning code syntax verification for input \"%s\"\n", input);
	return_value = first_after_macro_scan(fp, after_label_fname, word_array, label_array);
	if (!return_value || return_value == ERROR_TERMINATE_ASSEMBLER)
	{
		printf("\tErrors were found for input \"%s\", no output files are created\n\n", input);
		return 1;
	}
	printf("\tSuccess\n");
	/* update addresses for data_type labels */
	increment_data_type_labels_address(*label_array, return_value);
	/* begin second scan */
	fseek(fp, 0, SEEK_SET);
	printf("Beginning labels' addressing verification for input \"%s\"\n", input);
	return_value = second_after_macro_scan(fp, after_label_fname, word_array, label_array);
	fclose(fp);
	if (return_value)
	{
		printf("\tErrors were found, no output files are created\n\n");
		return 1;
	}
	printf("\tSuccess\n");
	printf("Creating output files for input \"%s\"\n", input);
	return_value = create_object_file(input, *word_array);
	if (return_value < 0)
	{
		if (return_value == ERROR_OBJECT_FILE_ACCESS)
			print_error(input, NULL, return_value, 0);
		else
			printf("\tError: file size unknown for object file created \"%s.obj\"\n", input);
		error_flag = 1;
	}
	else
	{
		if (return_value > 0)
		{
			error_flag = 1;
			printf("\tSuccessfully created object file \"%s.obj\"\n", input);
		}
		else
			printf("\tEmpty code file. No object file created for input \"%s\"\n", input);
	}

	return_value = create_extern_file(input, *label_array);
	if (return_value < 0)
	{
		if (return_value == ERROR_EXTERN_FILE_ACCESS)
		{
			error_flag = 1;
			print_error(input, NULL, return_value, 0);
		}
		else
			printf("\tError: file size unknown for extern file created \"%s.ext\"\n", input);
	}
	else
	{
		if (return_value > 0)
			printf("\tSuccessfully created extern file \"%s.ext\"\n", input);
		else
			printf("\tNo extern labels declared. No extern file created for input \"%s\"\n", input);
	}

	return_value = create_entry_file(input, *label_array);
	if (return_value < 0)
	{
		if (return_value == ERROR_ENTRY_FILE_ACCESS)
		{
			error_flag = 1;
			print_error(input, NULL, return_value, 0);
		}
		else
			printf("\tError: file size unknown for entry file created \"%s.ent\"\n", input);
	}
	else
	{
		if (return_value > 0)
			printf("\tSuccessfully created entry file \"%s.ent\"\n", input);
		else
			printf("\tNo entry labels declared. No entry file created for input \"%s\"\n", input);
	}
	return error_flag;
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
			{
				print_error(fname, line, error_return, line_number);
				error_return = 0; /* to not cause a program stop */
				--instruction_count;
			}
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

int second_after_macro_scan(FILE *fp, char *fname, Word **word_array, Label **label_array)
{
	char line[MAX_CHARS_IN_LINE], *line_ptr, word[MAX_WORD_LENGTH], error_flag, command_type;
	int word_array_index, error_return, word_len, line_number, label_index;
	Command current_command;
	word_array_index = line_number = error_flag = 0;
	while (fgets(line, MAX_CHARS_IN_LINE, fp))
	{
		/* second scan begins ONLY if the syntax for the commands is correct */
		error_return = 0;
		++line_number;
		line_ptr = line;
		if (strchr(line_ptr, ':'))
			line_ptr = strchr(line_ptr, ':') + 1; /* skip label */
		/* why not use word_array for opcode, instead of get_command_op_code, also using is_command */
		line_ptr = skip_blanks(line_ptr);
		word_len = read_word_delimited(line_ptr, word, " \t\n,");
		command_type = get_command_op_code(word);
		line_ptr += word_len;
		line_ptr = skip_blanks(line_ptr);
		/*printf("line = %s", line);
		printf(" command_type %d", command_type);
		printf(" command word = %s", word);
		printf(" before index = %d", word_array_index);*/
		if (command_type == -1)
		{
			/* declaration types */
			command_type = get_declaration_type(word);
/*			printf (" command_type = %d\n", command_type);*/
			if (command_type == TYPE_STRING || command_type == TYPE_DATA)
				word_array_index = skip_data_words(line_ptr, *word_array, word_array_index, command_type);
			else if (command_type == TYPE_ENTRY)
			{
				read_word(line_ptr, word);
				error_return = second_scan_read_entry_declaration(word, *label_array);
			}
			/* no need to verify anything with .extern commands, it was done in the first scan */
			if (error_return)
			{
				error_flag = 1;
				print_error(fname, line, error_return, line_number);
			}
			continue;
		}
		current_command = (*word_array)[word_array_index++].Data.command;
/*		printf(" src_type %d dst_type %d\n", current_command.src_addressing_type, current_command.dst_addressing_type);*/
		if (current_command.src_addressing_type == ADDRESSING_TYPE_DIRECT || current_command.dst_addressing_type == ADDRESSING_TYPE_DIRECT)
		{
			if (current_command.src_addressing_type)
			{
				word_len = read_word_delimited(line_ptr, word, " \t\n,");
				line_ptr += word_len;
				line_ptr = skip_blanks(line_ptr);
				++line_ptr; /* skip over ',' */
				line_ptr = skip_blanks(line_ptr);
				if (current_command.src_addressing_type == ADDRESSING_TYPE_DIRECT)
				{	
					/* this is a case of 2 arguments with the first being of ADDRESSING_TYPE_DIRECT*/
					label_index = find_label_by_name(word, *label_array);
					if (label_index == -1)
					{
						error_flag = 1;
						print_error(fname, line, ERROR_EXTERN_LABEL_NOT_DECLARED, line_number);
						continue;
					}
					if ((*label_array)[label_index].label_type == TYPE_EXTERN_DECLARATION)
					{
						error_return = save_label(word, label_array, word_array_index + BASE_IC_ADDRESS, &label_index);
						(*label_array)[label_index].label_type = TYPE_EXTERN;
						(*word_array)[word_array_index].Data.data_word.are_type = ADDRESSING_TYPE_E_SET;
						(*word_array)[word_array_index].type = TYPE_ADDRESS;
					}
					else
					{
						(*word_array)[word_array_index].Data.data_word.value = (*label_array)[label_index].address;
						(*word_array)[word_array_index].Data.data_word.are_type = ADDRESSING_TYPE_R_SET;
						(*word_array)[word_array_index].type = TYPE_ADDRESS;
					}
				}
				if (error_return)
				{
					error_flag = 1;
					print_error(fname, line, error_return, line_number);
				}
				++word_array_index;
			}
			if (current_command.dst_addressing_type == ADDRESSING_TYPE_DIRECT)
			{	
				/* this is a case of 2 arguments */
				word_len = read_word_delimited(line_ptr, word, " \t\n");
				label_index = find_label_by_name(word, *label_array);
				if (label_index == -1)
				{
					error_flag = 1;
					print_error(fname, line, ERROR_EXTERN_LABEL_NOT_DECLARED, line_number);
					continue;
				}
				if ((*label_array)[label_index].label_type == TYPE_EXTERN_DECLARATION)
				{
					error_return = save_label(word, label_array, word_array_index + BASE_IC_ADDRESS, &label_index);
					(*label_array)[label_index].label_type = TYPE_EXTERN;
					(*word_array)[word_array_index].Data.data_word.are_type = ADDRESSING_TYPE_E_SET;
					(*word_array)[word_array_index].type = TYPE_ADDRESS;
				}
				else
				{
					(*word_array)[word_array_index].Data.data_word.value = (*label_array)[label_index].address;
					(*word_array)[word_array_index].Data.data_word.are_type = ADDRESSING_TYPE_R_SET;
					(*word_array)[word_array_index].type = TYPE_ADDRESS;
				}
			}
			if (error_return)
			{
				error_flag = 1;
				print_error(fname, line, error_return, line_number);
			}
			++word_array_index;
		}
		else /* skip number of words by command type */
			word_array_index = skip_command_argument_words(word_array_index, current_command);
		/*printf("after index = %d\n", word_array_index);*/
		if (error_return)
		{
			print_error(fname, line, error_return, line_number);
			error_flag = 1;
		}
	}
	return error_flag;
}

int second_scan_read_two_arguments(char *line, Word *word_array, int word_array_index, Label *label_array, int command_type)
{
	/* word_array points to a command word which needs a source and a destination argument 
	char word[MAX_WORD_LENGTH], src_addressing_type, dst_addressing_type;
	int word_len, error_return = 0;
	word_len = read_word_delimited(line, word, ","); *//* we dont expect blanks */
/*	src_addressing_type = get_addressing_type(word, label_array, &error_return);*/ /* src_addressing_type now holds an error or a proper addressing type */
/*	if (error_return)
		return error_return;
       	line += word_len + 1;*/ /* point past ',' */
/*	word_len = read_word_delimited(line, word, ", \t");*/
/*	dst_addressing_type = get_addressing_type(word, label_array, &error_return); *//* dst_addressing_type now holds an error or a proper addressing type */
/*	if (error_return)
		return error_return;*/
	/*
	error_return = verify_addressing_types_for_command(command_type, src_addressing_type, dst_addressing_type);*/
/*	if (error_return)
		return error_return;*/
	return 0;
}

int after_macro_handle_label(char *line, char *colon_ptr, int instruction_address, Label **label_array, int *stored_label_index)
{
	/* assumes colon_ptr points to ':' in label */
	int return_value;
	char word[MAX_WORD_LENGTH], label_name[MAX_WORD_LENGTH];
	return_value = verify_label_syntax(line, colon_ptr);
	if (return_value)
		return return_value;
	*colon_ptr = '\0';
	read_word(line, label_name);
	*colon_ptr = ':';
	return_value = find_label_by_name(label_name, *label_array);
	if (return_value != -1)
		return ERROR_LABEL_DUPLICATE;
	if (!verify_not_reserved(label_name) || get_command_op_code(label_name) != -1)
		return ERROR_LABEL_RESERVED_WORD;
	/* ignore labels in .entry or .extern lines */
	read_word(colon_ptr + 1, word);
	if (!strcmp(word, ".extern") || !strcmp(word, ".entry"))
		return WARN_LABEL_IN_ENTRY_EXTERN_LINE;
	/*printf("label_name = %s index = %d\n", label_name, *stored_label_index);*/
	return_value = save_label(label_name, label_array, instruction_address, stored_label_index);
	return return_value;
}

int after_macro_save_words(char *line, int instruction_address, int *error_return, Word **word_array, Label **label_array, char *command_type)
{
	/* assume non blank at *line */
	char command_code, *command_end_ptr;
	int return_value, word_index;
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
		return_value = save_word(instruction_address++, (int)command_code, TYPE_COMMAND, word_array, &word_index);
		if (return_value) {
			*error_return = return_value;
			return instruction_address;
		}
		(*word_array)[word_index].Data.command.are_type = ADDRESSING_TYPE_A_SET;
		/* save argument words */
		instruction_address += after_macro_save_command_arguments(command_end_ptr, instruction_address, command_code, word_array, error_return);
	}
	else /* declaration type command (".data" ".string" etc) */
	{
		if (command_code == TYPE_DATA || command_code == TYPE_STRING)
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
		return first_read_entry_declaration_data(line, instruction_address, label_array, error_return);
	if (opcode == TYPE_EXTERN)
		return first_read_extern_declaration_data(line, instruction_address, label_array, error_return);
	return 0;
}

int first_read_extern_declaration_data(char *line, int instruction_address, Label **label_array, int *error_return)
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
	save_label(word, label_array, instruction_address, &label_index);
	(*label_array)[label_index].label_type = TYPE_EXTERN_DECLARATION;
	return 0;
}

int first_read_entry_declaration_data(char *line, int instruction_address, Label **label_array, int *error_return)
{
	char word[MAX_WORD_LENGTH], *word_ptr;
	int word_len;
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
/*	
	label_index = is_word_label(word, label_array);
	if (label_index == -1)
	{
		*error_return = ERROR_ENTRY_LABEL_NOT_DECLARED;
		return 0;
	}
	printf("entry label_index = %d\n", label_index);
	(*label_array)[label_index].label_type = TYPE_ENTRY; */
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
		*error_return = save_word(instruction_address++, num * sign, sign == 1 ? TYPE_DATA : TYPE_DATA_NEGATIVE, word_array, NULL);
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
		*error_return = save_word(instruction_address++, *line, TYPE_DATA, word_array, NULL);
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
	*error_return = save_word(instruction_address++, '\0', TYPE_DATA, word_array, NULL);
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
	*command_code = get_command_op_code(word);
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
	char src_argument[MAX_WORD_LENGTH], dst_argument[MAX_WORD_LENGTH];
	char src_argument_addressing_type, dst_argument_addressing_type, words_added = 0;
	int word_len, word_array_index, command_word_array_index, src_value, dst_value;
	Word *word_array_dereference = *word_array;
	*error_return = 0;
	if (opcode <= TWO_ARGUMENT_OPCODE_UPPER_RANGE)
	{
		/* read registers / *registers and set values accordingly */
		/* expect 2 arguments for commands with such opcode */
		if (*line == '\n') {
			*error_return = ERROR_MISSING_ARGUMENTS;
			return words_added;
		}
		word_len = read_word_delimited(line, src_argument, " \t\n,"); /* first word read must be greater than one length due to assumption */
		line += word_len;
		line = skip_blanks(line);
		if (*line != ',')
		{
			if (*line == '\n')
				*error_return = ERROR_MISSING_ARGUMENTS;
			else
				*error_return = ERROR_BLANK_BETWEEN_ARGUMENTS; /* blanks IN argument? */
			return words_added;
		}
		++line;
		line = skip_blanks(line);
		word_len = read_word_delimited(line, dst_argument, " \t\n,"); /* second word read, after skipping blanks we expect a non ',' */
		if (!word_len)
		{
			*error_return = ERROR_CONSEQUTIVE_COMMAS;
			return words_added;
		}
		line += word_len;
		*error_return = handle_syntax_post_relevant_arguments(line);
		/* keep the data storing of words till after the simpler syntax checks */
		src_argument_addressing_type = handle_addressing(src_argument, &src_value, error_return);
		if (*error_return) /* *error_return holds errors for the source word's syntax such as punctuations' existence etc */
			return words_added;
		dst_argument_addressing_type = handle_addressing(dst_argument, &dst_value, error_return);
		if (*error_return) /* *error_return holds errors for the destination word's syntax such as punctuations' existence etc */
			return words_added;
		if (opcode == LEA_OPCODE && src_argument_addressing_type != ADDRESSING_TYPE_DIRECT)
		{
			*error_return = ERROR_SOURCE_ADDRESSING_TYPE_INVALID;
			return words_added;
		}
		if (opcode != CMP_OPCODE && dst_argument_addressing_type == ADDRESSING_TYPE_IMMEDIATE)
		{
			*error_return = ERROR_SOURCE_ADDRESSING_TYPE_INVALID;
			return words_added;
		}
		/* addressing type validation passed, save into memory */
		command_word_array_index = word_array_index - 1; /* command word was saved right before the first argument */
		save_word(instruction_address + words_added++, 0, 0, word_array, &word_array_index);
		if (src_argument_addressing_type == ADDRESSING_TYPE_IMMEDIATE)
		{
			word_array_dereference[command_word_array_index].Data.command.src_addressing_type = ADDRESSING_TYPE_IMMEDIATE;
			word_array_dereference[word_array_index].Data.data_word.value = src_value;
			word_array_dereference[word_array_index].Data.data_word.are_type = ADDRESSING_TYPE_A_SET;
			word_array_dereference[word_array_index].type = TYPE_IMMEDIATE;
		}
		else if (src_argument_addressing_type == ADDRESSING_TYPE_DIRECT) /* no value for DIRECT ADDRESSING YET, and we don't know if label is internal or external */
			word_array_dereference[command_word_array_index].Data.command.src_addressing_type = ADDRESSING_TYPE_DIRECT;
		else if (src_argument_addressing_type == ADDRESSING_TYPE_INDIRECT_REGISTER)
		{
			word_array_dereference[command_word_array_index].Data.command.src_addressing_type = ADDRESSING_TYPE_INDIRECT_REGISTER;
			word_array_dereference[word_array_index].Data.complex_data_word.src_register = src_value;
			word_array_dereference[word_array_index].Data.complex_data_word.are_type = ADDRESSING_TYPE_A_SET;
			word_array_dereference[word_array_index].type = TYPE_REGISTER;
		}
		else /* src_argument_addressing_type == DIRECT_REGISTER_ADDRESSING */
		{
			word_array_dereference[command_word_array_index].Data.command.src_addressing_type = ADDRESSING_TYPE_DIRECT_REGISTER;
			word_array_dereference[word_array_index].Data.complex_data_word.src_register = src_value;
			word_array_dereference[word_array_index].Data.complex_data_word.are_type = ADDRESSING_TYPE_A_SET;	
			word_array_dereference[word_array_index].type = TYPE_REGISTER;
		}
		/* dst addressing */
		if (dst_argument_addressing_type == ADDRESSING_TYPE_IMMEDIATE)
		{
			save_word(instruction_address + words_added++, 0, 0, word_array, &word_array_index);
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_IMMEDIATE;
			word_array_dereference[word_array_index].Data.data_word.value = dst_value;
			word_array_dereference[word_array_index].Data.data_word.are_type = ADDRESSING_TYPE_A_SET;
			word_array_dereference[word_array_index].type = TYPE_IMMEDIATE;
		}
		else if (dst_argument_addressing_type == ADDRESSING_TYPE_DIRECT) /* no value for DIRECT ADDRESSING YET, and we don't know if label is internal or external */
		{
			save_word(instruction_address + words_added++, 0, 0, word_array, &word_array_index);
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_DIRECT;
		}
		else if (dst_argument_addressing_type == ADDRESSING_TYPE_INDIRECT_REGISTER)
		{
			if (src_argument_addressing_type != ADDRESSING_TYPE_INDIRECT_REGISTER && src_argument_addressing_type != ADDRESSING_TYPE_DIRECT_REGISTER)
				save_word(instruction_address + words_added++, 0, 0, word_array, &word_array_index);
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_INDIRECT_REGISTER;
			word_array_dereference[word_array_index].Data.complex_data_word.dst_register = dst_value;
			word_array_dereference[word_array_index].Data.complex_data_word.are_type = ADDRESSING_TYPE_A_SET;
			word_array_dereference[word_array_index].type = TYPE_REGISTER;
		}
		else /* dst_argument_addressing_type == DIRECT_REGISTER_ADDRESSING */
		{
			if (src_argument_addressing_type != ADDRESSING_TYPE_INDIRECT_REGISTER && src_argument_addressing_type != ADDRESSING_TYPE_DIRECT_REGISTER)
				save_word(instruction_address + words_added++, 0, 0, word_array, &word_array_index);
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_DIRECT_REGISTER;
			word_array_dereference[word_array_index].Data.complex_data_word.dst_register = dst_value;
			word_array_dereference[word_array_index].Data.complex_data_word.are_type = ADDRESSING_TYPE_A_SET;
			word_array_dereference[word_array_index].type = TYPE_REGISTER;
		}
		return words_added;
	}
	else if (opcode <= ONE_ARGUMENT_OPCODE_UPPER_RANGE)
	{
		/* expect 1 arguments for commands with such opcode */
		if (*line == '\n')
		{
			*error_return = ERROR_MISSING_ARGUMENTS;
			return words_added;
		}
		word_len = read_word_delimited(line, dst_argument, " \t\n,"); /* must be greater than one length due to assumption */
		line += word_len;
		line = skip_blanks(line);
		if (*line != '\n')
		{
			if (*line == ',')
				*error_return = ERROR_COMMA_AFTER_ARGUMENTS;
			else
				*error_return = ERROR_BLANK_BETWEEN_ARGUMENTS;
			return words_added;
		}
		dst_argument_addressing_type = handle_addressing(dst_argument, &dst_value, error_return);
		if (*error_return) /* *error_return holds errors for the source word's syntax such as punctuations' existence etc */
			return words_added;
		if (opcode != PRN_OPCODE && dst_argument_addressing_type == ADDRESSING_TYPE_IMMEDIATE)
		{
			*error_return = ERROR_DESTINATION_ADDRESSING_TYPE_INVALID;
			return words_added;
		}
		if (dst_argument_addressing_type == ADDRESSING_TYPE_DIRECT_REGISTER && (opcode == JMP_OPCODE || opcode == BNE_OPCODE || opcode == JSR_OPCODE))
		{
			*error_return = ERROR_DESTINATION_ADDRESSING_TYPE_INVALID;
			return words_added;
		}
		command_word_array_index = word_array_index - 1; /* command word was saved right before the first argument */
		save_word(instruction_address + words_added++, 0, 0, word_array, &word_array_index);
		if (dst_argument_addressing_type == ADDRESSING_TYPE_IMMEDIATE)
		{
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_IMMEDIATE;
			word_array_dereference[word_array_index].Data.data_word.value = dst_value;
			word_array_dereference[word_array_index].Data.data_word.are_type = ADDRESSING_TYPE_A_SET;
			word_array_dereference[word_array_index].type = TYPE_IMMEDIATE;
		}
		else if (dst_argument_addressing_type == ADDRESSING_TYPE_DIRECT) /* no value for DIRECT ADDRESSING YET, and we don't know if label is internal or external */
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_DIRECT;
		else if (dst_argument_addressing_type == ADDRESSING_TYPE_INDIRECT_REGISTER)
		{
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_INDIRECT_REGISTER;
			word_array_dereference[word_array_index].Data.complex_data_word.dst_register = dst_value;
			word_array_dereference[word_array_index].Data.complex_data_word.are_type = ADDRESSING_TYPE_A_SET;	
			word_array_dereference[word_array_index].type = TYPE_REGISTER;
		}
		else /* dst_argument_addressing_type == DIRECT_REGISTER_ADDRESSING */
		{
			word_array_dereference[command_word_array_index].Data.command.dst_addressing_type = ADDRESSING_TYPE_DIRECT_REGISTER;
			word_array_dereference[word_array_index].Data.complex_data_word.dst_register = dst_value;
			word_array_dereference[word_array_index].Data.complex_data_word.are_type = ADDRESSING_TYPE_A_SET;	
			word_array_dereference[word_array_index].type = TYPE_REGISTER;
		}
		return words_added;
	}
	/* expect 0 arguments due to assumption there is a word to be read */
	if (*line != '\n')
		*error_return = ERROR_TOO_MANY_ARGUMENTS;
	return words_added;
}

int verify_label_syntax(char *line, char *end)
{
	char word[MAX_WORD_LENGTH];
	int len, i;
	if (strchr(end + 1, ':'))
		return ERROR_LABEL_MULTIPLE_COLON;
	*end = '\0';
	line = skip_blanks(line);
	if (!(len = read_word(line, word)))
	{
		*end = ':';
		return ERROR_LABEL_EMPTY;
	}
	*end = ':';
	if (!isalpha(word[0]))
		return ERROR_LABEL_NOT_BEGIN_WITH_ALPHA;
	line += len;
	if (line < end)
	{
		line = skip_blanks(line);
		if (line != end)
			return ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON;
		return ERROR_LABEL_BLANKS_PRE_COLON;
	}
	for (i = 0; i < len; ++i)
		if (ispunct(word[i]))
			return ERROR_LABEL_PUNCT;
	return 0;
}

int find_label_by_name(char *name, Label *label_array)
{
	int i = 0;
	while (label_array[i].address) {
		if (!strcmp(label_array[i].name, name))
				return i;
		++i;
	}
	return -1;
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

int save_label(char *input_label_name, Label **label_array, int instruction_count, int *stored_label_index)
{
	static int next_label_array_index;
	Label *tmp_for_allocation, *label_array_dereference;
	int error_return = 0;
	if (!label_array)
	{
		next_label_array_index = 0;
		return 0;
	}
	label_array_dereference = *label_array;
	strcpy(label_array_dereference[next_label_array_index].name, input_label_name);
	label_array_dereference[next_label_array_index].address = instruction_count;
	label_array_dereference[next_label_array_index].label_type = TYPE_CODE; /* defaults to code on set */
	if (stored_label_index)
		*stored_label_index = next_label_array_index;
	/* handle array index increment */
	tmp_for_allocation = increment_label_array_index(label_array_dereference, ++next_label_array_index, &error_return);
	if (tmp_for_allocation) /* null if there was an issue with allocation, in which case error_return is set to the relevant error */
		*label_array = tmp_for_allocation;
	return error_return;
}

int save_word(int instruction_count, int value, char type, Word **word_array, int *word_array_index)
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
	if (word_array_index)
		*word_array_index = next_word_array_index;
	word_array_dereference[next_word_array_index].code_address = instruction_count;
	word_array_dereference[next_word_array_index].type = type;
	word_array_dereference[next_word_array_index].Data.data_word.value = value;
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
		type = get_command_op_code(word);
	}
	else
	{
		++line;
		read_word(line, word);
		type = get_command_op_code(word);
		/*type = get_data_type(word);*/
	}
	if (type == -1)
		return ERROR_COMMAND_UNKNOWN;
	return 0;
}

char get_command_op_code(char *op)
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
	save_label(NULL, NULL, 0, NULL);
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
	{
		temp_word_array[last_initialized].code_address = 0;
		temp_word_array[last_initialized].type = 0;
		temp_word_array[last_initialized].Data.data_word.value = 0;
		temp_word_array[last_initialized].Data.data_word.are_type= 0;
	}
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
	save_word(0, 0, 0, NULL, NULL);
	return;
}

void print_labels(Label *label_array)
{
	int i = 0;
	printf("All:\n");
	while (label_array[i].address)
	{
		printf(	"name: %s addr: %d type: %d\n", 
			label_array[i].name, 
			label_array[i].address,
			label_array[i].label_type
		);
		++i;
	}
	i = 0;
	printf("Entry labels:\n");
	while (label_array[i].address)
	{
		if (label_array[i].is_entry) {
			printf(	"name: %s addr: %d type: %d\n", 
				label_array[i].name, 
				label_array[i].address,
				label_array[i].label_type
			);
		}
		++i;
	}
	i = 0;
	printf("Extern labels declaration:\n");
	while (label_array[i].address)
	{
		if (label_array[i].label_type == TYPE_EXTERN_DECLARATION) {
			printf(	"name: %s addr: %d type: %d\n", 
				label_array[i].name, 
				label_array[i].address,
				label_array[i].label_type
			);
		}
		++i;
	}
	i = 0;
	printf("Extern labels:\n");
	while (label_array[i].address)
	{
		if (label_array[i].label_type == TYPE_EXTERN) {
			printf(	"name: %s addr: %d type: %d\n", 
				label_array[i].name, 
				label_array[i].address,
				label_array[i].label_type
			);
		}
		++i;
	}
	i = 0;
	printf("Other labels:\n");
	while (label_array[i].address)
	{
		if (label_array[i].label_type != TYPE_ENTRY && label_array[i].label_type != TYPE_EXTERN_DECLARATION && label_array[i].label_type != TYPE_EXTERN) {
			printf(	"name: %s addr: %d type: %d\n", 
				label_array[i].name, 
				label_array[i].address,
				label_array[i].label_type
			);
		}
		++i;
	}
}

void increment_data_type_labels_address(Label *label_array, int address_increment)
{
	int i = 0;
	while (label_array[i].address)
	{
		if (label_array[i].label_type == TYPE_DATA)
			label_array[i].address += address_increment;
		++i;
	}
}

void print_words(Word *word_array)
{
	int i = 0;
	while (word_array[i].code_address)
	{
		printf("addr %d value %d type %d\n", 
			word_array[i].code_address, 
			word_array[i].Data.data_word.value,
			word_array[i].type
		);
		printf("opcode %d src_addressing_type %d dst_addressing_type %d are_type %d\n",
			word_array[i].Data.command.opcode,
			word_array[i].Data.command.src_addressing_type,
			word_array[i].Data.command.dst_addressing_type,
			word_array[i].Data.command.are_type
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

int skip_command_argument_words(int word_array_index, Command current_command)
{
	if (current_command.opcode > ONE_ARGUMENT_OPCODE_UPPER_RANGE) /* no argument commands */
		return word_array_index;
	if (current_command.opcode > TWO_ARGUMENT_OPCODE_UPPER_RANGE) /* one argument commands */
		return word_array_index + 1;
	/* two argument commands */
	if ((current_command.src_addressing_type == ADDRESSING_TYPE_DIRECT_REGISTER || current_command.src_addressing_type == ADDRESSING_TYPE_INDIRECT_REGISTER)
	    && (current_command.dst_addressing_type == ADDRESSING_TYPE_DIRECT_REGISTER || current_command.dst_addressing_type == ADDRESSING_TYPE_INDIRECT_REGISTER))
		return word_array_index + 1;
	return word_array_index + 2;
}

int skip_data_words(char *line, Word *word_array, int word_array_index, int command_type)
{
	/* assume line is a properly syntax .string or .data declaration data portion */
	if (command_type == TYPE_STRING)
	{
		++line; /* point past '"' */
		/* can't just read word and delimit by '"' because '"' could be escaped by '\"' */
		while (*line)
		{
			if (*line == '"')
			{
				/* end of string input, +1 the count due to '\0' terminator */
				++word_array_index;
				break;
			}
			if (*line == '\\') /* escape character alone is not a character in the data string */
				++line;
			++line, ++word_array_index;
		}
	}
	else
	{
		++word_array_index; /* input is not empty, we handled such case in the first read */	
		while ((line = strchr(line, ',')))
			++line, ++word_array_index;
	}
	return word_array_index;
}

int second_scan_read_entry_declaration(char *label_name, Label *label_array)
{
	int label_index = find_label_by_name(label_name, label_array);
	if (label_index != -1)
	{
		label_array[label_index].is_entry = 1;
		return 0;
	}
	return ERROR_ENTRY_LABEL_NOT_DECLARED;
}

int handle_addressing(char *word, int *word_value, int *error_return)
{
       	char *word_ptr = word;
	if (*word_ptr == '#')
		return read_immediate_addressing(word_ptr + 1, word_value, error_return);
	if (*word_ptr == '*')
		return read_indirect_register_addressing(word_ptr + 1, word_value, error_return);
	if (strlen(word) == 2 && *word_ptr == 'r')
		return read_direct_register_addressing(word_ptr, word_value, error_return);
	return ADDRESSING_TYPE_DIRECT;
}

int read_indirect_register_addressing(char *word, int *word_value, int *error_return)
{
	if ((*word_value = get_register(word)) != -1)
		return ADDRESSING_TYPE_INDIRECT_REGISTER;
	*error_return = ERROR_INDIRECT_REGISTER_INVALID;
	return 0;
}

int read_direct_register_addressing(char *word, int *word_value, int *error_return)
{
	if ((*word_value = get_register(word)) != -1)
		return ADDRESSING_TYPE_DIRECT_REGISTER;
	*error_return = ERROR_DIRECT_REGISTER_INVALID;
	return 0;
}

int read_immediate_addressing(char *word, int *word_value, int *error_return)
{
	char sign = 1, *int_str = word;
	int read_int = 0;
	if (*int_str == '-')
	{
		sign = -1;
		++int_str;
	}
	else if (*int_str == '+')
		++int_str;
	if (!isdigit(*int_str))
	{
		*error_return = ERROR_IMMEDIATE_INVALID;
		return 0;
	}
	while (isdigit(*int_str))
	{
		if (read_int * 10 + (*int_str - '0') > IMMEDIATE_OVERFLOW)
		{
			*error_return = ERROR_IMMEDIATE_OVERFLOW;
			return 0;
		}
		read_int = read_int * 10 + (*int_str++ - '0');
	}
	if (*int_str)
	{
		*error_return = ERROR_IMMEDIATE_INVALID;
		return 0;
	}
	*word_value = read_int * sign;
	return ADDRESSING_TYPE_IMMEDIATE;
}

int create_object_file(char *input, Word *word_array)
{
	int i, current_block_bit_shift;
	long fsize, mask, number_in_block, output_value;
	char object_file_name[MAX_FILENAME_LENGTH], value_str[MAX_CHARS_IN_LINE];
	Command current_command;
	Print_format_word print_format_word;
	FILE *object_file_ptr;
	i = 0;
	if (!word_array[i].code_address) /* no code case -> no .obj file */
		return 0;
	strcpy(object_file_name, input);
	strcat(object_file_name, ".obj");
	object_file_ptr = fopen(object_file_name, "w");
	if (!object_file_ptr)
		return ERROR_OBJECT_FILE_ACCESS;
	while (word_array[i].code_address)
	{
		/* address */
		itoa_base10(word_array[i].code_address, value_str);
		fputs(value_str, object_file_ptr);
		fputs("\t", object_file_ptr);
		/* don't forget to pad ABOVE */
/*
		itoa_base10(word_array[i].Data.bits.value, value_str);
		fputs(value_str, object_file_ptr);
		fputs("\t", object_file_ptr);*/

		if (word_array[i].type == TYPE_COMMAND)
		{
			current_command = word_array[i].Data.command;
			output_value = convert_command_format_to_output_format(current_command);
			/* puts 3 bits' values block by block 
			 * from command structure */
			mask = FIFTEEN_BIT_3_BITS_MSB_MASK;
			current_block_bit_shift = 12;
			number_in_block = output_value & mask;
			number_in_block >>= current_block_bit_shift;
			itoa_base10(number_in_block, value_str);
			fputs(value_str, object_file_ptr);

			mask = TWELVE_BIT_3_BITS_MSB_MASK;
			current_block_bit_shift = 9;
			number_in_block = output_value & mask;
			number_in_block >>= current_block_bit_shift;
			itoa_base10(number_in_block, value_str);
			fputs(value_str, object_file_ptr);

			mask = NINE_BIT_3_BITS_MSB_MASK;			
			current_block_bit_shift = 6;
			number_in_block = output_value & mask;
			number_in_block >>= current_block_bit_shift;
			itoa_base10(number_in_block, value_str);
			fputs(value_str, object_file_ptr);

			mask = SIX_BIT_3_BITS_MSB_MASK;			
			current_block_bit_shift = 3;
			number_in_block = output_value & mask;
			number_in_block >>= current_block_bit_shift;
			itoa_base10(number_in_block, value_str);
			fputs(value_str, object_file_ptr);

			mask = THREE_BIT_3_BITS_MSB_MASK;
			current_block_bit_shift = 0;
			number_in_block = output_value & mask;
			number_in_block >>= current_block_bit_shift;
			itoa_base10(number_in_block, value_str);
			fputs(value_str, object_file_ptr);
			fputs("\t", object_file_ptr);
		}
		else if (word_array[i].type == TYPE_REGISTER)
		{
			print_format_word = word_array[i].Data.print_format_word;
			itoa_base10(print_format_word.field1, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field2, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field3, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field4, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field5, value_str);
			fputs(value_str, object_file_ptr);
			fputs("\t", object_file_ptr);
		}
		else if (word_array[i].type == TYPE_DATA)
		{
			/* field types */
			print_format_word = word_array[i].Data.print_format_word;
			itoa_base10(print_format_word.field5, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field4, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field3, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field2, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field1, value_str);
			fputs(value_str, object_file_ptr);
			fputs("\t", object_file_ptr);
		}
		else if (word_array[i].type == TYPE_DATA_NEGATIVE)
		{
			/* field types */
/*			word_array[i].Data.data_word.value = ~word_array[i].Data.data_word.value;*/
			/* field 4's msb acts as the sign extension to a 15 bit binary */
			print_format_word = word_array[i].Data.print_format_word;
			mask = 4; /* 0b100 mask */
			if ((print_format_word.field4 & mask) == mask)
				strcpy(value_str, "7");
			else
				strcpy(value_str, "0");
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field4, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field3, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field2, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field1, value_str);
			fputs(value_str, object_file_ptr);
			fputs("\t", object_file_ptr);
		}
		else /* TYPE_ADDRESS or TYPE_IMMEDIATE */
		{
			/* works for immediates, not for addresses */
			/* field types */
			print_format_word = word_array[i].Data.print_format_word;
			itoa_base10(print_format_word.field4, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field3, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field2, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field1, value_str);
			fputs(value_str, object_file_ptr);
			itoa_base10(print_format_word.field5, value_str);
			fputs(value_str, object_file_ptr);
			fputs("\t", object_file_ptr);
		}
/*		 as val and are_type 
		itoa_base10(word_array[i].Data.data_word.value, value_str);
		fputs(value_str, object_file_ptr);
		fputs("\t", object_file_ptr);
		itoa_base10(word_array[i].Data.data_word.are_type, value_str);
		fputs(value_str, object_file_ptr);
		fputs("\t", object_file_ptr); */

		/* as command 
		itoa_base10(word_array[i].Data.command.opcode, value_str);
		fputs(value_str, object_file_ptr);
		fputs("\t", object_file_ptr);
		itoa_base10(word_array[i].Data.command.src_addressing_type, value_str);
		fputs(value_str, object_file_ptr);
		fputs("\t", object_file_ptr);
		itoa_base10(word_array[i].Data.command.dst_addressing_type, value_str);
		fputs(value_str, object_file_ptr);
		fputs("\t", object_file_ptr);
		itoa_base10(word_array[i].Data.command.are_type, value_str);
		fputs(value_str, object_file_ptr); */

		fputs("\n", object_file_ptr);
		++i;
	}
	fsize = ftell(object_file_ptr);
	fclose(object_file_ptr);
	return fsize;
}

int create_extern_file(char *input, Label *label_array)
{
	int i;
	long fsize;
	char extern_file_name[MAX_FILENAME_LENGTH], value_str[MAX_CHARS_IN_LINE];
	FILE *extern_file_ptr;
	if (!is_label_type_exist(label_array, TYPE_EXTERN)) /* no extern labels exist */
		return 0;
	strcpy(extern_file_name, input);
	strcat(extern_file_name, ".ext");
	extern_file_ptr = fopen(extern_file_name, "w");
	if (!extern_file_ptr)
		return ERROR_EXTERN_FILE_ACCESS;
	while (label_array[i].address)
	{
		if (label_array[i].label_type == TYPE_EXTERN)
		{
			fputs(label_array[i].name, extern_file_ptr);
			fputs("\t", extern_file_ptr);
			itoa_base10(label_array[i].address, value_str);
			fputs(value_str, extern_file_ptr);
			fputs("\n", extern_file_ptr);
		}
		++i;
	}
	fsize = ftell(extern_file_ptr);
	fclose(extern_file_ptr);
	return fsize;
}

int create_entry_file(char *input, Label *label_array)
{
	int i;
	long fsize;
	char entry_file_name[MAX_FILENAME_LENGTH], value_str[MAX_CHARS_IN_LINE];
	FILE *entry_file_ptr;
	if (!is_entry_type_label_exist(label_array)) /* no entry labels exist */
		return 0;
	strcpy(entry_file_name, input);
	strcat(entry_file_name, ".ent");
	entry_file_ptr = fopen(entry_file_name, "w");
	if (!entry_file_ptr)
		return ERROR_ENTRY_FILE_ACCESS;
	i = 0;
	while (label_array[i].address)
	{
		if (label_array[i].is_entry)
		{
			fputs(label_array[i].name, entry_file_ptr);
			fputs("\t", entry_file_ptr);
			itoa_base10(label_array[i].address, value_str);
			fputs(value_str, entry_file_ptr);
			fputs("\n", entry_file_ptr);
		}
		++i;
	}
	fsize = ftell(entry_file_ptr);
	fclose(entry_file_ptr);
	return fsize;
}

int is_entry_type_label_exist(Label *label_array)
{
	int i = 0;
	while (label_array[i].address)
		if (label_array[i++].is_entry)
			return 1;
	return 0;
}

int is_label_type_exist(Label *label_array, int label_type)
{
	int i = 0;
	while (label_array[i].address)
		if (label_array[i++].label_type == label_type)
			return 1;
	return 0;
}

long convert_command_format_to_output_format(Command current_command)
{
	long value;
	int opcode, src_addressing_type, dst_addressing_type, are_type;
	opcode = current_command.opcode;
	src_addressing_type = current_command.src_addressing_type;
	dst_addressing_type = current_command.dst_addressing_type;
	are_type = current_command.are_type;
	value = (opcode << OPCODE_BIT_SHIFT) + (src_addressing_type << SRC_ADDRESSING_BIT_SHIFT) +
		(dst_addressing_type << DST_ADDRESSING_BIT_SHIFT) + are_type;
	return value;
}

int handle_syntax_post_relevant_arguments(char *line)
{
	char *end_of_line;
	if (*line == '\n')
		return 0;
	end_of_line = strchr(line, '\n');
	if (!end_of_line)
		return ERROR_LINE_TOO_LONG;
	--end_of_line;
	while (*end_of_line == ' ' || *end_of_line == '\t')
		--end_of_line;
	if (*end_of_line == ',')
		return ERROR_COMMA_AFTER_ARGUMENTS;
	if (*line == ' ' || *line == '\t')
	{
		line = skip_blanks(line);
		if (*line == ',')
			return ERROR_CONSEQUTIVE_COMMAS;
		if (*line != '\n')
			return ERROR_BLANK_BETWEEN_ARGUMENTS;
		return 0;
	}
	if (*line == ',')
	{
		++line;
		line = skip_blanks(line);
		if (*line == ',')
			return ERROR_CONSEQUTIVE_COMMAS;
		return ERROR_TOO_MANY_ARGUMENTS;
	}
	return 0;
}
