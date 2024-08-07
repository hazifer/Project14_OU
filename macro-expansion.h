#ifndef MACROEXPANSION_H
#define MACROEXPANSION_H

#include <stdlib.h>
#include <ctype.h>
#include "error-handling.h"

typedef struct Macro {
	char name[MAX_WORD_LENGTH];
	char content[MAX_MACRO_LINES * MAX_CHARS_IN_LINE];
} Macro;

/* handle_filename_extension: adds the extension for the file name if needed
 * returns pointer to the file name with the extension added to it */
char * handle_filename_extension(char *sfname, char *dfname, int *error_return);

/* expand_macros: expands the source file's macros into a similarly named file with an extension ".am"
 * saves the .am file created into dfname_holder for the callee's use
 * returns 0 on success
 * returns 1 on failure
 * sets the relevant message to the user (according to case) using the struct User_Output, overriding old 'type' 'line' and 'message' fields */
int expand_macros(char *sfname, char *dfname_holder, int *error_return);
int expand_macros_memory_allocated(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, Macro **macro_array, int *error_return);

/* expand_macros_print_label: writes a label from a given line into the output file
 * returns a pointer to the first ':' from a label in the given line
 * returns a pointer to the first non blank character when no label is found in the given line
 * also handles the first indentation of the line past the label (if exists) and if a label doesn't exist, written into the output file
 * NO ERROR HANDLING */
char * expand_macros_handle_label(char *line, FILE *fpout);

/* expand_macros_handle_command_state: handles the COMMAND state in the macro expansion stage 
 * returns errors according to each case:
 * 	ERROR_WORD_FOUND_PRE_MACRO_KEYWORD
 * 	ERROR_MACRO_NAME_EMPTY
 * 	ERROR_MACRO_NAME_RESERVED_WORD
 * 	ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX
 * 	ERROR_MACRO_NAME_NOT_UNIQUE
 * 	ERROR_MULTIPLE_WORDS_AFTER_MACRO_DECLARATION 
 * returns STATE_CHANGE_READ_MACRO_CONTENT when a correct format macro declaration was found (after uniqueness + syntax check)
 * returns 0 when the line was printed, or a macro was expanded (non of the previous cases occurred) */
int expand_macros_handle_command_state(char *line, FILE *fpout, Macro *macro_array, int next_macro_index);
int expand_macros_handle_collect_macro_content_state(char *line, Macro *macro_array, int next_macro_index);

/* verify_till_macr_word: reads line and returns a pointer to the first occurrence of "macr " or "macr\t" in line if exists 
 * returns NULL and raises an error of type ERROR_WORD_FOUND_PRE_MACRO_KEYWORD into out in case there a words prior to "macr " or "macr\t"
 * returns NULL if it doesn't exist */
char * read_till_macr_keyword(char *line, int *error_return);
char * read_till_endmacr_keyword(char *line, int *error_return);


/* verify_macro_name_syntax: verifies if the macro name is in a correct format 
 * returns 1 if it is
 * returns 0 if it is not */
char verify_macro_name_syntax(char *word);

/* get_macro_name_index: given an array of Macro types, checks if word given is any Macro element's name field.
 * compares the input word to each element's name field from Macro array
 * returns -1 if it is unique (word isn't any of the elements)
 * returns integer representing the index, if it is not (word is the same of a specific Macro's name field) */
char get_macro_name_index(char *word, Macro *macro_array);

/* allocate_macro_array_memory: allocates memory for an array of struct Macro elements.
 * each call increments the allocated member count by MACRO_ARRAY_INIT_SIZE
 * returns a pointer to the memory address of the allocated array
 * returns NULL on failure
 * raises an error of type ERROR_EXCEEDED_MACRO_ARRAY_LIMIT if there was an attemt to allocate too many elements, limited by MACRO_ARRAY_INIT_SIZE * MACRO_ARRAY_SIZE_MULTIPLIER_LIMIT */
Macro * allocate_macro_array_memory(Macro *macro_array, int *error_return);
Macro * increment_macro_array_index(Macro *macro_array, int next_macro_array, int *error_return);
Macro * init_macro_array_memory();
void reset_macro_array_indices();

/* expand_macro: expands a macro's contents into the output file */
void expand_macro(FILE *fpout, Macro *macro_array, int macro_index);

/* is_newline_needed: computes and returns a boolean, according to state_value - true (1) or false (0) */
char is_newline_needed(int state_value);

#endif /* MACROEXPANSION_H */
