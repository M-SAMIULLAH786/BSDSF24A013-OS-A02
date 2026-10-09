# Operating Systems Lab Assignment 02
## Linux `ls` Command Implementation in C

### Project Overview

This project implements a simplified version of the Linux `ls` command using the C programming language. It demonstrates Linux directory handling, file metadata, memory allocation, sorting, terminal formatting, colorized output, and recursive directory traversal.

### Features

| Version | Feature | Description |
|---|---|---|
| v1.0.0 | Initial Setup | Project structure, source code, and Makefile |
| v1.1.0 | Long Listing (`-l`) | Displays permissions, links, owner, group, size, modification time, and filename |
| v1.2.0 | Column Display | Displays filenames in columns, arranged down then across |
| v1.3.0 | Horizontal Display (`-x`) | Displays filenames across each row |
| v1.4.0 | Alphabetical Sorting | Sorts filenames alphabetically using `qsort()` |
| v1.5.0 | Colorized Output | Uses terminal colors for directories, executable files, and symbolic links |
| v1.6.0 | Recursive Listing (`-R`) | Lists directory contents and recursively visits subdirectories |

### Technologies Used

- **Language:** C
- **Compiler:** GCC
- **Operating Environment:** Linux / WSL
- **Build Tool:** GNU Make
- **Version Control:** Git and GitHub

### Project Structure

```text
BSDSF24A013-OS-A02/
├── src/
│   └── ls-v1.0.0.c
├── bin/
│   └── ls                 # Generated executable
├── obj/
├── man/
├── Makefile
├── README.md
├── REPORT.md
└── .gitignore
```

### Requirements

- GCC compiler
- GNU Make
- Linux or Windows Subsystem for Linux (WSL)
- Git

### Build Instructions

Clone the repository:

```bash
git clone https://github.com/M-SAMIULLAH786/BSDSF24A013-OS-A02.git
cd BSDSF24A013-OS-A02
```

Compile the program:

```bash
make
```

Clean the generated executable:

```bash
make clean
```

### Usage

Run the program in the current directory:

```bash
./bin/ls
```

Display detailed file information:

```bash
./bin/ls -l
```

Display filenames horizontally:

```bash
./bin/ls -x
```

Display directory contents recursively:

```bash
./bin/ls -R
```

List a specific directory:

```bash
./bin/ls /home
```

Options can be used individually. Combined options such as `-lR` are not supported by the current argument-handling implementation.

### Important System Calls and Library Functions

- `opendir()` — Opens a directory.
- `readdir()` — Reads directory entries.
- `closedir()` — Closes a directory.
- `lstat()` — Retrieves file metadata without following symbolic links.
- `getpwuid()` — Retrieves the file owner's username.
- `getgrgid()` — Retrieves the group name.
- `ioctl()` — Retrieves terminal width.
- `malloc()` and `realloc()` — Allocate and resize memory.
- `free()` — Releases allocated memory.
- `qsort()` — Sorts filenames alphabetically.
- `getopt()` — Processes command-line options.

### Version Control

The project uses separate Git branches for individual features and version tags for releases.

Feature branches:

- `feature-long-listing-v1.1.0`
- `feature-column-display-v1.2.0`
- `feature-horizontal-display-v1.3.0`
- `feature-alphabetical-sort-v1.4.0`
- `feature-colorized-output-v1.5.0`
- `feature-recursive-listing-v1.6.0`

Version tags range from `v1.1.0` to `v1.6.0`.

### Testing

The program was compiled with `make` and tested using the following commands:

```bash
./bin/ls
./bin/ls -l
./bin/ls -x
./bin/ls -R
```

### Conclusion

This assignment provides practical experience with C programming on Linux, directory operations, file metadata, dynamic memory management, sorting algorithms, terminal output, recursive traversal, Makefiles, and Git-based version control.

### Author

**Student:** M. Sami Ullah  
**Repository:** [BSDSF24A013-OS-A02](https://github.com/M-SAMIULLAH786/BSDSF24A013-OS-A02)
# BSDSF24A013-OS-A02
Assignment_2 Of Operating System
