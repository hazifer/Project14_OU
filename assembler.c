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
		/* check file name size ?? */
		if (strlen(argv[argc]) > MAX_FILENAME_LENGTH)
		{
			printf("error for input \"%s\":\n\texceeding the maximum file length of %d for an input file\n", argv[argc], MAX_FILENAME_LENGTH);
			continue;
		}
		printf("beginning macro expansion for input \"%s\"\n", argv[argc]);
		out = allocate_output_array_memory(out, &error_return);
		if (error_return)
		{
			/* print depends on error_return? */
			printf("error initializing memory for the program for input \"%s\"\n", argv[argc]);
			continue;
		}
		fname = handle_filename_extension(argv[argc], ".as", &out, &error_return); /* &out ?? */
		if (error_return)
		{
			print_errors(out);
			continue;
		}
		
		log_error(&out, "test3", "test", ERROR_PROGRAM_MEMORY_ALLOCATION, 0, &error_return);
		log_error(&out, "test2", "test233", ERROR_PROGRAM_MEMORY_ALLOCATION, 0, &error_return);
		printf("WHAT IS THIS\n");
		return_value = expand_macros(fname, after_macro_fname, &out, &error_return);
		print_errors(out);
		/*
		if (fname != argv[argc])
			free(fname);
		if (return_value) 
		{
			print_errors(out);
			continue;
		}
		labels = allocate_label_array_memory(labels, out);
		if (!labels)
		{
			printf("error initializing memory for the program for input \"%s\"\n", argv[argc]);
			continue;
		}
		return_value = begin_assembler(fname, after_macro_fname, labels, &out);
		if (return_value) 
		{
			print_errors(out);
			continue;
		}*/
		/* ?print_errors(out); */
		/* clear out, labels = FREE? */
		/* temp print labels
		i = 0;
		while (labels[i].decimal_instruction_address)
			printf("label name: %s\n", labels[i++].name); */
	}
	return 0;
}
