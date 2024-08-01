#include "error-handling.h"

void log_error(User_Output **out, char *file_name, char *line, int error_type, int line_number, int *error_return)
{
	static int error_index;
	char ln_str[MAX_LINE_DIGITS_IN_OUTPUT_FILE];
	User_Output *tmp;
	if (!out && !error_return)
	{
		error_index = 0;
		return;
	}
	(*out)[error_index].message_type = error_type;
	strcpy((*out)[error_index].message, ERROR_BASE_STRING);
	strcat((*out)[error_index].message, file_name);
	switch(error_type)
	{
		case ERROR_LABEL_DUPLICATE:
			strcat((*out)[error_index].message, ": an already defined label detected in line\n\t");
			break;
		case ERROR_LABEL_NOT_BEGIN_WITH_ALPHA:
			strcat((*out)[error_index].message, ": label begins with a non alphabetic character in line\n\t");
			break;
		case ERROR_LABEL_MULTIPLE_COLON:
			strcat((*out)[error_index].message, ": multiple colons found (possibly multiple label declarations) in line\n\t");
			break;
		case ERROR_LABEL_EMPTY:
			strcat((*out)[error_index].message, ": empty label in line\n\t");
			break;
		case ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON:
			strcat((*out)[error_index].message, ": incorrect label format, multiple words found pre colon in line\n\t");
			break;
		case ERROR_WORD_FOUND_PRE_MACR_KEYWORD:
			strcat((*out)[error_index].message, ": atleast one word found before \"macr\" statement in line\n\t");
			break;
		case ERROR_SOURCE_FILE_ACCESS:
			strcat((*out)[error_index].message, ": couldn't access file for reading.\n");
			break;
		case ERROR_DESTINATION_FILE_ACCESS:
			strcat((*out)[error_index].message, ": couldn't access file for writing.\n");
			break;
		case ERROR_PROGRAM_MEMORY_ALLOCATION:
			strcat((*out)[error_index].message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_SOURCE_FILE_MEMORY_ALLOCATION:
			strcat((*out)[error_index].message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_DESTINATION_FILE_MEMORY_ALLOCATION:
			strcat((*out)[error_index].message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_MACRO_NAME_EMPTY:
			strcat((*out)[error_index].message, ": empty macro name used in line\n\t");
			break;
		case ERROR_MACRO_NAME_RESERVED_WORD:
			strcat((*out)[error_index].message, ": reserved word used as macro name in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_MACR_KEYWORD:
			strcat((*out)[error_index].message, ": characters detected after macro declaration in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX:
			strcat((*out)[error_index].message, ": ilegal macro name used (must begin with an alphabetic letter & cannot contain punctuations) in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_UNIQUE:
			strcat((*out)[error_index].message, ": macro declared multiple times, second declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD:
			strcat((*out)[error_index].message, ": characters detected before end of macro declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_ENDMACR_KEYWORD:
			strcat((*out)[error_index].message, ": characters detected after end of macro declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_MACRO_NAME:
			strcat((*out)[error_index].message, ": characters detected after macro name in line\n\t");
			break;
		case ERROR_EXCEEDED_OUTPUT_ARRAY_LIMIT:
			strcat((*out)[error_index].message, ": too many errors in code, code ignored after line\n\t");
			break;
		case ERROR_EXCEEDED_MACRO_ARRAY_LIMIT:
			strcat((*out)[error_index].message, ": Exceeded memory allocation limit, too many macros defined\n");
			break;
		default:
			strcat((*out)[error_index].message, ": unknown error in line\n\t");
			break;
	}
	if (line_number)
	{
		(*out)[error_index].line = line_number;
		itoa_base10(line_number, ln_str);
		strcat((*out)[error_index].message, ln_str);
		strcat((*out)[error_index].message, " - ");
		strcat((*out)[error_index].message, line);
	}
	tmp = increment_output_array_index(*out, ++error_index, error_return);
	if (tmp)
		*out = tmp;
}

void print_errors(User_Output *out)
{
	int error_index = 0;
	while (out[error_index].message_type)
		printf("%s", out[error_index++].message);
}

User_Output * increment_output_array_index(User_Output *out, int next_output_index, int *error_return)
{
	User_Output *temp_output_array = NULL;
	if (next_output_index && next_output_index % OUTPUT_ARRAY_INIT_SIZE == 0)
		temp_output_array = allocate_output_array_memory(out, error_return);
	return temp_output_array;
}

User_Output * allocate_output_array_memory(User_Output *out, int *error_return)
{
	static char output_multiplier_factor; /* acts as a multiplier to increase macro_array size with jumps of MACROINIT */
	User_Output *temp_output_array;
	int last_initialized;
	size_t alloc_size;
	if (!out)
	{
		output_multiplier_factor = 1;
		return NULL;
	}
	last_initialized = output_multiplier_factor++ * OUTPUT_ARRAY_INIT_SIZE; 
	alloc_size = last_initialized + OUTPUT_ARRAY_INIT_SIZE;
	if (output_multiplier_factor > OUTPUT_ARRAY_SIZE_MULTIPLIER_LIMIT) /* exceeded unique macro limit for the program (MACROINIT * MACROLIMITFACTOR) */
	{
		*error_return = ERROR_EXCEEDED_OUTPUT_ARRAY_LIMIT;
		return NULL;
	}
	temp_output_array = (User_Output *)realloc(out, alloc_size * sizeof(User_Output)); /* realloc of out */
	if (!temp_output_array)
	{
		*error_return = ERROR_PROGRAM_MEMORY_ALLOCATION;	
		return NULL; /* couldn't allocate enough memory for reallocation/allocation */
	}
	/* 0 out of newly allocated memory for message_type field (type 0 means there is no content)  */
	for (; last_initialized < alloc_size; ++last_initialized)
		temp_output_array[last_initialized].message_type = 0;
	*error_return = 0;
	return temp_output_array;
}

User_Output * init_output_array_memory()
{
	User_Output *temp_output_array; 
	size_t alloc_size = OUTPUT_ARRAY_INIT_SIZE;
	temp_output_array = (User_Output *)calloc(alloc_size, sizeof(User_Output));
	reset_output_array_indices();
	return temp_output_array;
}

void reset_output_array_indices()
{
	allocate_output_array_memory(NULL, NULL);
	log_error(NULL, NULL, NULL, 0, 0, NULL);
	return;
}
