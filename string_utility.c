#include <string.h>
#include "string_utility.h"

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
		word[len] = '\0';
	}
	else
		while (*line && *line != ' ' && *line != '\t' && *line != '\n')
			++line, ++len;
	return len;
}

void add_tabs_after_newline(char *content)
{
	/* what if the last character is '\n'? */
	/* is this even needed? maybe add tab to content expect in first line? */
	char tmp;
	char *new_end_index = content + strlen(content);
	int tabs_to_add = count_newlines(content) - 1;
	new_end_index += tabs_to_add;
	*new_end_index = '\0';
	while (tabs_to_add >= 0 && new_end_index > content)
	{
		if (*(new_end_index - tabs_to_add - 1) != '\n')
			{
				tmp = *new_end_index;
				*new_end_index = *(new_end_index - tabs_to_add - 1);
				*(new_end_index - tabs_to_add - 1) = tmp;
			}
		else
		{
			--tabs_to_add;
			*(new_end_index - tabs_to_add - 1) = '\t';
			--new_end_index;
		}
		--new_end_index;
	}
}

int count_newlines(char *content)
{
	int count = 0;
	for (; *content; ++content)
		if (*content == '\n')
			++count;
	return count;
}
