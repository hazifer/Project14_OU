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
	ERROR_LINE_MEMORY_ALLOCATION,
	ERROR_LABEL_NOT_BEGIN_WITH_ALPHA,
	ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON,
	MAX_FILENAME_LENGTH = 32,
	MAX_MACRO_NAME_LENGTH = 32,
	MAX_MACRO_LINES = 50,
	MAX_CHARS_IN_LINE = 81,
	MAX_LINES_IN_OUTPUT_FILE = 1000
};

typedef struct User_Output {
	int type;
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

/* expand_macros_handle_label: writes a label from a given line into the output file
 * returns a pointer to the first ':' from a label in the given line
 * returns a pointer to the first non blank character when no label is found in the given line
 * returns NULL when there is an issue with the label itself (such as reading multiple words prior to ':' 
 * sets the relevant message to the user (according to case) using the struct User_Output, overriding old 'type' 'line' and 'message' fields */
char * expand_macros_handle_label(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, int ln, User_Output *out);

/* itoa_base10: converts an input integer to string and sets it into n_str */
void itoa_base10(int n, char *n_str);
/* reverse_str: reverses an input string */
void reverse_str(char *str);
