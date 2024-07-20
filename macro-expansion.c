#include "macro-expansion.h"

char * handle_filename_extension(char *filename, char extension[], User_Output *out)
{
	/* only handle cases of no .** extension, and an entirely existing .** extension */
	char *extended_filename;
	FILE *fp;
	if (strstr(filename, extension))
		extended_filename = filename;
	else
	{
		/* in the event that the source filename is not ending with ".as" */
		extended_filename = (char *)malloc((strlen(filename) + 3) * sizeof(char)); /* +3 for ".as" extension */
		/* error handling */
		if(!extended_filename)
		{
			out->message_type = ERROR_SOURCE_FILE_MEMORY_ALLOCATION;
			strcpy(out->message, ERROR_BASE_STRING);
			strcat(out->message, filename);
			strcat(out->message, extension);
			strcat(out->message, ": couldn't allocate enough memory for the program.\n");
			return NULL;
		}
		extended_filename[0] = '\0';
		strcat(extended_filename, filename);
		strcat(extended_filename, extension);
	}
	/* testing for read access of the input file */
	fp = fopen(extended_filename, "r");
	if (!fp)
	{

		return NULL;
	}
	fclose(fp);
	out->message_type = SUCCESS;
	strcpy(out->message, SUCCESS_BASE_STRING);
	strcat(out->message, extended_filename);
	strcat(out->message, "\n");
	return extended_filename;
}

int expand_macros(char *sfname, User_Output *out)
{
	FILE *fpin, *fpout;
	char *dfname, *line, *tmp;
	/* memory allocation, file access, and error handling */
	dfname = (char *)malloc(strlen(sfname) * sizeof(char));
	if (!dfname)
	{
		log_error(out, sfname, NULL, ERROR_DESTINATION_FILE_MEMORY_ALLOCATION, 0); /* line_number = 0 to ignore line in log_error */
		return 1;
	}
	strcpy(dfname, sfname);
	/* destination file is terminated with ".am" (as in after macro) */
	tmp = strstr(dfname, ".as");
	tmp[2] = 'm';
	fpin = fopen(sfname, "r");
	if (!fpin)
	{
		log_error(out, sfname, NULL, ERROR_SOURCE_FILE_ACCESS, 0); /* line_number = 0 to ignore line in log_error */
		return 1;
	}
	fpout = fopen(dfname, "w");
	if (!fpout)
	{
		log_error(out, dfname, NULL, ERROR_DESTINATION_FILE_ACCESS, 0); /* line_number = 0 to ignore line in log_error */
		return 1;
	}
	line = (char *)malloc(MAX_CHARS_IN_LINE * sizeof(char));
	if (!line)
	{
		log_error(out, sfname, line, ERROR_PROGRAM_MEMORY_ALLOCATION, 0); /* line_number = 0 to ignore line in log_error */
		return 1;
	}
	expand_macros_memory_allocated(sfname, dfname, fpin, fpout, line, out);
	/* sfname is freed in main */
	if (out->message_type != SUCCESS)
		remove(dfname);
	free(dfname);
	fclose(fpin);
	fclose(fpout);
	free(line);
	return 0;
}

void handle_errors_macro_expansion(User_Output *out)
{
	if (out->message_type)
		printf("%s", out->message);
}

int expand_macros_memory_allocated(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, User_Output *out)
{
	char *li, *li2; /* line indexes */
	char word[MAX_MACRO_NAME_LENGTH];
	int line_number = 0, state = COMMAND;
	while (fgets(line, MAX_CHARS_IN_LINE, fpin) && (li = expand_macros_handle_label(sfname, dfname, fpin, fpout, line, ++line_number, out)))
	{
		/* line was read, and li is not NULL */
		if (*li == ';' || *li == '\n')
		{
			/* found a comment or a blank line - output a newline and skip this line */
			fputc('\n', fpout);
			continue;
		}
		/* not a comment nor a blank line */
		if (*li == ':')
			++li;
		li = skip_blanks(li);
		if (state == COMMAND)
		{
			/* look for "macr " or "macr\t" statement FIRST in line (li is currently pointing to a non blank, and not ':') */
			li2 = verify_till_macr_word(li, out);
			if (li2 == NULL && out->message_type == ERROR_WORD_FOUND_PRE_MACRO_KEYWORD)
			{
				log_error(out, sfname, line, ERROR_WORD_FOUND_PRE_MACRO_KEYWORD, line_number);
				break;
			}
			if (li2 == NULL)
			{
				/* no "macr " or "macr\t" found, check for already declared macro names against a single word in line, or output line */
				li = skip_blanks(li);
				fputs(li, fpout);
				continue;
			}
			/* point past "macr " or "macr\t" and get the word, treat it as a macro name if only one word exists */
			li2 = li2 + strlen("macr");
			li2 = skip_blanks(li2);
			if (!read_word(li2, word))
			{
				log_error(out, sfname, line, ERROR_MACRO_NAME_EMPTY, line_number);
				break;
			}
			if (!verify_not_reserved(word))
			{
				log_error(out, sfname, line, ERROR_MACRO_NAME_RESERVED_WORD, line_number);
				break;
			}
			if (!verify_macro_name_syntax(word))
			{
				log_error(out, sfname, line, ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX, line_number);
				break;
			}
			if (!verify_macro_name_unique(word, ))
			{
				log_error(out, sfname, line, ERROR_MACRO_NAME_NOT_UNIQUE, line_number);
				break;
			}
			li2 += strlen(word);
			if (read_word(li2, word))
			{
				log_error(out, sfname, line, ERROR_MULTIPLE_WORDS_AFTER_MACRO_DECLARATION, line_number);
				break;
			}
			/* change state if needed */
			/* print already "collected" macros */
		}
		else if (state == COLLECT_MACRO_CONTENT)
		{

		}
		/*
		li = expand_macros_handle_macro(sfname, dfname, fpin, fpout, line, ln, out);
		fputc('\n', fpout);*/ /* REMOVE THIS */
	}
	if (out->message_type != SUCCESS)
		return 1;
	return 0;
}

char * expand_macros_handle_label(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, int line_number, User_Output *out)
{
	char *end = strchr(line, ':'), tmp;
	int li = 0;
	while (*line == ' ' || *line == '\t')
		++line;
	if (!end)
	{
		/* every line is indented one tab (the label part is "pulled" into the beginning of the line, so no blanks are printed pre-label 
		 * in case of a comment or a new line here, it means we should skip this line (handled in the callee) 
		 * otherwise we indent one tab as there is no label at all */
		if (*line != ';' && *line != '\n')
			fputc('\t', fpout);
		return line;
	}
	if (!isalpha(*line))
	{
		log_error(out, sfname, line, ERROR_LABEL_NOT_BEGIN_WITH_ALPHA, line_number);
		return NULL;
	}
	while (line + li < end)
	{
		if (*(line + li) == ' ' || *(line + li) == '\t')
		{
			log_error(out, sfname, line, ERROR_LABEL_NOT_BEGIN_WITH_ALPHA, line_number);
			return NULL;
		}
		++li;
	}
	/* line + li reached end which points to ':'
	 * line points to first non blank character in line
	 * end + 1 is the character after ':' (end + 1 might already point to '\0' in the last line of the file) 
	 * we handle indentation into output */
	tmp = *(end + 1);
	*(end + 1) = '\0';
	fputs(line, fpout);
	fputc('\t', fpout);
	*(end + 1) = tmp;
	return line + li;
}

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

char * verify_till_macr_word(char *line, User_Output *out)
{
	/* assumes line is pointing to a non blank, past the label of the line */
	char *macr_index;
	macr_index = strstr(line, "macr ");
	if (!macr_index)
		macr_index = strstr(line, "macr\t");
	if (!macr_index)
		macr_index = strstr(line, "macr\n");
	if (!macr_index)
	{
		return NULL;
	}
	if (line[strlen("macr\n")] == '\n')
	{
		out->message_type = ERROR_MACRO_NAME_EMPTY;
		return NULL;
	}
	if (line < macr_index) /* line doesn't begin with "macr "/"macr\t" */
	{
		if (*(macr_index - 1) == ' ' || *(macr_index -1) == '\t') /* since line began with a non blank, this must mean there is atleast one word before "macr "/"macr\t" */
		{
			out->message_type = ERROR_WORD_FOUND_PRE_MACRO_KEYWORD;
			return NULL;
		}
	}
	/* if code reaches here, then a word of *macr syntax exists (* is a non blank, possibly multiple characters))
	 * it is okay if it's not the first in line. it might or might not be first in line
	 * checking either of cases is not in the scope of this function */
	while (*line && *line != '\n' && (*line == ' ' || *line == '\t')) /* skip blanks */
			++line;
	return line;
}

void log_error(User_Output *out, char *file_name, char *line, int error_type, int line_number)
{
	char ln_str[MAX_LINE_DIGITS_IN_OUTPUT_FILE];
	out->message_type = error_type;
	strcpy(out->message, ERROR_BASE_STRING);
	strcat(out->message, file_name);
	switch(error_type)
	{
		case ERROR_LABEL_NOT_BEGIN_WITH_ALPHA:
			strcat(out->message, ": label begins with a non alphabetic character in line\n\t");
			break;
		case ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON:
			strcat(out->message, ": incorrect label format, multiple words found pre colon in line\n\t");
			break;
		case ERROR_WORD_FOUND_PRE_MACRO_KEYWORD:
			strcat(out->message, ": reserved keyword \"macr\" used in line\n\t");
			break;
		case ERROR_SOURCE_FILE_ACCESS:
			strcat(out->message, ": couldn't access file for reading.\n");
			break;
		case ERROR_DESTINATION_FILE_ACCESS:
			strcat(out->message, ": couldn't access file for writing.\n");
			break;
		case ERROR_PROGRAM_MEMORY_ALLOCATION:
			strcat(out->message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_SOURCE_FILE_MEMORY_ALLOCATION:
			strcat(out->message, ": couldn't allocate enough memory for the program.\n");
			break;
		case ERROR_DESTINATION_FILE_MEMORY_ALLOCATION:
			strcat(out->message, ": couldn't create expanded macros file.\n");
			break;
		case ERROR_MACRO_NAME_EMPTY:
			strcat(out->message, ": empty macro name used in line\n\t");
			break;
		case ERROR_MACRO_NAME_RESERVED_WORD:
			strcat(out->message, ": reserved word used as macro name in line\n\t");
			break;
		case ERROR_MULTIPLE_WORDS_AFTER_MACRO_DECLARATION:
			strcat(out->message, ": multiple words after macro declaration in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_IN_LEGAL_SYNTAX:
			strcat(out->message, ": ilegal macro name used (must begin with an alphabetic letter & cannot contain punctuations) in line\n\t");
			break;
		case ERROR_MACRO_NAME_NOT_UNIQUE:
			strcat(out->message, ": macro declared multiple times, second declaration in line\n\t");
			break;
		default:
			strcat(out->message, ": unknown error in line\n\t");
			break;
	}
	if (line_number)
	{
		out->line = line_number;
		itoa_base10(line_number, ln_str);
		strcat(out->message, ln_str);
		strcat(out->message, " - ");
		strcat(out->message, line);
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
	while ((word[len] = *line) && word[len] != ' ' && word[len] != '\t' && word[len] != '\n')
		++line, ++len;
	word[len] = '\0';
	return len;
}

char verify_not_reserved(char *word)
{
	char *reserved_words[] = { "big", "nob", "test9", "haha" };
	int arr_len = sizeof(reserved_words) / sizeof(char *);
	for (arr_len--; arr_len >= 0; arr_len--)
		if (!strcmp(reserved_words[arr_len], word))
				return 0;
	return 1;
}

char verify_macro_name_syntax(char *word)
{
	/* assumes word is terminated with '\0' and not '\n' */
	if (isdigit(*word))
		return 0;
	while (*word)
		if (ispunct(*word++))
			return 0;
	return 1;
}
