#ifndef AFTERMACRO_H
#define AFTERMACRO_H

#include <stdlib.h>
#include <ctype.h>
#include "error-handling.h"
#include "string-utility.h"

typedef struct Label {
	char name[MAX_WORD_LENGTH];
	unsigned int decimal_instruction_address;
} Label;


int begin_assembler(char *fname, char *after_macro_fname, Label **labels, User_Output **out);

int first_after_macro_scan(FILE *fp, char *fname, Label **labels, User_Output **out);
/* save_label: saves a label's name and decimal instruction address (using ic given) from a given line
 * returns errors according to issue:
 */
int save_label(char *line, char *end, Label **label_array, int line_number, int instruction_address);

/* verify_label_syntax: verifies label syntax from beginning of line to end (which is assumed to be a pointer to the first ':' in line)
 * returns errors according to issue
 * returns 0 when the syntax is okay */
int verify_label_syntax(char *line, char *end);

/* receives a possible op word, and returns it's decimal value opcode if it's an assembly word or -1 if it isn't */
int get_command_op_code_decimal(char *op);

Label * allocate_label_array_memory(Label *label_array, int *error_return);
Label * increment_label_array_index(Label *label_array, int next_label_index, int *error_return);
Label * init_label_array_memory();
void reset_label_array_indices();

#endif /* AFTERMACRO_H */
