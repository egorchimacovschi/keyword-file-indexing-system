/*
 * functions.h - Function declarations for the file index
 */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"

DList create_list();
TFileNode list_find(DList list, char *id);
int list_insert(DList list, char *id, int score);
int list_remove(DList list, char *id);
void file_add_keyword(TFileNode file_node, char *keyword);
int file_has_keyword(TFileNode file_node, char *keyword);
int file_remove_keyword(TFileNode file_node, char *keyword);
void list_free(DList list);

PTrie_node trie_create_node();
refList alloc_node();
void insert_list(PTrie_node trie, TFileNode file);
void trie_insert(PTrie_node trie, char *word, TFileNode file);
refList trie_find(PTrie_node trie, char *word);
void remove_from_list(PTrie_node node, TFileNode file);
int remove_recursive(PTrie_node node, char *word,
	int depth, TFileNode file);
void trie_remove_ref(PTrie_node trie, char *word, TFileNode file);
void trie_print(PTrie_node trie, char *word, int depth, FILE *file_output);
PTrie_node trie_find_node(PTrie_node trie, char *prefix);
void trie_free(PTrie_node trie);

APriQueue heap_create(int capacity);
void heap_insert(APriQueue heap, TFileNode file);
void sift_up(APriQueue heap, int i);
TFileNode heap_extract_max(APriQueue heap);
void heap_free(APriQueue heap);

void handle_add(DList list, PTrie_node trie, FILE *file_output,
	char *id, int score, int num_words, char **words);
void handle_del(DList list, PTrie_node trie,
	FILE *file_output, char *id);
void handle_addkw(DList list, PTrie_node trie, FILE *file_output,
			char *id, char *keyword);
void handle_delkw(DList list, PTrie_node trie, FILE *file_output,
			char *id, char *keyword);
void handle_find(PTrie_node trie, FILE *file_output, char *keyword);
void handle_topk(PTrie_node trie, FILE *file_output,
	char *keyword, int num_files);
void id_prefix(PTrie_node node, char ***ids, int *count, int *capacity);
void handle_prefix(PTrie_node trie, FILE *file_output, char *prefix);
void handle_print(PTrie_node trie, FILE *file_output);
void parse_add(char *buffer, DList list, PTrie_node trie, FILE *file_output);
void do_instrs(int num_instrs, FILE *file_input, FILE *file_output,
	DList list, PTrie_node trie);

#endif
