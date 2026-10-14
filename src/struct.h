/*
 * struct.h - Data structure definitions for the file index
 */
#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct FileNode {
	char *id;
	int score;
	char **words_list;
	int words_num;
	struct FileNode *next;
	struct FileNode *prev;
} FileNode, *TFileNode;

typedef struct {
	TFileNode first;
	TFileNode last;
} DoubleList, *DList;

/*
 * Node of the reference list stored in a trie node: points to a file that
 * contains the word
 */
typedef struct refNode {
	TFileNode file_node;
	struct refNode *next;
} refNode, *refList;

typedef struct trie_node {
	struct trie_node *next_arr[26];
	int is_word;
	refList word_files;
} trie_node, *PTrie_node;

/* A heap element: holds a reference to a file and its priority (score) */
typedef struct {
	TFileNode file_node;
	int priority;
} Item;

typedef struct heap {
	int maxHeapSize;
	int size;
	Item *elem;
} PriQueue, *APriQueue;

#endif
