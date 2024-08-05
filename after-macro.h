#ifndef AFTERMACRO_H
#define AFTERMACRO_H

#include <stdlib.h>
#include <ctype.h>
#include "error-handling.h"
#include "string-utility.h"

typedef struct Label {
	char name[MAX_WORD_LENGTH];
	char label_type;
	unsigned int decimal_instruction_address;
} Label;

typedef struct Word {
	unsigned int decimal_instruction_address	: 16;
	union {
		unsigned int value			: 16; 
		struct operation {
			unsigned int opcode		:  4;
			unsigned int src_addressing_type:  4;
			unsigned int dst_addressing_type:  4;
			unsigned int are_type		:  4;
		} operation;
	} Data;
} Word;

int begin_assembler(char *fname, char *after_macro_fname, Word **word_array, Label **labels, User_Output **out);

int first_after_macro_scan(FILE *fp, char *fname, Word **word_array, Label **labels, User_Output **out);

/* after_macro_handle_label: responsible for label portion handling after the macro expansion.
 * calls for verify_label_syntax(), verify_label_unique(), and save_label()
 * returns the first error seen from any of any above and sets it into error_return
 * sets error_return to 0 if no error occurred and all of the above functions succeeded */
void after_macro_handle_label(char *line, char *colon_ptr, int line_number, int instruction_address, Label **label_array, int *error_return, int *stored_label_index);

/* after_macro_save_words: saves words, their addresses and their values into the Word struct array
 * this is done starting a line, and the call ends with that line
 * after_macro_save_words saves the first syntax error from a line into int *error_return for the callee's use */
void after_macro_save_words(char *line, int instructions_address, int *error_return, Word **word_array);
int after_macro_verify_command_till_arguments(char *line, char *command_code);

/* save_label: saves a label's name and decimal instruction address (using ic given) from a given line
 * returns 0 on success, storing the added label's index in label_array into stored_index
 * returns errors returned by increment_label_array_index():
 * ERROR_EXCEEDED_LABEL_ARRAY_LIMIT
 * ERROR_PROGRAM_MEMORY_ALLOCATION
 */
int save_label(char *line, char *end, Label **label_array, int line_number, int instruction_address, int *stored_label_index);

int save_word(char *line, char *end, Word **word_array, int line_number, int instruction_address);

/* save_label_data_type: sets a label's data type by testing against a word input 
 * returns the type (although also stored into label_array[label_index]) */
char save_label_data_type(Label *label_array, int label_index, char *word);

char get_declaration_type(char *word);

/* verify_label_syntax: verifies label syntax from beginning of line to end (which is assumed to be a pointer to the first ':' in line)
 * returns errors according to issue
 * returns 0 when the syntax is okay */
int verify_label_syntax(char *line, char *end);

/* verify_label_unique: verifies if a label starting at line and ending at end (assumes end points to ':') is unique
 * returns 0 if it is
 * returns ERROR_LABEL_DUPLICATE if it isn't */
int verify_label_unique(char *line, char *end, Label **label_array);

/* verify_label_syntax: verifies if a line's syntax (after a label) is okay.
 * returns 0 if it is
 * returns the following error types depending on case if it isn't:
 * */
int verify_line_syntax(char *line);

/* receives a possible op word, and returns it's decimal value opcode if it's an assembly word or -1 if it isn't */
char get_command_op_code_decimal(char *op);

Label * init_label_array_memory();
Label * allocate_label_array_memory(Label *label_array, int *error_return);
Label * increment_label_array_index(Label *label_array, int next_label_index, int *error_return);
void reset_label_array_indices();

Word * init_word_array_memory();
Word * allocate_word_array_memory(Word *word_array, int *error_return);
Word * increment_word_array_index(Word *word_array, int next_word_index, int *error_return);
void reset_word_array_indices();

void print_labels(Label *label_array);

#endif /* AFTERMACRO_H */
