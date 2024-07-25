
#define ERROR_BASE_STRING 	"\terror in macro expansion for file "
#define	SUCCESS_BASE_STRING	"\tsuccessfuly completed macro expansion for file "

enum {
	SUCCESS = 1,
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
	STATE_COMMAND,
	STATE_MACRO_EXPANDED,
	STATE_COLLECT_MACRO_CONTENT,
	MACRO_LINE_COLLECTED,
	MACRO_EXPANDED,
	MAX_LINE_DIGITS_IN_OUTPUT_FILE = 10,
	MAX_FILENAME_LENGTH = 32,
	MAX_MACRO_NAME_LENGTH = 32,
	MAX_MACRO_LINES = 50,
	MAX_CHARS_IN_LINE = 81,
	MACRO_ARRAY_INIT_SIZE = 10,
	MACRO_ARRAY_SIZE_MULTIPLIER_LIMIT = 10
};
