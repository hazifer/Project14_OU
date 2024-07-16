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
	ERROR_DESTINATION_FILE_MEMORY_ALLOCATION,
	ERROR_SOURCE_FILE_ACCESS,
	ERROR_DESTINATION_FILE_ACCESS,
	MAX_FILENAME_LENGTH = 32,
	MAX_MACRO_NAME_LENGTH = 32,
	MAX_MACRO_LINES = 50,
	MAX_CHARS_IN_LINE = 81
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

/* handle_filename_extension: adds the extension for the file name if needed
 * returns pointer to the file name with the extension added to it */
char * handle_filename_extension(char *, char [], User_Output *);

/* handle_errors_macro_expansion: prints the relevant macro expansion error message */
void handle_errors_macro_expansion(User_Output *);

int expand_macros(char *, User_Output *);
