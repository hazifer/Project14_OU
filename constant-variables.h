#ifndef CONSTANTVARIABLES_H
#define CONSTANTVARIABLES_H

#define ERROR_BASE_STRING 	"\terror in macro expansion for file "
#define	SUCCESS_BASE_STRING	"\tsuccessfuly completed macro expansion for file "
enum {
	SUCCESS = 				1,
	ERROR_SOURCE_FILE_MEMORY_ALLOCATION,
	ERROR_SOURCE_FILE_ACCESS,
	ERROR_DESTINATION_FILE_MEMORY_ALLOCATION,
	ERROR_DESTINATION_FILE_ACCESS,
	ERROR_PROGRAM_MEMORY_ALLOCATION,
	ERROR_WORD_FOUND_PRE_MACR_KEYWORD,
	ERROR_WORD_FOUND_PRE_ENDMACR_KEYWORD,
	ERROR_MACRO_NAME_EMPTY,
	ERROR_MACRO_NAME_RESERVED_WORD,
	ERROR_WORD_FOUND_AFTER_MACR_KEYWORD,
	ERROR_WORD_FOUND_AFTER_ENDMACR_KEYWORD,
	ERROR_WORD_FOUND_AFTER_MACRO_NAME,
	ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX,
	ERROR_MACRO_NAME_NOT_UNIQUE,
	ERROR_EXCEEDED_MACRO_ARRAY_LIMIT,
	ERROR_EXCEEDED_LABEL_ARRAY_LIMIT,
	ERROR_EXCEEDED_OUTPUT_ARRAY_LIMIT,
	ERROR_LABEL_NOT_BEGIN_WITH_ALPHA,
	ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON,
	ERROR_LABEL_EMPTY,
	ERROR_LABEL_MULTIPLE_COLON,
	ERROR_LABEL_DUPLICATE,

	STATE_COMMAND,
	STATE_MACRO_EXPANDED,
	STATE_COLLECT_MACRO_CONTENT,

	MACRO_LINE_COLLECTED,
	MACRO_EXPANDED,

	MAX_LINE_DIGITS_IN_OUTPUT_FILE =	10,
	MAX_FILENAME_LENGTH = 			32,
	MAX_WORD_LENGTH = 			32,
	MAX_MACRO_LINES =			50,
	MAX_CHARS_IN_LINE = 			81,
	MAX_OP_LENGTH =				5,
	MAX_LINES = 				100000,

	MACRO_ARRAY_INIT_SIZE =	 		25,
	MACRO_ARRAY_SIZE_MULTIPLIER_LIMIT = 	50,
	LABEL_ARRAY_INIT_SIZE =	 		25,
	LABEL_ARRAY_SIZE_MULTIPLIER_LIMIT = 	50,
	OUTPUT_ARRAY_INIT_SIZE = 		25,
	OUTPUT_ARRAY_SIZE_MULTIPLIER_LIMIT = 	50,
	ERROR_MESSAGE_LINE_LENGTH_MULTIPLIER =	44,
	BASE_IC_ADDRESS = 			100,
	NUMBER_OF_COMMANDS =			16
};

#endif /* CONSTANTVARIABLES_H */
