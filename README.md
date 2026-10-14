# Keyword File Indexing System

A **keyword-based file indexing system** written in C. Files are registered together with a score and a set of keywords, and can then be found by an exact word or by a prefix, listed alphabetically, or ranked with a **TOP-K** query.

It is built from scratch on a doubly linked list, a trie and a max-heap, with no external libraries.

**Tech:** C · GCC · Make

## ✨ Features

- Add and remove files, each with a score and a list of keywords
- Add or remove individual keywords on an existing file
- **Word search**: every file that contains a given keyword
- **Prefix search**: every file that has a keyword starting with a given prefix
- **TOP-K ranking**: the K highest-scoring files for a keyword
- **Alphabetical listing** of all indexed words with their files

## 📟 Commands

| Command | What it does | Output |
| --- | --- | --- |
| `ADD <id> <score> <n> <kw1> … <kwN>` | Adds a file with a score and `n` keywords | `OK`, or `EXISTS` if the id is taken |
| `DEL <id>` | Removes a file and all its keyword references | `OK`, or `NOT FOUND` |
| `ADDKW <id> <keyword>` | Adds a keyword to an existing file | `OK`, or `NOT FOUND` |
| `DELKW <id> <keyword>` | Removes a keyword from an existing file | `OK`, or `NOT FOUND` |
| `FIND <keyword>` | Lists the files that contain the keyword | `<count> <id> …` sorted by id, or `EMPTY` |
| `PREFIX <prefix>` | Lists the files with a keyword starting with the prefix | `<count> <id> …` sorted by id, or `EMPTY` |
| `TOPK <keyword> <k>` | Lists the `k` highest-scoring files for the keyword | `<count> <id> …` by score, highest first, or `EMPTY` |
| `PRINT` | Lists every indexed word with its files, in alphabetical order | `<word> <count> <id> …` per line, or `EMPTY` |

Notes on behaviour:

- Duplicate keywords in an `ADD` are stored once.
- In `TOPK`, files with the same score are ordered alphabetically by id.
- If `DELKW` removes a file's last keyword, the file is removed from the index.

## 🧱 Data structures

| Structure | Used for |
| --- | --- |
| Doubly linked list | The indexed files (id, score, keyword list) |
| Trie (26 children per node) | Storing keywords for fast exact and prefix lookup |
| Reference list inside each trie node | Links a word to every file that contains it |
| Max-heap (priority queue) | Ranking files by score for `TOPK` |

A lookup walks the trie one letter at a time, so its cost depends on the length of the word, not on how many files are indexed. A prefix query finds the trie node for the prefix and collects the files of the whole subtree below it.

## 🚀 Getting started

### Requirements

- `gcc`
- `make`

### Build and run

```bash
git clone https://github.com/egorchimacovschi/keyword-file-indexing-system.git
cd keyword-file-indexing-system

make build                               # compiles to ./keyword_index
make run                                 # runs examples/sample.in

./keyword_index <input_file> [output_file]
```

If no output file is given, results are printed to the terminal.

### Run the tests

```bash
make test     # runs every case in tests/input and compares with tests/expected
make asan    
make clean
```

## 📥 Input format

```
<number of commands>
<command>                    (one line per command)
```

### Example

`examples/sample.in`

```
14
ADD IronMan 97 3 avenger genius billionaire
ADD CaptainAmerica 91 2 avenger supersoldier
ADD Thor 89 3 avenger asgardian thunder
ADD Loki 88 2 villain asgardian
FIND avenger
TOPK avenger 2
PREFIX as
DELKW Thor thunder
ADDKW IronMan sorcerer
DEL Loki
FIND asgardian
PREFIX so
PRINT
ADD IronMan 10 1 duplicate
```

Output

```
OK
OK
OK
OK
3 CaptainAmerica IronMan Thor
2 IronMan CaptainAmerica
2 Loki Thor
OK
OK
OK
1 Thor
1 IronMan
asgardian 1 Thor
avenger 3 CaptainAmerica IronMan Thor
billionaire 1 IronMan
genius 1 IronMan
sorcerer 1 IronMan
supersoldier 1 CaptainAmerica
EXISTS
```

## 📁 Project structure

```
keyword-file-indexing-system/
├── src/
│   ├── main.c            # entry point: argument handling and program flow
│   ├── functions.c       # list, trie, heap and command handlers
│   ├── functions.h       # function declarations
│   └── struct.h          # data structure definitions
├── tests/
│   ├── input/            # test cases
│   ├── expected/         # expected output of each test case
│   └── run_tests.sh      # compares actual and expected output
├── examples/
│   └── sample.in
└── Makefile
```

## 🧩 Implementation notes

- **Recursive trie deletion.** Removing a file from a word (`remove_recursive`) was the hardest part. After the file's reference is removed, a node may be left with no word marker and no children. The recursion reports this to the parent, which frees the node, so the trie never keeps dead branches.
- **Reference lists.** Trie nodes don't copy file data: each word points back to the file nodes in the main list, so a file's score and keywords live in exactly one place.
- **Heap ordering.** The heap is 1-indexed and compares by score first, then by id, so `TOPK` results are deterministic.

## ⚠️ Limitations

- Keywords must contain only lowercase letters `a`–`z`, because the trie has 26 children per node. File ids are case-sensitive.
- File ids and keywords can be up to 99 characters long, and a single command line up to about 1,000 characters.

## ✍️ Author

Written by [Egor Chimacovschi](https://github.com/egorchimacovschi).
