/*
 * functions.c - Doubly linked list, trie and max-heap, plus the command handlers
 */
#include "functions.h"

/* Creates an empty list and allocates its memory */
DList create_list()
{
	DList new_list = NULL;

	new_list = malloc(sizeof(DoubleList));
	if (!new_list) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	new_list->first = NULL;
	new_list->last = NULL;

	return new_list;
}

/* Looks up a node by id; returns the node if found, NULL otherwise */
TFileNode list_find(DList list, char *id)
{
	if (!list || !list->first) {
		return NULL;
	}

	TFileNode temp = list->first;

	/* Walk the list node by node, comparing ids */
	while (temp) {
		if (!strcmp(temp->id, id)) {
			return temp;
		}

		temp = temp->next;
	}

	return NULL;
}

/* Appends a new node with the given id and score to the end of the list */
int list_insert(DList list, char *id, int score)
{
	TFileNode new_file = NULL;

	if (!list)
		return 0;

	/* Do not insert if the id already exists */
	if (list_find(list, id))
		return 0;

	new_file = malloc(sizeof(FileNode));
	if (!new_file) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	new_file->next = NULL;
	new_file->prev = NULL;
	new_file->id = malloc(strlen(id) + 1);
	strcpy(new_file->id, id);
	new_file->score = score;

	new_file->words_list = NULL;
	new_file->words_num = 0;

	if (!list->first) {
		/* Empty list: the new node becomes both first and last */
		list->first = new_file;
		list->last = new_file;

	} else {
		/* Link the new node after the current last element */
		new_file->prev = list->last;
		new_file->next = list->last->next;
		list->last->next = new_file;
		list->last = new_file;
	}

	return 1;
}

/* Removes the node with the given id from the list and frees its memory */
int list_remove(DList list, char *id)
{
	TFileNode temp = NULL;

	if (!list)
		return 0;

	if (!list_find(list, id))
		return 0;
	else
		temp = list_find(list, id);

	/* Free the id and all the keywords of the node */
	free(temp->id);
	for (int i = 0; i < temp->words_num; i++) {
		free(temp->words_list[i]);
	}
	free(temp->words_list);

	/* Four cases, depending on the node's position in the list: */
	if (!temp->next && !temp->prev) {
		list->last = NULL;
		list->first = NULL;
		free(temp);
	} else if (!temp->prev) {
		list->first = list->first->next;
		list->first->prev = NULL;
		free(temp);
	} else if (!temp->next) {
		list->last = list->last->prev;
		list->last->next = NULL;
		free(temp);
	} else {
		/*
		 * Link the neighbours to each other, skipping the node
		 * being removed
		 */
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		free(temp);
	}

	return 1;
}

/* Adds a new keyword to a file node's keyword list */
void file_add_keyword(TFileNode file_node, char *keyword)
{
	char **temp = NULL;

	/* Grow the keyword array by one and append the new keyword */
	temp = realloc(file_node->words_list, \
		(file_node->words_num + 1) * sizeof(char *));

	if (!temp) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	temp[file_node->words_num] = malloc(strlen(keyword) + 1);
	if (!temp[file_node->words_num]) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}
	strcpy(temp[file_node->words_num], keyword);
	file_node->words_num++;
	file_node->words_list = temp;
}

/*
 * Checks whether a file node contains a given keyword; returns 1 if so, 0
 * otherwise
 */
int file_has_keyword(TFileNode file_node, char *keyword)
{
	for (int i = 0; i < file_node->words_num; i++) {
		if (!strcmp(file_node->words_list[i], keyword))
			return 1;
	}

	return 0;
}

int file_remove_keyword(TFileNode file_node, char *keyword)
{
	if (!file_node || !file_node->words_num)
		return 0;

	char *temp_string = NULL;
	int new_len = 0;

	for (int i = 0; i < file_node->words_num; i++) {
		if (!strcmp(file_node->words_list[i], keyword)) {
			new_len = file_node->words_num - 1;
			file_node->words_num--;
			temp_string = file_node->words_list[i];
			free(temp_string);
			/*
			 * Move the last element into the removed slot to
			 * fill the gap
			 */
			file_node->words_list[i] = file_node->words_list[new_len];
			return 1;
		}
	}

	return 0;
}

/*
 * Frees the whole list: every node with its id and keywords, then the list
 * structure itself
 */
void list_free(DList list)
{
	if (!list || !list->first)
		return;

	TFileNode temp_iteration = NULL, temp_free = NULL;

	temp_iteration = list->first;
	temp_free = temp_iteration;

	/* Walk the list and free each node before advancing */
	while (temp_iteration) {
		temp_iteration = temp_iteration->next;

		free(temp_free->id);
		for (int i = 0; i < temp_free->words_num; i++)
			free(temp_free->words_list[i]);
		free(temp_free->words_list);
		free(temp_free);

		temp_free = temp_iteration;
	}

	free(list);
}

/* Creates a new trie node with all children NULL and no word marked */
PTrie_node trie_create_node()
{
	PTrie_node new_node = NULL;

	new_node = malloc(sizeof(trie_node));
	if (!new_node) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	new_node->is_word = 0;
	new_node->word_files = NULL;

	for (int i = 0; i < 26; i++)
		new_node->next_arr[i] = NULL;

	return new_node;
}

/*
 * Allocates a new node for the reference list (the files that contain a
 * word)
 */
refList alloc_node()
{
	refList new_list = NULL;

	new_list = malloc(sizeof(refNode));
	if (!new_list) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	new_list->next = NULL;
	new_list->file_node = NULL;

	return new_list;
}

/* Adds a file reference at the head of a trie node's file list */
void insert_list(PTrie_node trie, TFileNode file)
{
	refNode *temp = NULL;

	temp = alloc_node();
	temp->file_node = file;
	temp->next = trie->word_files;
	trie->word_files = temp;
}

/*
 * Inserts a word into the trie and associates the file with it. Walks the
 * word letter by letter, creating nodes where they are missing, then marks
 * the end of the word
 */
void trie_insert(PTrie_node trie, char *word, TFileNode file)
{
	int len = strlen(word);
	int index = 0;
	PTrie_node temp = NULL;

	temp = trie;
	for (int i = 0; i < len; i++) {
		/* Compute the letter's index (a=0, b=1, ..., z=25) */
		index = word[i] - 'a';

		if (!temp->next_arr[index])
			temp->next_arr[index] = trie_create_node();

		temp = temp->next_arr[index];
	}

	/* Mark the last node as the end of a word and add the file */
	temp->is_word = 1;
	insert_list(temp, file);
}

/*
 * Looks up a word in the trie and returns the list of associated files, or
 * NULL if the word does not exist
 */
refList trie_find(PTrie_node trie, char *word)
{
	int index = 0, len = strlen(word);
	PTrie_node temp = trie;

	for (int i = 0; i < len; i++) {
		index  = word[i] - 'a';

		if (temp->next_arr[index])
			temp = temp->next_arr[index];
		else
			return NULL; /* Letter missing from the trie: the word does not exist */
	}

	/* Make sure it is an actual stored word, not just a prefix */
	if (!temp->is_word)
		return NULL;

	return temp->word_files;
}

/* Removes a file's reference from a trie node's list */
void remove_from_list(PTrie_node trie_node, TFileNode file)
{
	refList prev = NULL;
	refList temp = trie_node->word_files;

	/*
	 * Walk the list until the file's reference is found, then unlink it
	 */
	while (temp) {
		if (temp->file_node == file) {
			if (!prev)
				trie_node->word_files = temp->next;
			else
				prev->next = temp->next;

			free(temp);
			return;
		}

		prev = temp;
		temp = temp->next;
	}
}

/*
 * Recursively removes a file's reference from a word in the trie. If a node
 * ends up with no word marker and no children after the removal, it returns
 * 1 so that its parent frees it
 */
int remove_recursive(PTrie_node trie_node, char *word, \
	int depth, TFileNode file)
{
	int index = word[depth] - 'a', len = strlen(word), temp = 0;
	if (!trie_node)
		return 0;

	if (depth == len) {
		/*
		 * End of the word reached: remove the file from the node's
		 * list
		 */
		remove_from_list(trie_node, file);

		/* Other files or children remain: keep the node */
		if (!trie_node->word_files)
			trie_node->is_word = 0;
		else
			return 0;

		for (int i = 0; i < 26; i++) {
			if (trie_node->next_arr[i])
				return 0;
		}

		return 1; /* The node can be deleted */
	}

	index = word[depth] - 'a';

	if (!trie_node->next_arr[index])
		return 0;

	temp = 0;

	/* Recurse deeper and check whether the child should be deleted */
	temp = remove_recursive(trie_node->next_arr[index],
									  word, depth + 1, file);
	if (temp) {
		/* The child reported that it can be deleted: free it */
		free(trie_node->next_arr[index]->word_files);
		free(trie_node->next_arr[index]);
		trie_node->next_arr[index] = NULL;
	}

	/* Check whether the current node can be deleted as well */
	if (trie_node->is_word)
		return 0;

	for (int i = 0; i < 26; i++) {
		if (trie_node->next_arr[i])
			return 0;
	}

	return 1;
}

/*
 * Starts the recursive removal of a file's reference from a word in the
 * trie
 */
void trie_remove_ref(PTrie_node trie, char *word, TFileNode file)
{
	remove_recursive(trie, word, 0, file);
}

/*
 * Prints every word in the trie in alphabetical order, with its files. The
 * word is built letter by letter in the `word` buffer while descending the
 * trie
 */
void trie_print(PTrie_node trie, char *word, int depth, FILE *file_output)
{
	refList temp = NULL;
	char **id = NULL, *aux = NULL;
	int count = 0;

	if (!trie)
		return;

	if (trie->is_word) {
		word[depth] = '\0';
		temp = trie->word_files;

		/*
		 * Count how many files are associated with the current word
		 */
		while (temp) {
			count++;
			temp = temp->next;
		}

		/*
		 * Collect the file ids into an array so they can be sorted
		 * alphabetically
		 */
		id = malloc(count * sizeof(char *));
		if (!id) {
			fprintf(stderr, "Memory allocation fail\n");
			exit(1);
		}

		temp = trie->word_files;

		for (int i = 0; i < count; i++) {
			id[i] = temp->file_node->id;
			temp = temp->next;
		}

		/* Sort the ids alphabetically (selection sort) */
		for (int i = 0; i < count - 1; i++)
			for (int j = i + 1; j < count; j++)
				if (strcmp(id[i], id[j]) > 0) {
					aux = id[i];
					id[i] = id[j];
					id[j] = aux;
				}

		fprintf(file_output, "%s %d", word, count);

		for (int i = 0; i < count; i++) {
			fprintf(file_output, " %s", id[i]);
		}

		fprintf(file_output, "\n");

		free(id);
	}

	/* Visit all children recursively in alphabetical order (a -> z) */
	for (int i = 0; i < 26; i++) {
		if (trie->next_arr[i]) {
			word[depth] = 'a' + i;
			trie_print(trie->next_arr[i], word, depth + 1, file_output);
		}
	}
}

/* Finds and returns the trie node that corresponds to the given prefix */
PTrie_node trie_find_node(PTrie_node trie, char *prefix)
{
	PTrie_node temp = NULL;
	temp = trie;
	int index = 0, len = strlen(prefix);

	if (!prefix)
		return NULL;

		/*
		 * Descend the trie letter by letter, following the prefix
		 */
	for (int i = 0; i < len; i++) {
		index = prefix[i] - 'a';

		/* The prefix is not in the trie */
		if (!temp->next_arr[index])
			return NULL;

		temp = temp->next_arr[index];
	}

	return temp;
}

/*
 * Recursively frees the whole trie: first the file list, then the children,
 * then the node itself
 */
void trie_free(PTrie_node trie)
{
	refList temp = NULL;

	if (!trie)
		return;

	/* Free the current node's reference list */
	while (trie->word_files) {
		temp = trie->word_files;
		trie->word_files = trie->word_files->next;
		free(temp);
	}

	/* Recursively free every child */
	for (int i = 0; i < 26; i++) {
		trie_free(trie->next_arr[i]);
	}

	free(trie);
}

/* Creates a max-heap with the given capacity */
APriQueue heap_create(int capacity)
{
	APriQueue new_heap = NULL;

	new_heap = malloc(sizeof(PriQueue));
	if (!new_heap) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	/* The element array is 1-indexed, so allocate capacity + 1 */
	new_heap->elem = malloc((capacity + 1) * sizeof(Item));
	if (!new_heap->elem) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}
	new_heap->maxHeapSize = capacity;
	new_heap->size = 0;
	return new_heap;
}

/*
 * Moves the element at position i up towards the root while it has a higher
 * priority than its parent. On equal priority, the file with the
 * alphabetically smaller id wins
 */
void sift_up(APriQueue heap, int i)
{
	Item temp;
	int parent = 0, higher = 0, equal = 0;

	/* The parent of node i is at position i / 2 */
	parent = i / 2;

	while (i > 1) {
		higher = heap->elem[i].priority > heap->elem[parent].priority;
		equal = heap->elem[i].priority == heap->elem[parent].priority &&\
		 strcmp(heap->elem[i].file_node->id, \
			heap->elem[parent].file_node->id) < 0;

		 /* The element is already in place */
		if (!higher && !equal)
			break;

		/* Swap the element with its parent and keep climbing */
		temp = heap->elem[i];
		heap->elem[i] = heap->elem[parent];
		heap->elem[parent] = temp;

		i = parent;
		parent = i / 2;
	}
}

/* Inserts a file into the heap, using its score as the priority */
void heap_insert(APriQueue heap, TFileNode file)
{
	if (heap->size == heap->maxHeapSize) {
		fprintf(stderr, "Heap full\n");
		return;
	}

	/*
	 * Append at the end and sift the element up to its correct position
	 */
	heap->size++;
	heap->elem[heap->size].file_node = file;
	heap->elem[heap->size].priority = file->score;

	sift_up(heap, heap->size);
}

/*
 * Moves the element at position i down towards the leaves while one of its
 * children has a higher priority. On equal priority, the file with the
 * alphabetically smaller id wins
 */
void sift_down(APriQueue heap, int i)
{
	int left = 0, right = 0, best = 0;
	Item temp;

	while (1) {
		/*
		 * Initialise the left and right children, assuming the root
		 * is the best
		 */
		left = 2 * i;
		right = 2 * i + 1;
		best = i;

		if (left <= heap->size && \
		   heap->elem[left].priority > heap->elem[best].priority)
			best = left;

		if (left <= heap->size && \
		   heap->elem[left].priority == heap->elem[best].priority && \
		   strcmp(heap->elem[left].file_node->id, \
			heap->elem[best].file_node->id) < 0)
			best = left;

		if (right <= heap->size && \
		   heap->elem[right].priority > heap->elem[best].priority)
			best = right;

		if (right <= heap->size && \
			heap->elem[right].priority == heap->elem[best].priority && \
		   strcmp(heap->elem[right].file_node->id, \
			heap->elem[best].file_node->id) < 0)
			best = right;

		if (best == i)
			break;

		/*
		 * Swap with the higher-priority child and keep descending
		 */
		temp = heap->elem[i];
		heap->elem[i] = heap->elem[best];
		heap->elem[best] = temp;

		i = best;
	}
}

/*
 * Removes and returns the element with the highest priority. Moves the last
 * element to position 1 and sifts it down to its place
 */
TFileNode heap_extract_max(APriQueue heap)
{
	TFileNode max = NULL;

	if (heap->size == 0)
		return NULL;

	max = heap->elem[1].file_node;

	/* Replace the root with the last element and restore the heap */
	heap->elem[1] = heap->elem[heap->size];
	heap->size--;

	if (heap->size > 0)
		sift_down(heap, 1);

	return max;
}

/* Frees the heap's memory */
void heap_free(APriQueue heap)
{
	if (!heap)
		return;

	free(heap->elem);
	free(heap);
	heap = NULL;
}

/*
 * ADD: adds a new file with the given score and keywords. Prints EXISTS if
 * the file already exists
 */
void handle_add(DList list, PTrie_node trie, FILE *file_output,
			char *id, int score, int num_words, char **words)
{
	TFileNode inserted_file = NULL;

	if (!list || list_find(list, id)) {
		fprintf(file_output, "EXISTS\n");
		return;
	}

	list_insert(list, id, score);
	inserted_file = list_find(list, id);

	/*
	 * Add each unique word to the trie and to the file's keyword list
	 */
	for (int i = 0; i < num_words; i++) {
		if (file_has_keyword(inserted_file, words[i]))
			continue;

		file_add_keyword(inserted_file, words[i]);
		trie_insert(trie, words[i], inserted_file);
	}

	fprintf(file_output, "OK\n");
}

/*
 * DEL: removes a file and all its references from the trie. Prints NOT
 * FOUND if the file does not exist
 */
void handle_del(DList list, PTrie_node trie, FILE *file_output, char *id)
{
	TFileNode file_to_delete = NULL;

	file_to_delete = list_find(list, id);
	if (!file_to_delete) {
		fprintf(file_output, "NOT FOUND\n");
		return;
	}

	/*
	 * Remove the file's reference from every one of its keywords in the
	 * trie
	 */
	for (int i = 0; i < file_to_delete->words_num; i++) {
		trie_remove_ref(trie, file_to_delete->words_list[i], file_to_delete);
	}

	list_remove(list, id);

	fprintf(file_output, "OK\n");
}

/* ADDKW: adds a new keyword to an existing file */
void handle_addkw(DList list, PTrie_node trie, FILE *file_output,
			   char *id, char *keyword)
{
	TFileNode file_reference = NULL;

	file_reference = list_find(list, id);
	if (!file_reference) {
		fprintf(file_output, "NOT FOUND\n");
		return;
	}

	/* Add the keyword only if the file does not have it yet */
	if (!file_has_keyword(file_reference, keyword)) {
		file_add_keyword(file_reference, keyword);
		trie_insert(trie, keyword, file_reference);
	}

	fprintf(file_output, "OK\n");
}

/* DELKW: removes a keyword from an existing file */
void handle_delkw(DList list, PTrie_node trie, FILE *file_output,
			   char *id, char *keyword)
{
	TFileNode file_reference = NULL;

	file_reference = list_find(list, id);
	if (!file_reference) {
		fprintf(file_output, "NOT FOUND\n");
		return;
	}

	if (file_has_keyword(file_reference, keyword)) {
		trie_remove_ref(trie, keyword, file_reference);
		file_remove_keyword(file_reference, keyword);
	}

	fprintf(file_output, "OK\n");

	/* If the file has no keywords left, remove it from the file list */
	if (!file_reference->words_num) {
		list_remove(list, file_reference->id);
	}

}

/*
 * FIND: prints how many files contain the given word, followed by their ids
 * in alphabetical order
 */
void handle_find(PTrie_node trie, FILE *file_output, char *keyword)
{
	refList list_file_with_word = NULL, temp = NULL;
	char **id_list = NULL, *aux = NULL;
	int count = 0;

	list_file_with_word = trie_find(trie, keyword);
	if (!list_file_with_word) {
		fprintf(file_output, "EMPTY\n");
		return;
	}

	/* Count how many files contain the word */
	temp = list_file_with_word;
	while (temp) {
		count++;
		temp = temp->next;
	}

	id_list = malloc(count * sizeof(char *));
	if (!id_list) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	/* Collect the ids and sort them alphabetically */
	temp = list_file_with_word;

	for (int i = 0; i < count; i++) {
		id_list[i] = temp->file_node->id;
		temp = temp->next;
	}

	for (int i = 0; i < count - 1; i++) {
		for (int j = i + 1; j < count; j++) {
			if (strcmp(id_list[i], id_list[j]) > 0) {
				aux = id_list[i];
				id_list[i] = id_list[j];
				id_list[j] = aux;
			}
		}
	}

	fprintf(file_output, "%d", count);

	for (int i = 0; i < count; i++)
		fprintf(file_output, " %s", id_list[i]);

	fprintf(file_output, "\n");
	free(id_list);
}

/*
 * TOPK: prints the num_files highest-scoring files that contain the given
 * word, using the heap for efficient extraction
 */
void handle_topk(PTrie_node trie, FILE *file_output,
	char *keyword, int num_files)
{
	APriQueue heap = NULL;
	refList list_file_with_word = NULL, temp = NULL;
	TFileNode file = NULL;
	int count = 0, limit = 0;

	list_file_with_word = trie_find(trie, keyword);
	if (!list_file_with_word) {
		fprintf(file_output, "EMPTY\n");
		return;
	}

	/* Count the available files to size the heap */
	temp = list_file_with_word;
	while (temp) {
		count++;
		temp = temp->next;
	}

	/*
	 * Insert all files into the heap and extract the first `limit` in
	 * descending order
	 */
	heap = heap_create(count);
	temp = list_file_with_word;
	while (temp) {
		heap_insert(heap, temp->file_node);
		temp = temp->next;
	}

	/* Never extract more than exist */
	limit = (num_files < count) ? num_files : count;

	fprintf(file_output, "%d", limit);
	for (int i = 0; i < limit; i++) {
		file = heap_extract_max(heap);
		fprintf(file_output, " %s", file->id);
	}

	fprintf(file_output, "\n");
	heap_free(heap);
}

/*
 * PRINT: prints every word in the trie with its files, or EMPTY if the trie
 * is empty
 */
void handle_print(PTrie_node trie, FILE *file_output)
{
	char buffer[100];
	int non_empty = 0;

	/*
	 * Check whether the trie has at least one child (i.e. it is not
	 * empty)
	 */
	for (int i = 0; i < 26; i++) {
		if (trie->next_arr[i]) {
			non_empty = 1;
			break;
		}
	}

	if (!non_empty) {
		fprintf(file_output, "EMPTY\n");
		return;
	}

	trie_print(trie, buffer, 0, file_output);
}

/*
 * Recursively collects all unique ids in the given subtree. The array grows
 * on demand
 */
void id_prefix(PTrie_node node, char ***ids, int *count, int *capacity)
{
	refList temp = NULL;
	char **grown = NULL;
	int found = 0;

	if (!node)
		return;

	if (node->is_word) {
		temp = node->word_files;

		while (temp) {
			/* Make sure no duplicates are added */
			found = 0;
			for (int i = 0; i < *count; i++) {
				if (!strcmp((*ids)[i], temp->file_node->id)) {
					found = 1;
					break;
				}
			}

			if (!found) {
				if (*count == *capacity) {
					*capacity = *capacity ? *capacity * 2 : 8;
					grown = realloc(*ids, *capacity * sizeof(char *));
					if (!grown) {
						fprintf(stderr, "Memory allocation fail\n");
						exit(1);
					}
					*ids = grown;
				}
				(*ids)[(*count)++] = temp->file_node->id;
			}

			temp = temp->next;
		}
	}

	for (int i = 0; i < 26; i++) {
		id_prefix(node->next_arr[i], ids, count, capacity);
	}
}

/*
 * PREFIX: prints all files that have at least one keyword starting with the
 * given prefix, sorted alphabetically by id
 */
void handle_prefix(PTrie_node trie, FILE *file_output, char *prefix)
{
	PTrie_node node = NULL;
	char **ids = NULL, *aux = NULL;
	int count = 0, capacity = 0, i = 0, j = 0;

	node = trie_find_node(trie, prefix);
	if (!node) {
		fprintf(file_output, "EMPTY\n");
		return;
	}

	id_prefix(node, &ids, &count, &capacity);

	if (count == 0) {
		fprintf(file_output, "EMPTY\n");
		free(ids);
		return;
	}

	/* Sort alphabetically */
	for (i = 0; i < count - 1; i++) {
		for (j = i + 1; j < count; j++)
			if (strcmp(ids[i], ids[j]) > 0) {
				aux = ids[i];
				ids[i] = ids[j];
				ids[j] = aux;
			}
		}

	fprintf(file_output, "%d", count);

	for (i = 0; i < count; i++) {
		fprintf(file_output, " %s", ids[i]);
	}

	fprintf(file_output, "\n");

	free(ids);
}

void parse_add(char *buffer, DList list, PTrie_node trie, FILE *file_output)
{
	char **words = NULL, *rest  = NULL, id[100], temp[100];
	int num_words = 0, score = 0;

	/* Read what follows ADD */
	sscanf(buffer + 4, "%s %d %d", id, &score, &num_words);

	words = malloc(num_words * sizeof(char *));
	if (!words) {
		fprintf(stderr, "Memory allocation fail\n");
		exit(1);
	}

	/* Skip the id and the ADD token */
	rest = buffer + strlen(id) + 4;

	/* Skip the score */
	while (*rest == ' ')
		rest++;

	while (*rest && *rest != ' ')
		rest++;

	/* Skip num_words */
	while (*rest == ' ')
		rest++;
	while (*rest && *rest != ' ')
		rest++;

	/* rest now points at the first keyword */
	while (*rest == ' ')
		rest++;

	/* Read all keywords into the `words` array of strings */
	for (int i = 0; i < num_words; i++) {
		sscanf(rest, "%s", temp);

		words[i] = malloc(strlen(temp) + 1);
		if (!words[i]) {
			fprintf(stderr, "Memory allocation fail\n");
			exit(1);
		}

		strcpy(words[i], temp);

		/* Skip the keyword */
		rest += strlen(temp);

		while (*rest == ' ')
			rest++;
	}

	handle_add(list, trie, file_output, id, score, num_words, words);

	for (int j = 0; j < num_words; j++) {
		free(words[j]);
	}

	free(words);
}

/* Reads and executes every instruction from the input file */
void do_instrs(int num_instrs, FILE *file_input, FILE *file_output,
			   DList list, PTrie_node trie)
{
	char buffer[1100];
	char command[20], id[100], keyword[100], prefix[100];
	int num_files = 0;

	for (int i = 0; i < num_instrs; i++) {
		fgets(buffer, sizeof(buffer), file_input);
		/* Strip the trailing newline */
		buffer[strcspn(buffer, "\n")] = '\0';

		sscanf(buffer, "%s", command);
		if (!command[0])
			continue;

		if (strcmp(command, "ADD") == 0) {
			parse_add(buffer, list, trie, file_output);

		} else if (strcmp(command, "DEL") == 0) {
			sscanf(buffer, "%*s %s", id);
			handle_del(list, trie, file_output, id);

		} else if (strcmp(command, "ADDKW") == 0) {
			sscanf(buffer + 6, "%s %s", id, keyword);
			handle_addkw(list, trie, file_output, id, keyword);

		} else if (strcmp(command, "DELKW") == 0) {
			sscanf(buffer + 6, "%s %s", id, keyword);
			handle_delkw(list, trie, file_output, id, keyword);

		} else if (strcmp(command, "FIND") == 0) {
			sscanf(buffer + 5, "%s", keyword);
			handle_find(trie, file_output, keyword);

		} else if (strcmp(command, "TOPK") == 0) {
			sscanf(buffer + 5, "%s %d", keyword, &num_files);
			handle_topk(trie, file_output, keyword, num_files);

		} else if (strcmp(command, "PRINT") == 0) {
			handle_print(trie, file_output);

		} else if (strcmp(command, "PREFIX") == 0) {
			sscanf(buffer + 7, "%s", prefix);
			handle_prefix(trie, file_output, prefix);
		}
	}
}
