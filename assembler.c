#include "macro-expansion.h"
#include "after-macro.h"
#include "error-handling.h"

int main(int argc, char *argv[])
{
	int return_value, i, error_return;
	char *fname, after_macro_fname[MAX_FILENAME_LENGTH];
	Label *labels = NULL;
	User_Output *out = NULL;
	while (argc-- > 1)
	{
		error_return = 0;
		/* check file name size ?? */
		if (strlen(argv[argc]) > MAX_FILENAME_LENGTH)
		{
			printf("error for input \"%s\":\n\texceeding the maximum file length of %d for an input file\n", argv[argc], MAX_FILENAME_LENGTH);
			continue;
		}
		printf("beginning macro expansion for input \"%s\"\n", argv[argc]);
		out = init_output_array_memory();
		if (!out)
		{
			printf("error initializing memory for the program for input \"%s\"\n", argv[argc]);
			continue;
		}
		fname = handle_filename_extension(argv[argc], ".as", &out, &error_return); /* &out ?? */
		if (error_return)
		{
			print_errors(out);
			free(out);
			continue;
		}	
		expand_macros(fname, after_macro_fname, &out, &error_return);
		free(fname);
		if (error_return) 
		{
			print_errors(out);
			free(out);
			continue;
		}
		labels = init_label_array_memory();
		if (!labels)
		{
			printf("error initializing memory for the program for input \"%s\"\n", argv[argc]);
			free(out);
			continue;
		}
		printf("xd2\n");
		return_value = begin_assembler(fname, after_macro_fname, &labels, &out);
		if (return_value) 
		{
			print_errors(out);
			free(out);
			free(labels);
			continue;
		} 
		print_errors(out);
		i = 0;
		while (labels[i].decimal_instruction_address)
			printf("label name: %s\n", labels[i++].name);
		free(out);
		free(labels);
	}
	return 0;
}
