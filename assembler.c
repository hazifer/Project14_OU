#include "macro-expansion.h"

int main(int argc, char *argv[])
{
	char *fname;
	User_Output *out = (User_Output *)malloc(sizeof(User_Output));
	if (!out)
	{
		printf("error allocating memory for the program\n");
		return 1;
	}
	while (argc-- > 1)
	{
		out->type = 0;
		printf("beginning macro expansion for input \"%s\"\n", argv[argc]);
		fname = handle_filename_extension(argv[argc], ".as", out);
		if (fname)
		{
			expand_macros(fname, out);
			if (fname != argv[argc])
				free(fname);
			if (out->type == SUCCESS)
			{
				/* continue program for this file */
			}
		}
		handle_errors_macro_expansion(out);
	}
	free (out);
	return 0;
}
