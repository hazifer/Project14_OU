#include "macro-expansion.h"
#include "after-macro.h"
#include "error-handling.h"

int main(int argc, char *argv[])
{
	int error_return;
	char *fname, after_macro_fname[MAX_FILENAME_LENGTH];
	Label *labels = NULL;
	Word *words = NULL;
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
		fname = handle_filename_extension(argv[argc], ".as", &error_return); /* &out ?? */
		if (error_return)
			continue;
		expand_macros(fname, after_macro_fname, &error_return);
		free(fname);
		if (error_return)
		{
			continue;
		}
		labels = init_label_array_memory();
		if (!labels)
		{
			printf("error initializing memory for the program for input \"%s\"\n", argv[argc]);
			continue;
		}
		words = init_word_array_memory();
		if (!words)
		{
			printf("error initializing memory for the program for input \"%s\"\n", argv[argc]);
			free(labels);
			continue;
		}
		begin_assembler(fname, after_macro_fname, &words, &labels);
		print_labels(labels);
		printf("\nwords:\n");
		print_words(words);
		if (labels)
			free(labels);
		if (words)
			free(words);
	}
	return 0;
}
