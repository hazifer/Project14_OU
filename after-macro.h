#include <stdlib.h>
#include <ctype.h>
#include "error-handling.h"
#include "string-utility.h"

typedef struct Label {
	char name[MAX_MACRO_NAME_LENGTH];
	unsigned int decimal_intruction_address;
} Label;

/* save_label: saves a label's name and decimal instruction address (using ic given) from a given line
 * returns errors according to issue:
 */
int save_label(char *line, Label *label_array, int line_number, int ic, User_Output *out);

int get_command_op_code_decimal(char *line, int line_number, int ic, User_Output *out);
