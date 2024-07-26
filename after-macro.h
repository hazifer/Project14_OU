#ifndef AFTERMACRO_H
#define AFTERMACRO_H

#include <stdlib.h>
#include <ctype.h>
#include "error-handling.h"
#include "string-utility.h"

typedef struct Label {
	char name[MAX_MACRO_NAME_LENGTH];
	unsigned int decimal_intruction_address;
} Label;


int begin_assembler(char *fname, char *after_macro_fname, Label *labels, User_Output *out);

int first_after_macro_scan(FILE *fp, char *fname, Label *labels, User_Output *out);
/* save_label: saves a label's name and decimal instruction address (using ic given) from a given line
 * returns errors according to issue:
 */
int save_label(char *line, char *end, Label *label_array, int line_number, int instruction_address, char *fname, User_Output *out);

/* receives a possible op word, and returns it's decimal value opcode if it's an assembly word or -1 if it isn't */
int get_command_op_code_decimal(char *op);

Label * allocate_label_array_memory(Label *label_array, User_Output *out);

#endif /* AFTERMACRO_H */
