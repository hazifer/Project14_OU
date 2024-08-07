#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include "string-utility.h"
#include "constant-variables.h"

/* print_error: prints the an error message to stdin according to error_type */
void print_error(char *file_name, char *line, int error_type, int line_number);

#endif /* ERRORHANDLING_H */
