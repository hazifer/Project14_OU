#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "string_utility.h"

#define ERROR_BASE_STRING 	"\terror in macro expansion for file "
#define	SUCCESS_BASE_STRING	"\tsuccessfuly completed macro expansion for file "
enum 
{
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

typedef struct User_Output {
	int message_type;
	int line;
	char message[MAX_CHARS_IN_LINE];
} User_Output;

typedef struct Macro {
	char name[MAX_MACRO_NAME_LENGTH];
	char content[MAX_MACRO_LINES * MAX_CHARS_IN_LINE];
} Macro;

/* handle_errors_macro_expansion: prints the relevant macro expansion message
 * types of messages/error handling:
 * SUCCESS
 * ERROR_SOURCE_FILE_MEMORY_ALLOCATION
 * ERROR_DESTINATION_FILE_MEMORY_ALLOCATION
 * ERROR_SOURCE_FILE_ACCESS
 * ERROR_DESTINATION_FILE_ACCESS */
void handle_errors_macro_expansion(User_Output *out);

/* handle_filename_extension: adds the extension for the file name if needed
 * returns pointer to the file name with the extension added to it */
char * handle_filename_extension(char *sfname, char *dfname, User_Output *out);

/* expand_macros: expands the source file's macros into a similarly named file with an extension ".am"
 * returns 0 on success
 * returns 1 on failure
 * sets the relevant message to the user (according to case) using the struct User_Output, overriding old 'type' 'line' and 'message' fields */
int expand_macros(char *sfname, User_Output *out);
int expand_macros_memory_allocated(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, User_Output *out, Macro *macro_array);

/* expand_macros_print_label: writes a label from a given line into the output file
 * returns a pointer to the first ':' from a label in the given line
 * returns a pointer to the first non blank character when no label is found in the given line
 * also handles the first indentation of the line past the label (if exists) and if a label doesn't exist, written into the output file
 * NO ERROR HANDLING */
char * expand_macros_handle_label(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, int line_number, User_Output *out);

/* expand_macros_handle_command_state: handles the COMMAND state in the macro expansion stage 
 * returns errors according to each case:
 * 	ERROR_WORD_FOUND_PRE_MACRO_KEYWORD
 * 	ERROR_MACRO_NAME_EMPTY
 * 	ERROR_MACRO_NAME_RESERVED_WORD
 * 	ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX
 * 	ERROR_MACRO_NAME_NOT_UNIQUE
 * 	ERROR_MULTIPLE_WORDS_AFTER_MACRO_DECLARATION 
 * returns STATE_CHANGE_READ_MACRO_CONTENT when a correct format macro declaration was found (after uniqueness + syntax check)
 * returns 0 when the line was printed, or a macro was expanded (non of the previous cases occurred) */
int expand_macros_handle_command_state(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, int line_number, User_Output *out, Macro *macro_array, int next_macro_index);
int expand_macros_handle_collect_macro_content_state(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, int line_number, User_Output *out, Macro *macro_array, int next_macro_index);

/* log_error: logs the relevant messages to print out to the user */
void log_error(User_Output *out, char *file_name, char *line, int error_type, int line_number);

/* verify_till_macr_word: reads line and returns a pointer to the first occurrence of "macr " or "macr\t" in line if exists 
 * returns NULL and raises an error of type ERROR_WORD_FOUND_PRE_MACRO_KEYWORD into out in case there a words prior to "macr " or "macr\t"
 * returns NULL if it doesn't exist */
char * read_till_macr_keyword(char *line, User_Output *out);
char * read_till_endmacr_keyword(char *line, User_Output *out);

/* verify_not_reserved: verifies input word against a static set of reserved words.
 * returns 1 if word is not one of the set
 * returns 0 if it is part of the set */
char verify_not_reserved(char *word);

/* verify_macro_name_syntax: verifies if the macro name is in a correct format 
 * returns 1 if it is
 * returns 0 if it is not */
char verify_macro_name_syntax(char *word);

/* get_macro_name_index: given an array of Macro types, checks if word given is any Macro element's name field.
 * compares the input word to each element's name field from Macro array
 * returns -1 if it is unique (word isn't any of the elements)
 * returns integer representing the index, if it is not (word is the same of a specific Macro's name field) */
char get_macro_name_index(char *word, Macro *macro_array);

/* allocate_macro_array_memory: allocates memory for an array of struct Macro elements.
 * each call increments the allocated member count by MACRO_ARRAY_INIT_SIZE
 * returns a pointer to the memory address of the allocated array
 * returns NULL on failure
 * raises an error of type ERROR_EXCEEDED_MACRO_ARRAY_LIMIT if there was an attemt to allocate too many elements, limited by MACRO_ARRAY_INIT_SIZE * MACRO_ARRAY_SIZE_MULTIPLIER_LIMIT */
Macro * allocate_macro_array_memory(Macro *macro_array, User_Output *out);
Macro * increment_macro_array_index(Macro *macro_array, int next_macro_array, User_Output *out);

/* expand_macro: expands a macro's contents into the output file */
void expand_macro(FILE *fpout, Macro *macro_array, int macro_index);

/* is_newline_needed: computes and returns a boolean, according to state_value - true (1) or false (0) */
char is_newline_needed(int state_value);
