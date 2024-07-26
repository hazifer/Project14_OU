#include "error-handling.h"

void log_error(User_Output *out, char *file_name, char *line, int error_type, int line_number)
{
	static int error_index;
	char ln_str[MAX_LINE_DIGITS_IN_OUTPUT_FILE];
	out[error_index].message_type = error_type;
	strcpy(out[error_index].message, ERROR_BASE_STRING);
	strcat(out[error_index].message, file_name);
	switch(error_type)
	{
		case ERROR_LABEL_NOT_BEGIN_WITH_ALPHA:
			strcat(out[error_index].message, ": label begins with a non alphabetic character in line\n\t");
			break;
		case ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON:
			strcat(out[error_index].message, ": incorrect label format, multiple words found pre colon in line\n\t");
			break;
		case ERROR_WORD_FOUND_PRE_MACR_KEYWORD:
			strcat(out[error_index].message, ": atleast one word found before \"macr\" statement in line\n\t");
			break;
		case ERROR_SOURCE_FILE_ACCESS:
			strcat(out[error_index].message, ": couldn't access file for reading.\n");
			break;
		case ERROR_DESTINATION_FILE_ACCESS:
			strcat(out[error_index].message, ": couldn't access file for writing.\n");
			break;
		case ERROR_PROGRAM_MEMORY_ALLOCATION:
			strcat(out[error_index].message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_SOURCE_FILE_MEMORY_ALLOCATION:
			strcat(out[error_index].message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_DESTINATION_FILE_MEMORY_ALLOCATION:
			strcat(out[error_index].message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_MACRO_NAME_EMPTY:
			strcat(out[error_index].message, ": empty macro name used in line\n\t");
			break;
		case ERROR_MACRO_NAME_RESERVED_WORD:
			strcat(out[error_index].message, ": reserved word used as macro name in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_MACR_KEYWORD:
			strcat(out[error_index].message, ": characters detected after macro declaration in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX:
			strcat(out[error_index].message, ": ilegal macro name used (must begin with an alphabetic letter & cannot contain punctuations) in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_UNIQUE:
			strcat(out[error_index].message, ": macro declared multiple times, second declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD:
			strcat(out[error_index].message, ": characters detected before end of macro declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_ENDMACR_KEYWORD:
			strcat(out[error_index].message, ": characters detected after end of macro declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_MACRO_NAME:
			strcat(out[error_index].message, ": characters detected after macro name in line\n\t");
			break;
		default:
			strcat(out[error_index].message, ": unknown error in line\n\t");
			break;
	}
	if (line_number)
	{
		out[error_index].line = line_number;
		itoa_base10(line_number, ln_str);
		strcat(out[error_index].message, ln_str);
		strcat(out[error_index].message, " - ");
		strcat(out[error_index].message, line);
	}
	error_index++;
/*	increment_error_array_index(out, error_index);*/
}

void print_errors(User_Output *out)
{
	int error_index = 0;
	/* should be done in a while for multiple errors */
	while (out[error_index++].message_type)
		printf("%s", out[error_index].message);
}

