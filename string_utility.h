/* itoa_base10: converts an input integer to string and sets it into n_str */
void itoa_base10(int n, char *n_str);

/* reverse_str: reverses an input string */
void reverse_str(char *str);

/* skip_blanks: skips blanks from line
 * returns a pointer to the first non blank in line (\n and \0 aren't considered blanks) */
char * skip_blanks(char *line);

/* read_word: reads a single word from line and saves it into word 
 * returns length of word read */
int read_word(char *line, char *word);

/* add_tabs_after_newline: adds a tab character '\t' after every '\n' in a given content */
void add_tabs_after_newline(char *content);

/* count_newlines: counts and returns the number of '\n' characters in a given string */
int count_newlines(char *content);
