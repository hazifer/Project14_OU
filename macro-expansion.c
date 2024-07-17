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
			out->type = ERROR_SOURCE_FILE_MEMORY_ALLOCATION;
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
		out->type = ERROR_SOURCE_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, extended_filename);
		strcat(out->message, ": couldn't access file for reading.\n");
		return NULL;
	}
	fclose(fp);
	out->type = SUCCESS;
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
		out->type = ERROR_DESTINATION_FILE_MEMORY_ALLOCATION;
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
		out->type = ERROR_SOURCE_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ": couldn't access file for reading.\n");
		return 1;
	}
	fpout = fopen(dfname, "w");
	if (!fpout)
	{
		out->type = ERROR_DESTINATION_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, dfname);
		strcat(out->message, ": couldn't access file for reading.\n");
		return 1;
	}
	line = (char *)malloc(MAX_CHARS_IN_LINE * sizeof(char));
	if (!line)
	{
		out->type = ERROR_LINE_MEMORY_ALLOCATION;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ": couldn't allocate enough memory for the program.\n");
		return 1;
	}
	expand_macros_memory_allocated(sfname, dfname, fpin, fpout, line, out);
	/* sfname is freed in main */
	free(dfname);
	fclose(fpin);
	fclose(fpout);
	free(line);
	return 0;
}

void handle_errors_macro_expansion(User_Output *out)
{
	if (out->type)
		printf("%s", out->message);
}

int expand_macros_memory_allocated(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, User_Output *out)
{
	char *li;
	int ln = 0;
	while (fgets(line, MAX_CHARS_IN_LINE, fpin) && (li = expand_macros_handle_label(sfname, dfname, fpin, fpout, line, ++ln, out)))
	{
		/* line was read, and li is not NULL */
		if (*li == ';' || *li == '\n')
			fputc('\n', fpout);
		else
		{

			fputc('\n', fpout); /* REMOVE THIS */
		}
	}
	if (out->type != SUCCESS)
		return 1;
	return 0;
}

char * expand_macros_handle_label(char *sfname, char *dfname, FILE *fpin, FILE *fpout, char *line, int ln, User_Output *out)
{
	char *end = strchr(line, ':'), tmp;
	char ln_str[MAX_LINES_IN_OUTPUT_FILE];
	int li = 0;
	while (*line == ' ' || *line == '\t')
		++line;
	if (!end)
	{
		if (*line != ';' && *line != '\n')
			fputc('\t', fpout);
		return line;
	}
	if (!isalpha(*line))
	{
		itoa_base10(ln, ln_str);
		out->type = ERROR_LABEL_NOT_BEGIN_WITH_ALPHA;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ": label begins with a non alphabetic character in line\n\t");	
		strcat(out->message, ln_str);
		strcat(out->message, line);
		return NULL;
	}
	while (line + li < end)
	{
		if (*(line + li) == ' ' || *(line + li) == '\t')
		{
			itoa_base10(ln, ln_str);
			out->type = ERROR_LABEL_MULTIPLE_WORDS_PRE_COLON;
			strcpy(out->message, ERROR_BASE_STRING);
			strcat(out->message, sfname);
			strcat(out->message, ": incorrect label format, multiple words found pre colon\n\t");	
			strcat(out->message, ln_str);
			strcat(out->message, line);
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
		*(n_str + i) = n % 10;
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
