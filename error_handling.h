#include <stdio.h>
#include "string_utility.h"
#include "constant_variables.h"

typedef struct User_Output {
	int message_type;
	int line;
	char message[MAX_CHARS_IN_LINE];
} User_Output;

/* handle_errors_macro_expansion: prints the relevant macro expansion message
 * types of messages/error handling:
 * SUCCESS
 * ERROR_SOURCE_FILE_MEMORY_ALLOCATION
 * ERROR_DESTINATION_FILE_MEMORY_ALLOCATION
 * ERROR_SOURCE_FILE_ACCESS
 * ERROR_DESTINATION_FILE_ACCESS */
void handle_errors_macro_expansion(User_Output *out);

/* log_error: logs the relevant messages to print out to the user */
void log_error(User_Output *out, char *file_name, char *line, int error_type, int line_number);

