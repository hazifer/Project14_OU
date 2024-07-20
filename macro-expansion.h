#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
	ERROR_LABEL_NOT_BEGIN_WITH_ALPHA,
	ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON,
	ERROR_WORD_FOUND_PRE_MACRO_KEYWORD,
	ERROR_MACRO_NAME_EMPTY,
	ERROR_MACRO_NAME_RESERVED_WORD,
	ERROR_MULTIPLE_WORDS_AFTER_MACRO_DECLARATION,
	ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX,
	ERROR_MACRO_NAME_NOT_UNIQUE,
	COMMAND,
	COLLECT_MACRO_CONTENT,
	MAX_LINE_DIGITS_IN_OUTPUT_FILE = 10,
	MAX_FILENAME_LENGTH = 32,
	MAX_MACRO_NAME_LENGTH = 32,
	MAX_MACRO_LINES = 50,
	MAX_CHARS_IN_LINE = 81
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
int expand_macros_memory_allocated(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, User_Output *out);

/* expand_macros_print_label: writes a label from a given line into the output file
 * returns a pointer to the first ':' from a label in the given line
 * returns a pointer to the first non blank character when no label is found in the given line
 * returns NULL when there is an issue with the label itself (such as reading multiple words prior to ':')
 * sets the relevant message to the user (according to case) using the struct User_Output, overriding old 'type' 'line' and 'message' fields 
 * also handles the first indentation of the line past the label (if exists) and if a label doesn't exist, written into the output file */
char * expand_macros_handle_label(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, int ln, User_Output *out);

/* itoa_base10: converts an input integer to string and sets it into n_str */
void itoa_base10(int n, char *n_str);

/* reverse_str: reverses an input string */
void reverse_str(char *str);

/* log_error: logs the relevant messages to print out to the user */
void log_error(User_Output *out, char *file_name, char *line, int error_type, int line_number);

/* verify_till_macr_word: reads line and returns a pointer to the first occurrence of "macr " or "macr\t" in line if exists 
 * returns NULL and raises an error of type ERROR_WORD_FOUND_PRE_MACRO_KEYWORD into out in case there a words prior to "macr " or "macr\t"
 * returns NULL if it doesn't exist */
char * verify_till_macr_word(char *line, User_Output *out);

/* skip_blanks: skips blanks from line
 * returns a pointer to the first non blank in line (\n and \0 aren't considered blanks) */
char * skip_blanks(char *line);

/* read_word: reads a single word from line and saves it into word 
 * returns length of word read */
int read_word(char *line, char *word);

/* verify_not_reserved: verifies input word against a static set of reserved words.
 * returns 1 if word is not one of the set
 * returns 0 if it is part of the set */
char verify_not_reserved(char *word);

/* verify_macro_name_syntax: verifies if the macro name is in a correct format 
 * returns 1 if it is
 * returns 0 if it is not */
char verify_macro_name_syntax(char *word);
