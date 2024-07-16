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
			strcat(out->message, ", couldn't allocate enough memory for the program.\n");
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
		strcat(out->message, ", couldn't access file for reading.\n");
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
	char *dfname, *tmp;
	dfname = (char *)malloc(strlen(sfname) * sizeof(char));
	if (!dfname)
	{
		out->type = ERROR_DESTINATION_FILE_MEMORY_ALLOCATION;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ", couldn't create expanded macros file.\n");
		return 1;
	}
	strcpy(dfname, sfname);
	tmp = strstr(dfname, ".as");
	tmp[2] = 'm';
	fpin = fopen(sfname, "r");
	if (!fpin)
	{
		out->type = ERROR_SOURCE_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, sfname);
		strcat(out->message, ", couldn't access file for reading.\n");
		return 1;
	}
	fpout = fopen(dfname, "w");
	if (!fpout)
	{
		out->type = ERROR_DESTINATION_FILE_ACCESS;
		strcpy(out->message, ERROR_BASE_STRING);
		strcat(out->message, dfname);
		strcat(out->message, ", couldn't access file for reading.\n");
		return 1;
	}
	return 0;
}

void handle_errors_macro_expansion(User_Output *out)
{
	if (out->type)
		printf("%s", out->message);
}
