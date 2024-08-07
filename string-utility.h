#ifndef STRINGUTILITY_H
#define STRINGUTILITY_H

#include <string.h>

/* itoa_base10: converts an input integer to string and sets it into n_str */
void itoa_base10(int n, char *n_str);

/* reverse_str: reverses an input string */
void reverse_str(char *str);

/* skip_blanks: skips blanks from line
 * returns a pointer to the first non blank in line (\n and \0 aren't considered blanks) */
char * skip_blanks(char *line);

/* read_word: reads a single word from line and saves it into word, delimited by blanks 
 * returns length of word read */
int read_word(char *line, char *word);
/* read_word_delimited: reads a single word from line and saves it into word, delimited by the given delimiters array of chars
 * returns length of word read */
int read_word_delimited(char *line, char *word, char *delimiters);

/* add_tabs_after_newline: adds a tab character '\t' after every '\n' in a given content */
void add_tabs_after_newline(char *content);

/* count_newlines: counts and returns the number of '\n' characters in a given string */
int count_newlines(char *content);

/* count_words_in_line: returns the number of words scanned delimited by the given delimiters */
int count_words_in_line(char *line, char *delimiters, const int max_word_length);

/* verify_not_reserved: verifies input word against a static set of reserved words.
 * returns 1 if word is not one of the set
 * returns 0 if it is part of the set */
char verify_not_reserved(char *word);

#endif /* STRINGUTILITY_H */
