#include "string-utility.h"

void itoa_base10(int n, char *n_str)
{
	/* assumes enough memory for n_str, assumes n >= 0 */
	int i = 0;
	while (n)
	{
		*(n_str + i) = n % 10 + '0';
		++i;
		n /= 10;
	}
	*(n_str + i) = '\0';
	reverse_str(n_str);
}

void reverse_str(char *str)
{
	int len = strlen(str);
	int i;
	char tmp;
	for (i = 0; i < len / 2; ++i)
	{
		tmp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = tmp;
	}
}

char * skip_blanks(char *line)
{
	while (*line == ' ' || *line == '\t')
		++line;
	return line;
}

int read_word(char *line, char *word)
{
	int len = 0;
	line = skip_blanks(line);
	if (word)
	{
		while ((word[len] = *line) && word[len] != ' ' && word[len] != '\t' && word[len] != '\n')
			++line, ++len;
		/* not needed? */
		word[len] = '\0';
	}
	else
		while (*line && *line != ' ' && *line != '\t' && *line != '\n')
			++line, ++len;
	return len;
}

int read_word_delimited(char *line, char *word, char *delimiters)
{
	int len, i;
	char c;
	len = i = 0;
	while ((c = word[len] = line[len]) && c != ' ' && c != '\t' && c != '\n' && !strchr(delimiters, c))
		++len;
	word[len] = '\0';
	return len;
}

int count_newlines(char *content)
{
	int count = 0;
	for (; *content; ++content)
		if (*content == '\n')
			++count;
	return count;
}

int count_words_in_line(char *line, char *delimiters, const int max_word_length)
{
	int words = 0;

	return words;
}

char verify_not_reserved(char *word)
{
	char *reserved_words[] = { "mov", "cmp", "add", "sub", 
				   "lea", "clr", "not", "inc", 
				   "dec", "jmp", "bne", "red", 
				   "prn", "jsr", "rts", "stop",
       				   "macr", "endmacr", ".data", ".string",
				   ".extern", ".entry"	};
	int arr_len = sizeof(reserved_words) / sizeof(char *);
	for (arr_len--; arr_len >= 0; arr_len--)
		if (!strcmp(reserved_words[arr_len], word))
				return 0;
	return 1;
}
