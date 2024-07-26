#include "macro-expansion.h"
#include "after-macro.h"
#include "error-handling.h"

int main(int argc, char *argv[])
{
	int return_value;
	char *fname, after_macro_fname[MAX_FILENAME_LENGTH];
	Label *labels;
	User_Output *out = (User_Output *)malloc(sizeof(User_Output));
	if (!out)
	{
		printf("error initializing memory for the program\n");
		return 1;
	}
	while (argc-- > 1)
	{
		/* check file name size ?? */
		out->message_type = 0;
		printf("beginning macro expansion for input \"%s\"\n", argv[argc]);
		fname = handle_filename_extension(argv[argc], ".as", out);
		if (!fname)
		{
			print_errors(out);
			continue;
		}
		return_value = expand_macros(fname, after_macro_fname, out);
		if (fname != argv[argc])
			free(fname);
		if (!return_value)
		{
			print_errors(out);
			continue;
		}
		labels = allocate_label_array_memory(labels, out);
		return_value = begin_assembler(fname, after_macro_fname, labels, out);
		print_errors(out);
	}
	free(out);
	return 0;
}
