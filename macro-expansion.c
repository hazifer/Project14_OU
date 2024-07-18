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
		out->message_type = ERROR_SOURCE_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, extended_filename);
		strcat(out->message, ": couldn't access file for reading.\n");
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
		out->message_type = ERROR_DESTINATION_FILE_MEMORY_ALLOCATION;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ": couldn't create expanded macros file.\n");
		return 1;
	}
	strcpy(dfname, sfname);
	/* destination file is terminated with ".am" (as in after macro) */
	tmp = strstr(dfname, ".as");
	tmp[2] = 'm';
	fpin = fopen(sfname, "r");
	if (!fpin)
	{
		out->message_type = ERROR_SOURCE_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ": couldn't access file for reading.\n");
		return 1;
	}
	fpout = fopen(dfname, "w");
	if (!fpout)
	{
		out->message_type = ERROR_DESTINATION_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, dfname);
		strcat(out->message, ": couldn't access file for reading.\n");
		return 1;
	}
	line = (char *)malloc(MAX_CHARS_IN_LINE * sizeof(char));
	if (!line)
	{
		out->message_type = ERROR_LINE_MEMORY_ALLOCATION;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ": couldn't allocate enough memory for the program.\n");
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
	int ln = 0, state = COMMAND;
	while (fgets(line, MAX_CHARS_IN_LINE, fpin) && (li = expand_macros_handle_label(sfname, dfname, fpin, fpout, line, ++ln, out)))
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
		while (*li == ' ' || *li == '\t')
			++li;
		if (state == COMMAND)
		{
			/* look for "macr " or "macr\t" statement FIRST in line (li is currently pointing to a non blank, and not ':') */
			/* 
			li2 = verify_till_macr_word(li);
			if (!li2)
				fputs(li, fpout); */
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
		/* every line is indented one tab (the label part is "pulled" into the beginning of the line, so no blanks are printed pre-label */
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
	 * end + 1 is the character after ':' (end + 1 might already point to '\0' in the last line of the file) */
	tmp = *(end + 1);
	*(end + 1) = '\0';
	fputs(line, fpout);
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

char * verify_till_macr_word(char *line)
{
	char *macr_index;
	macr_index = strstr(line, "macr ");
	if (!macr_index)
		macr_index = strstr(line, "macr\t");
	if (!macr_index)
		return NULL;
	/* found macr but it's possible that it's of the type *macr */
	while (line < macr_index)
	{
		
	}
}

void log_error(User_Output *out, char *sfname, char *line, int error_type, int line_number)
{
	char *line_str;
	char ln_str[MAX_LINE_DIGITS_IN_OUTPUT_FILE];
	itoa_base10(line_number, ln_str);
	out->message_type = error_type;
	out->line = line_number;
	strcpy(out->message, ERROR_BASE_STRING);
	strcat(out->message, sfname);
	if (error_type == ERROR_LABEL_NOT_BEGIN_WITH_ALPHA)
	{
		strcat(out->message, ": label begins with a non alphabetic character in line\n\t");	
		strcat(out->message, ln_str);
		strcat(out->message, " - ");
		strcat(out->message, line);
	}
	else if (error_type == ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON)
	{
		strcat(out->message, ": incorrect label format, multiple words found pre colon in line\n\t");	
		strcat(out->message, ln_str);
		strcat(out->message, " - ");
		strcat(out->message, line);
	}
	else
	{
		strcat(out->message, ": unknown error in line\n\t");	
		strcat(out->message, ln_str);
		strcat(out->message, " - ");
		strcat(out->message, line);
	}
}
