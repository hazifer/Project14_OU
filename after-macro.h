#ifndef AFTERMACRO_H
#define AFTERMACRO_H

#include <stdlib.h>
#include <ctype.h>
#include "error-handling.h"
#include "string-utility.h"

typedef struct Label {
	char name[MAX_WORD_LENGTH];
	unsigned int address				: 16;
	unsigned int label_type				:  8;
	unsigned int is_entry				:  1;
} Label;

/* ansi doesn't allow for char bitfields? causes a */
typedef struct Command {
	unsigned int opcode				:  4;
	unsigned int src_addressing_type		:  4;
	unsigned int dst_addressing_type		:  4;
	unsigned int are_type				:  3;
} Command;

typedef struct Data_word {
	unsigned int value				: 12;
	unsigned int are_type				:  3;
} Data_word;

typedef struct Complex_data_word {
	unsigned int 					:  6;
	unsigned int src_register			:  3;
	unsigned int dst_register			:  3;
	unsigned int are_type				:  3;
} Complex_data_word;

typedef struct Print_format_word {
	unsigned int field1				:  3;
	unsigned int field2				:  3;
	unsigned int field3				:  3;
	unsigned int field4				:  3;
	unsigned int field5				:  3;
} Print_format_word;

typedef struct Word {
	unsigned int code_address			: 16; /* size is not affected if we make code_address 15 bits */
	unsigned int type				:  8; /* and is_command 1 bit, either way Word is a size of 2 integers */
	union Data {						      /* even when also trying to squeeze the Operation_build's size and value field to fit 16 bits */
		Command command; 
		Data_word data_word;
		Complex_data_word complex_data_word;
		Print_format_word print_format_word;
	} Data;
} Word;

int begin_assembler(char *fname, char *after_macro_fname, Word **word_array, Label **labels);

/* first_after_macro_scan: scans the input file after the macros were expanded.
 * assumes a proper after macro file was input.
 * returns 0 when no errors has occurred
 * returns 1 when a code error has occurred
 * returns ERROR_TERMINATE_ASSEMBLER when any sort of memory allocation error has occurred */
int first_after_macro_scan(FILE *fp, char *fname, Word **word_array, Label **labels);

int second_after_macro_scan(FILE *fp, char *fname, Word **word_array, Label **label_array);

int second_scan_read_entry_declaration(char *label_name, Label *label_array);

/* second_scan_read_two_arguments: assumes line is pointing to properly syntaxed two arguemnts for a command of type command_type as input
 * return 0 when the arguments are of correct addressing types for the command type
 * returns an error otherwise */
int second_scan_read_two_arguments(char *line, Word *word_array, int word_array_index, Label *label_array, int command_type);

/* after_macro_handle_label: responsible for label portion handling after the macro expansion.
 * calls for verify_label_syntax(), verify_label_unique(), and save_label()
 * returns the first error found from any of any above
 * return 0 if not error was found */
int after_macro_handle_label(char *line, char *colon_ptr, int instruction_address, Label **label_array, int *stored_label_index);

/* after_macro_save_words: saves words, their addresses and their values into the Word struct array
 * this is done starting a line, and the call ends with that line
 * after_macro_save_words saves the first syntax error from a line into int *error_return for the callee's use */
int after_macro_save_words(char *line, int instruction_address, int *error_return, Word **word_array, Label **label_array, char *command_type);
int after_macro_verify_command_till_arguments(char **line, char *command_code);
int after_macro_save_command_arguments(char *line, int instruction_address, char opcode, Word **word_array, int *error_return);
int after_macro_save_declaration_words(char *line, int instruction_address, char opcode, Word **word_array, Label **label_array, int *error_return);

int handle_addressing(char *word, int *src_value, int *error_return);

/* read_immediate_addressing: expects an integer in word, representing an immediate to store into word_value
 * returns ADDRESSING_TYPE_IMMEDIATE and sets word_value to the read immediate on success.
 * returns 0 and sets error_return to ERROR_IMMEDIATE_INVALID on failure */
int read_immediate_addressing(char *word, int *word_value, int *error_return);

/* read_indirect_register_addressing: expects a register in word, representing a register to be indirectly accessed.
 * returns ADDRESSING_TYPE_INDIRECT_REGISTER and sets word_value to the registers' index on success
 * returns 0 and sets error_return to ERROR_INDIRECT_REGISTER_INVALID on failure */
int read_indirect_register_addressing(char *word, int *word_value, int *error_return);

/* read_direct_register_addressing: expects a register in word, representing a register to be directly accessed.
 * returns ADDRESSING_TYPE_DIRECT_REGISTER and sets word_value to the registers' index on success
 * returns 0 and sets error_return to ERROR_DIRECT_REGISTER_INVALID on failure */
int read_direct_register_addressing(char *word, int *word_value, int *error_return);

/* read_string_declaration_data: assumes line points to a non blank character
 * reads a string and stores each character from it into word_array (with its address and ascii value)
 * returns number of characters read
 * sets errors in reading into error_return 
 * string should be in the format of "....", with no preceding or receding characters
 * the '"' is escaped into the string by \" */
int read_string_declaration_data(char *line, int instruction_address, Word **word_array, int *error_return);

/* read_data_declaration_data: assumes line points to a non blank character
 * reads a series of integers seperated by colons from a line, and stores each integer into word_array (with its address and integer value)
 * returns number of integers read
 * sets errors in reading into error_return */
int read_data_declaration_data(char *line, int instruction_address, Word **word_array, int *error_return);

/* first_read_entry_declaration_data: assumes line points to a non blank character
 * reads an already defined label and sets its label_type to TYPE_ENTRY
 * returns 0 on success
 * returns 1 on failure and sets the relevant error into error_return */
int first_read_entry_declaration_data(char *line, int instruction_address, Label **label_array, int *error_return);

/* first_read_extern_declaration_data: assumes line points to a non blank character
 * reads an already defined label and sets its label_type to TYPE_EXTERN
 * returns 0 on success
 * returns 1 on failure and sets the relevant error into error_return */
int first_read_extern_declaration_data(char *line, int instruction_address, Label ** label_array, int *error_return);

/* save_label: saves a label's name and decimal instruction address
 * returns 0 on success, storing the added label's index in label_array into stored_index
 * returns errors returned by increment_label_array_index():
 * ERROR_EXCEEDED_LABEL_ARRAY_LIMIT
 * ERROR_PROGRAM_MEMORY_ALLOCATION */
int save_label(char *input_label_name, Label **label_array, int instruction_address, int *stored_label_index);

int handle_syntax_post_relevant_arguments(char *line);

int save_word(int instruction_count, int value, char is_command, Word **word_array, int *word_array_index);

void increment_data_type_labels_address(Label *label_array, int address_increment);

int skip_data_words(char *line, Word *word_array, int word_array_index, int command_type);

int skip_command_argument_words(int word_array_index, Command current_command);

int second_read_entry_declaration(char *label_name, Label *label_array);

/* save_label_data_type: sets a label's data type by testing against a word input 
 * returns the type (although also stored into label_array[label_index]) */
char save_label_data_type(Label *label_array, int label_index, char *word);

char get_declaration_type(char *word);

/* verify_label_syntax: verifies label syntax from beginning of line to end (which is assumed to be a pointer to the first ':' in line)
 * returns errors according to issue
 * returns 0 when the syntax is okay */
int verify_label_syntax(char *line, char *end);

/* find_label_by_name: returns a label's index in label_array (lookup by label name) 
 * returns -1 if it doesn't exists */
int find_label_by_name(char *name, Label *label_array);

/* verify_label_syntax: verifies if a line's syntax (after a label) is okay.
 * returns 0 if it is
 * returns the following error types depending on case if it isn't:
 * */
int verify_line_syntax(char *line);

/* is_word_label: return the index of the label with the name input as word
 * return -1 if no such label exists */
int is_word_label(char *word, Label **label_array);

/* receives a possible op word, and returns it's decimal value opcode if it's an assembly word or -1 if it isn't */
char get_command_op_code(char *op);

int create_object_file(char *input, Word *word_array);
int create_extern_file(char *input, Label *label_array);
int create_entry_file(char *input, Label *label_array);

Label * init_label_array_memory();
Label * allocate_label_array_memory(Label *label_array, int *error_return);
Label * increment_label_array_index(Label *label_array, int next_label_index, int *error_return);
void reset_label_array_indices();

Word * init_word_array_memory();
Word * allocate_word_array_memory(Word *word_array, int *error_return);
Word * increment_word_array_index(Word *word_array, int next_word_index, int *error_return);
void reset_word_array_indices();

void print_labels(Label *label_array);
void print_words(Word *word_array);

/* is_label_type_exist: reads the label_array and returns 1 upon finding a label of the input type
 * returns 0 otherwise */
int is_label_type_exist(Label *label_array, int label_type);
/* is_entry_type_label_exist: reads the label_array and returns 1 upon finding a label of type entry
 * returns 0 otherwise */
int is_entry_type_label_exist(Label *label_array);


long convert_command_format_to_output_format(Command current_command);

#endif /* AFTERMACRO_H */
