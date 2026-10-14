/*
 * main.c - Entry point of the keyword file indexing system
 *
 * Usage: keyword_index <input_file> [output_file]
 * Results go to stdout when no output file is given.
 */

#include "functions.h"

int main(int argc, char *argv[])
{
	FILE *file_input = NULL;
	FILE *file_output = stdout;
	DList list = NULL;
	PTrie_node trie = NULL;
	int num_instrs = 0;

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s <input_file> [output_file]\n", argv[0]);
		return 1;
	}

	file_input = fopen(argv[1], "r");
	if (!file_input) {
		perror(argv[1]);
		return 1;
	}

	if (argc == 3) {
		file_output = fopen(argv[2], "w");
		if (!file_output) {
			perror(argv[2]);
			fclose(file_input);
			return 1;
		}
	}

	list = create_list();
	trie = trie_create_node();

	if (fscanf(file_input, "%d\n", &num_instrs) != 1)
		num_instrs = 0;

	do_instrs(num_instrs, file_input, file_output, list, trie);

	list_free(list);
	trie_free(trie);
	fclose(file_input);
	if (file_output != stdout)
		fclose(file_output);
	return 0;
}
