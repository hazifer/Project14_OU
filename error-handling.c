#include "error-handling.h"

void log_error(User_Output *out, char *file_name, char *line, int error_type, int line_number)
{
	char ln_str[MAX_LINE_DIGITS_IN_OUTPUT_FILE];
	out->message_type = error_type;
	strcpy(out->message, ERROR_BASE_STRING);
	strcat(out->message, file_name);
	switch(error_type)
	{
		/*
		case ERROR_LABEL_NOT_BEGIN_WITH_ALPHA:
			strcat(out->message, ": label begins with a non alphabetic character in line\n\t");
			break;
		case ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON:
			strcat(out->message, ": incorrect label format, multiple words found pre colon in line\n\t");
			break;*/
		case ERROR_WORD_FOUND_PRE_MACR_KEYWORD:
			strcat(out->message, ": atleast one word found before \"macr\" statement in line\n\t");
			break;
		case ERROR_SOURCE_FILE_ACCESS:
			strcat(out->message, ": couldn't access file for reading.\n");
			break;
		case ERROR_DESTINATION_FILE_ACCESS:
			strcat(out->message, ": couldn't access file for writing.\n");
			break;
		case ERROR_PROGRAM_MEMORY_ALLOCATION:
			strcat(out->message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_SOURCE_FILE_MEMORY_ALLOCATION:
			strcat(out->message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_DESTINATION_FILE_MEMORY_ALLOCATION:
			strcat(out->message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_MACRO_NAME_EMPTY:
			strcat(out->message, ": empty macro name used in line\n\t");
			break;
		case ERROR_MACRO_NAME_RESERVED_WORD:
			strcat(out->message, ": reserved word used as macro name in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_MACR_KEYWORD:
			strcat(out->message, ": characters detected after macro declaration in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX:
			strcat(out->message, ": ilegal macro name used (must begin with an alphabetic letter & cannot contain punctuations) in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_UNIQUE:
			strcat(out->message, ": macro declared multiple times, second declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD:
			strcat(out->message, ": characters detected before end of macro declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_ENDMACR_KEYWORD:
			strcat(out->message, ": characters detected after end of macro declaration in line\n\t");
			break;
		case ERROR_WORD_FOUND_AFTER_MACRO_NAME:
			strcat(out->message, ": characters detected after macro name in line\n\t");
			break;
		default:
			strcat(out->message, ": unknown error in line\n\t");
			break;
	}
	if (line_number)
	{
		out->line = line_number;
		itoa_base10(line_number, ln_str);
		strcat(out->message, ln_str);
		strcat(out->message, " - ");
		strcat(out->message, line);
	}
}

void handle_errors_macro_expansion(User_Output *out)
{
	/* should be done in a while for multiple errors */
	if (out->message_type)
		printf("%s", out->message);
}

