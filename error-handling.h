#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include "string-utility.h"
#include "constant-variables.h"

typedef struct User_Output {
	int message_type;
	int line;
	char message[MAX_CHARS_IN_LINE * ERROR_MESSAGE_LINE_LENGTH_MULTIPLIER];
} User_Output;

/* handle_errors_macro_expansion: prints the relevant macro expansion message
 * types of messages/error handling:
 * SUCCESS
 * ERROR_SOURCE_FILE_MEMORY_ALLOCATION
 * ERROR_DESTINATION_FILE_MEMORY_ALLOCATION
 * ERROR_SOURCE_FILE_ACCESS
 * ERROR_DESTINATION_FILE_ACCESS */
void print_errors(User_Output *out);

/* log_error: logs the relevant messages to print out to the user */
void log_error(User_Output **out, char *file_name, char *line, int error_type, int line_number, int *error_return);

User_Output * increment_output_array_index(User_Output *out, int next_output_index, int *error_return);

User_Output * allocate_output_array_memory(User_Output *out, int *error_return);

#endif /* ERRORHANDLING_H */
