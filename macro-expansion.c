#include "macro-expansion.h"

char * handle_filename_extension(char *filename, char extension[], User_Output **out, int *error_return)
{
	/* only handle cases of no .** extension, and an entirely existing .** extension */
	char *extended_filename;
	FILE *fp;
	if (strstr(filename, extension))
		extended_filename = filename;
	else
	{
		/* in the event that the source filename is not ending with ".as" */
		extended_filename = (char *)malloc((strlen(filename) + 3) * sizeof(char)); /* +3 for ".as" extension */
		/* error handling */
		if(!extended_filename)
		{
			log_error(out, filename, NULL, ERROR_SOURCE_FILE_MEMORY_ALLOCATION, 0, error_return);
			if (*error_return)
			{
				*error_return = ERROR_OUTPUT_MEMORY_ALLOCATION;
				return NULL;
			}
			*error_return = ERROR_SOURCE_FILE_MEMORY_ALLOCATION;
			return NULL;
		}
		extended_filename[0] = '\0';
		strcat(extended_filename, filename);
		strcat(extended_filename, extension);
	}
	/* testing for read access of the input file */
	fp = fopen(extended_filename, "r");
	if (!fp)
	{
		log_error(out, extended_filename, NULL, ERROR_SOURCE_FILE_ACCESS, 0, error_return);
		if (*error_return) {
				*error_return = ERROR_OUTPUT_MEMORY_ALLOCATION;
				return NULL;
		}
		*error_return = ERROR_SOURCE_FILE_ACCESS;
		return NULL;
	}
	fclose(fp);
	*error_return = 0;
	(*out)->message_type = SUCCESS;
	strcpy((*out)->message, SUCCESS_BASE_STRING);
	strcat((*out)->message, extended_filename);
	strcat((*out)->message, "\n");
	return extended_filename;
}

int expand_macros(char *sfname, char *dfname_holder, User_Output **out, int *error_return)
{
	FILE *fpin, *fpout;
	char *dfname, *line, *tmp;
	Macro *macro_array = NULL;
	*error_return = 0;
	/* memory allocation, file access, and error handling */
	dfname = (char *)malloc(strlen(sfname) * sizeof(char));
	if (!dfname)
	{
		log_error(out, sfname, NULL, ERROR_DESTINATION_FILE_MEMORY_ALLOCATION, 0, error_return);
		if (*error_return)
				return ERROR_OUTPUT_MEMORY_ALLOCATION;
	/* free command to be used where we need */
		return 1;
	}
	strcpy(dfname, sfname);
	/* destination file is terminated with ".am" (as in after macro) */
	tmp = strstr(dfname, ".as");
	tmp[2] = 'm';
	fpin = fopen(sfname, "r");
	strcpy(dfname_holder, dfname);
	if (!fpin)
	{
		log_error(out, sfname, NULL, ERROR_SOURCE_FILE_ACCESS, 0, error_return);
		if (*error_return)
				return ERROR_OUTPUT_MEMORY_ALLOCATION;
	/* free command to be used where we need */
		return 1;
	}
	fpout = fopen(dfname, "w");
	if (!fpout)
	{
		log_error(out, dfname, NULL, ERROR_DESTINATION_FILE_ACCESS, 0, error_return);
		if (*error_return)
				return ERROR_OUTPUT_MEMORY_ALLOCATION;
	/* free command to be used where we need */
		return 1;
	}
	line = (char *)malloc(MAX_CHARS_IN_LINE * sizeof(char));
	if (!line)
	{
		log_error(out, sfname, line, ERROR_PROGRAM_MEMORY_ALLOCATION, 0, error_return);
		if (*error_return)
				return ERROR_OUTPUT_MEMORY_ALLOCATION;
	/* free command to be used where we need */
		return 1;
	}
	macro_array = init_macro_array_memory();
	if (*error_return)
	{
		if (*error_return == ERROR_EXCEEDED_MACRO_ARRAY_LIMIT)
		{
			log_error(out, sfname, line, ERROR_EXCEEDED_MACRO_ARRAY_LIMIT, 0, error_return);
			if (*error_return)
					return ERROR_OUTPUT_MEMORY_ALLOCATION;
	/* free command to be used where we need */
			return 1;
		}
		log_error(out, sfname, line, ERROR_PROGRAM_MEMORY_ALLOCATION, 0, error_return);
		if (*error_return)
				return ERROR_OUTPUT_MEMORY_ALLOCATION;
	/* free command to be used where we need */
		return 1;
	}
	expand_macros_memory_allocated(sfname, dfname, fpin, fpout, line, out, &macro_array, error_return);
	/* sfname is freed in main */
	if (*error_return)
		remove(dfname);
	/* free command to be used where we need */
	free(dfname);
	fclose(fpin);
	fclose(fpout);
	free(line);
	free(macro_array);
	return 0;
}

int expand_macros_memory_allocated(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, User_Output **out, Macro **macro_array, int *error_return)
{
	char *li; /* line indexe */
	Macro *temp_macro_array; /* for future macro_array memory reallocations */
	int  state = STATE_COMMAND, return_value, label_flag, line_number, next_macro_index;
	line_number = next_macro_index = 0;
	while (fgets(line, MAX_CHARS_IN_LINE, fpin) && (li = expand_macros_handle_label(line, fpout)))
	{
		++line_number;
		/* line was read, and li is not NULL */
		label_flag = 0;
		if (*li == ';' || *li == '\n')
		{
			/* found a comment or a blank line - output a newline and skip this line */
			fputc('\n', fpout);
			continue;
		}
		/* not a comment nor a blank line */
		if (*li == ':')
		{
			++li;
			label_flag = 1;
		}
		li = skip_blanks(li);
		if (state == STATE_COMMAND)
		{
			return_value = expand_macros_handle_command_state(li, fpout, *macro_array, next_macro_index);
			if (label_flag && is_newline_needed(return_value))
				fputc('\n', fpout);
			if (!return_value || return_value == MACRO_EXPANDED)
			{
				state = STATE_COMMAND;
				continue;
			}
			if (return_value != STATE_COLLECT_MACRO_CONTENT)
			{
				log_error(out, sfname, line, return_value, line_number, error_return);
				if (*error_return)
						return ERROR_OUTPUT_MEMORY_ALLOCATION;
				*error_return = return_value;
				break;
			}
			state = STATE_COLLECT_MACRO_CONTENT;
		}
		else if (state == STATE_COLLECT_MACRO_CONTENT)
		{
			return_value = expand_macros_handle_collect_macro_content_state(li, out, *macro_array, next_macro_index);
			if (return_value == MACRO_LINE_COLLECTED)
			{
				continue;
			}
			if (return_value == ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD || return_value == ERROR_WORD_FOUND_AFTER_ENDMACR_KEYWORD)
			{
				log_error(out, sfname, line, return_value, line_number, error_return);
				if (*error_return)
						return ERROR_OUTPUT_MEMORY_ALLOCATION;
				*error_return = return_value;
				break;
			}
			temp_macro_array = increment_macro_array_index(*macro_array, ++next_macro_index, &return_value);
			if (!temp_macro_array && (return_value == ERROR_PROGRAM_MEMORY_ALLOCATION || return_value == ERROR_EXCEEDED_MACRO_ARRAY_LIMIT))
			{
				log_error(out, sfname, line, return_value, return_value == ERROR_EXCEEDED_MACRO_ARRAY_LIMIT ? 0 : line_number, error_return);
				if (*error_return)
						return ERROR_OUTPUT_MEMORY_ALLOCATION;
				*error_return = return_value;
				break;
			}
			*macro_array = temp_macro_array ? temp_macro_array : *macro_array;
			state = STATE_COMMAND;
		}
	}
	/* function ran successfully, error is saved into error_return */
	return 0;
}

char * expand_macros_handle_label(char *line, FILE *fpout)
{
	char *end = strchr(line, ':'), tmp;
	line = skip_blanks(line);
	if (!end)
		return line;
	tmp = *(end + 1);
	*(end + 1) = '\0';
	fputs(line, fpout);
	*(end + 1) = tmp;
	return end;
}

int expand_macros_handle_command_state(char *line, FILE *fpout, Macro *macro_array, int next_macro_index)
{
	char *li;
	char word[MAX_WORD_LENGTH];
	int macro_index, error_return = 0;
	/* look for "macr" statements FIRST in line (li is currently pointing to a non blank, and not ':') */
	li = read_till_macr_keyword(line, &error_return);
	if (li == NULL && error_return == ERROR_WORD_FOUND_PRE_MACR_KEYWORD)
		return ERROR_WORD_FOUND_PRE_MACR_KEYWORD;
	if (li == NULL)
	{
		/* search for macro words? */
		/* check for already declared macro names against a single word in line, or output line */
		li = skip_blanks(line);
		read_word(li, word);
		if ((macro_index = get_macro_name_index(word, macro_array)) >= 0)
		{
			li += strlen(word);
			if (read_word(li, NULL))
				return ERROR_WORD_FOUND_AFTER_MACRO_NAME;
			expand_macro(fpout, macro_array, macro_index);
			return MACRO_EXPANDED;
		}
		fputs(line, fpout);
		return 0;
	}
	li = li + strlen("macr");
	li = skip_blanks(li);
	
	if (!read_word(li, word))
		return ERROR_MACRO_NAME_EMPTY;
	if (!verify_not_reserved(word))
		return ERROR_MACRO_NAME_RESERVED_WORD;
	if (!verify_macro_name_syntax(word))
		return ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX;
	if (get_macro_name_index(word, macro_array) >= 0)
		return ERROR_MACRO_NAME_NOT_UNIQUE;
	li += strlen(word);
	if (read_word(li, NULL))
		return ERROR_WORD_FOUND_AFTER_MACR_KEYWORD;
	strcpy((macro_array + next_macro_index)->name, word);
	return STATE_COLLECT_MACRO_CONTENT;
}

int expand_macros_handle_collect_macro_content_state(char *line, User_Output **out, Macro *macro_array, int next_macro_index)
{
	char *li;
	int error_return = 0;
       	li = read_till_endmacr_keyword(line, &error_return);
	if (li == NULL && error_return == ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD)
		return ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD;
	if (li == NULL)
	{
		strcat((macro_array + next_macro_index)->content, line);
		return MACRO_LINE_COLLECTED;
	}
	/* found endmacr, verified from start to endmacr index */
	li += strlen("endmacr");
	if (read_word(li, NULL))
		return ERROR_WORD_FOUND_AFTER_ENDMACR_KEYWORD;
	return STATE_COMMAND;
}

char * read_till_macr_keyword(char *line, int *error_return)
{
	/* assumes line is pointing to a non blank, past the label of the line */
	char word[MAX_WORD_LENGTH];
	char *macr_index;
	int word_length;
	*error_return = 0;
	/* check if macro macr statement begins the line */
	macr_index = strstr(line, "macr ");
	if (!macr_index)
		macr_index = strstr(line, "macr\t");
	if (!macr_index)
		macr_index = strstr(line, "macr\n");
	if (line == macr_index)
		return macr_index;
	/* check for any type of macr statement after the startof the line */
	word_length = read_word(line, word); /* the length of the read word allows to look for macr statements AFTER this word */
	line += word_length;
	if (!macr_index)
		macr_index = strstr(line, " macr ");
	if (!macr_index)
		macr_index = strstr(line, "\tmacr ");
	if (!macr_index)
		macr_index = strstr(line, " macr\t");
	if (!macr_index)
		macr_index = strstr(line, "\tmacr\t");
	if (!macr_index)
		macr_index = strstr(line, " macr\n");
	if (!macr_index)
		macr_index = strstr(line, "\tmacr\n");
	if (!macr_index)
		return NULL;
	/* macr_index is not NULL, since line was initially pointing to a non blank, there must be a word prior to the macr statement */
	if (macr_index > line && (*(macr_index - 1) == ' ' || *(macr_index - 1) == '\t'))
		*error_return = ERROR_WORD_FOUND_PRE_MACR_KEYWORD;
	return NULL;
}

char * read_till_endmacr_keyword(char *line, int *error_return)
{
	/* assumes line is pointing to a non blank, past the label of the line */
	char word[MAX_WORD_LENGTH];
	char *endmacr_index;
	int word_length;
	/* check if macro macr statement begins the line */
	endmacr_index = strstr(line, "endmacr ");
	if (!endmacr_index)
		endmacr_index = strstr(line, "endmacr\t");
	if (!endmacr_index)
		endmacr_index = strstr(line, "endmacr\n");
	if (line == endmacr_index)
		return endmacr_index;
	/* check for any type of macr statement after the startof the line */
	word_length = read_word(line, word); /* the length of the read word allows to look for macr statements AFTER this word */
	line += word_length;
	if (!endmacr_index)
		endmacr_index = strstr(line, " endmacr ");
	if (!endmacr_index)
		endmacr_index = strstr(line, "\tendmacr ");
	if (!endmacr_index)
		endmacr_index = strstr(line, " endmacr\t");
	if (!endmacr_index)
		endmacr_index = strstr(line, "\tendmacr\t");
	if (!endmacr_index)
		endmacr_index = strstr(line, " endmacr\n");
	if (!endmacr_index)
		endmacr_index = strstr(line, "\tendmacr\n");
	if (!endmacr_index)
		return NULL;
	/* macr_index is not NULL, since line was initially pointing to a non blank, there must be a word prior to the macr statement */
	*error_return = ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD;
	return NULL;
}

char verify_macro_name_syntax(char *word)
{
	/* assumes word is terminated with '\0' and not '\n' */
	if (isdigit(*word))
		return 0;
	while (*word)
		if (ispunct(*word++))
			return 0;
	return 1;
}

char get_macro_name_index(char *word, Macro *macro_array)
{
	int index = 0;
	while (macro_array->name[0])
	{
		if (!strcmp(word, macro_array->name))
			return index;
		++macro_array, ++index;
	}
	return -1;
}

Macro * allocate_macro_array_memory(Macro *macro_array, int *error_return)
{
	static char macro_multiplier_factor; /* acts as a multiplier to increase macro_array size with jumps of MACROINIT */
	Macro *temp_macro_array; 
	size_t alloc_size;
	if (!macro_array)
	{
		macro_multiplier_factor = 1;
		return NULL;
	}
	alloc_size = ++macro_multiplier_factor * MACRO_ARRAY_INIT_SIZE; /* number of Macro structs to allocate memory for */
	if (macro_multiplier_factor > MACRO_ARRAY_SIZE_MULTIPLIER_LIMIT) /* exceeded unique macro limit for the program (MACROINIT * MACROLIMITFACTOR) */
	{
		*error_return = ERROR_EXCEEDED_MACRO_ARRAY_LIMIT;
		return NULL; 
	}
	temp_macro_array = (Macro *)realloc(macro_array, alloc_size * sizeof(Macro)); /* realloc of macro_array */
	if (!temp_macro_array)
	{
		*error_return = ERROR_PROGRAM_MEMORY_ALLOCATION;
		return NULL; /* couldn't allocate enough memory for reallocation/allocation */
	}
	*error_return = 0;
	return temp_macro_array;
}

Macro * increment_macro_array_index(Macro *macro_array, int next_macro_index, int *error_return)
{
	Macro *temp_macro_array = NULL;
	if (next_macro_index && next_macro_index % MACRO_ARRAY_INIT_SIZE == 0)
		temp_macro_array = allocate_macro_array_memory(macro_array, error_return);
	return temp_macro_array;
}

Macro * init_macro_array_memory()
{
	Macro *temp_macro_array; 
	size_t alloc_size = MACRO_ARRAY_INIT_SIZE;
	reset_macro_array_indices();
	temp_macro_array = (Macro *)calloc(alloc_size, sizeof(Macro));
	return temp_macro_array;
}

void reset_macro_array_indices()
{
	allocate_macro_array_memory(NULL, NULL);
	return;
}

void expand_macro(FILE *fpout, Macro *macro_array, int macro_index)
{
	/* add_tabs_after_newline(macro_array[macro_index].content); */
	fputs(macro_array[macro_index].content, fpout);
}

char is_newline_needed(int state_value)
{
	if (!state_value)
		return 1;
	if (state_value == STATE_COLLECT_MACRO_CONTENT)
		return 1;
	return 0;
}
