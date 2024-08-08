#include "error-handling.h"

void print_error(char *file_name, char *line, int error_type, int line_number)
{
	char num_str[MAX_LINE_DIGITS_IN_OUTPUT_FILE];
	char output_message[MAX_CHARS_IN_LINE * ERROR_MESSAGE_LINE_LENGTH_MULTIPLIER];
	strcpy(output_message, ERROR_BASE_STRING);
	strcat(output_message, file_name);
	
	switch(error_type)
	{	
		case ERROR_INTERGER_LIST_ENDING_WITH_COMMA:
			strcat(output_message, ": integer list ending with a comma in .data declaration in line ");
			break;
		case ERROR_COMMA_BEFORE_INTEGERS:
			strcat(output_message, ": comma before integers in .data declaration in line ");
			break;
		case ERROR_MULTIPLE_COMMAS_IN_INTEGER_LIST:
			strcat(output_message, ": consequtive commas in .data declaration in line ");
			break;
		case ERROR_BLANKS_BETWEEN_INTEGERS:
			strcat(output_message, ": blanks between integers in .data declaration in line ");
			break;
		case ERROR_NOT_AN_INTEGER:
			strcat(output_message, ": atleast one of the inputs is not an integer in .data declaration in line ");
			break;
		case ERROR_EMPTY_INTEGER_LIST:
			strcat(output_message, ": empty integer list in .data declaration in line ");
			break;
		case ERROR_STRING_DECLARATION_NOT_OPENING_WITH_QUOTES:
			strcat(output_message, ": incorrect string format for .string declaration, no quotes opening the string in line ");
			break;
		case ERROR_STRING_DECLARATION_CHARACTERS_AFTER_END_OF_QUOTES:
			strcat(output_message, ": incorrect string format for .string declaration, characters following the string in line ");
			break;
		case ERROR_MISSING_QUOTES_END:
			strcat(output_message, ": incorrect string format for .string declaration, missing quotes' end for string in line");
			break;
		case ERROR_COMMAND_UNKNOWN:
			/* add command to be logged? */
			strcat(output_message, ": unknown command used in line ");
			break;
		case ERROR_MISSING_ARGUMENTS:
			strcat(output_message, ": missing arguments for command in line ");
			break;
		case ERROR_LABEL_RESERVED_WORD:
			strcat(output_message, ": reserved language word used as label in line ");
			break;
		case ERROR_TOO_MANY_ARGUMENTS:
			strcat(output_message, ": too many arguments for command in line ");
			break;
		case ERROR_BLANK_BETWEEN_ARGUMENTS:
			strcat(output_message, ": blanks seperating arguments in line ");
			break;
		case ERROR_COMMA_AFTER_ARGUMENTS:
			strcat(output_message, ": comma detected at end of line in line ");
			break;
		case ERROR_COMMA_BEFORE_COMMAND:
			strcat(output_message, ": invalid command syntax, comma detected before command in line ");
			break;
		case ERROR_COMMA_AFTER_COMMAND:
			strcat(output_message, ": invalid command syntax, comma detected after command in line ");
			break;
		case ERROR_LABEL_DUPLICATE:
			strcat(output_message, ": an already defined label detected in line ");
			break;
		case ERROR_LABEL_NOT_BEGIN_WITH_ALPHA:
			strcat(output_message, ": label begins with a non alphabetic character in line ");
			break;
		case ERROR_LABEL_MULTIPLE_COLON:
			strcat(output_message, ": multiple colons found (possibly multiple label declarations) in line ");
			break;
		case ERROR_LABEL_EMPTY:
			strcat(output_message, ": empty label in line ");
			break;
		case ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON:
			strcat(output_message, ": incorrect label format, multiple words found pre colon in line ");
			break;
		case ERROR_WORD_FOUND_PRE_MACR_KEYWORD:
			strcat(output_message, ": atleast one word found before \"macr\" statement in line ");
			break;
		case ERROR_SOURCE_FILE_ACCESS:
			strcat(output_message, ": couldn't access file for reading.\n");
			break;
		case ERROR_DESTINATION_FILE_ACCESS:
			strcat(output_message, ": couldn't access file for writing.\n");
			break;
		case ERROR_PROGRAM_MEMORY_ALLOCATION:
			strcat(output_message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_SOURCE_FILE_MEMORY_ALLOCATION:
			strcat(output_message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_DESTINATION_FILE_MEMORY_ALLOCATION:
			strcat(output_message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_MACRO_NAME_EMPTY:
			strcat(output_message, ": empty macro name used in line ");
			break;
		case ERROR_MACRO_NAME_RESERVED_WORD:
			strcat(output_message, ": reserved word used as macro name in line ");
			break;
		case ERROR_WORD_FOUND_AFTER_MACR_KEYWORD:
			strcat(output_message, ": characters detected after macro declaration in line ");
			break;
		case ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX:
			strcat(output_message, ": ilegal macro name used (must begin with an alphabetic letter & cannot contain punctuations) in line ");
			break;
		case ERROR_MACRO_NAME_NOT_UNIQUE:
			strcat(output_message, ": macro declared multiple times, second declaration in line ");
			break;
		case ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD:
			strcat(output_message, ": characters detected before end of macro declaration in line ");
			break;
		case ERROR_WORD_FOUND_AFTER_ENDMACR_KEYWORD:
			strcat(output_message, ": characters detected after end of macro declaration in line ");
			break;
		case ERROR_WORD_FOUND_AFTER_MACRO_NAME:
			strcat(output_message, ": characters detected after macro name in line ");
			break;
		case ERROR_EXCEEDED_MACRO_ARRAY_LIMIT:
			strcat(output_message, ": Exceeded memory allocation limit, too many macros defined\n");
			break;
		case ERROR_EXCEEDED_OUTPUT_ARRAY_LIMIT:
			printf("shoudln't get here\n");
			strcat(output_message, ": too many errors in code, code ignored after line ");
			break;
		case ERROR_EXCEEDED_LABEL_ARRAY_LIMIT:
			strcat(output_message, ": Exceeded memory allocation limit, too many labels defined\n");
			break;
		case ERROR_EXCEEDED_WORD_ARRAY_LIMIT:
			strcat(output_message, ": Exceeded memory allocation limit, too many words defined\n");
			break;
		default:
			strcat(output_message, ": unknown error code (");
			itoa_base10(error_type, num_str);
			strcat(output_message, num_str);
			strcat(output_message, ") in line ");
			break;
	}
	if (line_number)
	{
		itoa_base10(line_number, num_str);
		strcat(output_message, num_str);
		strcat(output_message, ":\n\t\t");
		strcat(output_message, line);
	}
	printf("%s", output_message);
}
