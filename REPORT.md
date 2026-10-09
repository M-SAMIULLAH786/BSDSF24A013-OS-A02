
# Operating Systems Lab Assignment 02
## Implementation of Linux ls Command

**Student Name:** M. Sami Ullah  
**Repository:** BSDSF24A013-OS-A02

## Feature 1: Project Setup
Created the required project structure, source file, Makefile, and report. Compiled the program using GCC.

## Feature 2: Long Listing (-l)
Implemented long listing using `lstat()`, `getpwuid()`, and `getgrgid()`. Displayed file permissions, links, owner, group, size, modification time, and filename.

## Feature 3: Default Column Display
Implemented terminal-width-based column display using `ioctl()` and `TIOCGWINSZ`. Filenames are arranged vertically down columns before moving across.

## Feature 4: Horizontal Display (-x)
Implemented horizontal filename display using the `-x` option. Filenames are printed across each row until the terminal width is reached.

## Feature 5: Alphabetical Sorting
Used `qsort()` and a comparison function to sort filenames alphabetically, ignoring case.

## Feature 6: Colorized Output
Used ANSI escape sequences to display directories in blue, executable files in green, and symbolic links in cyan.

## Feature 7: Recursive Listing (-R)
Implemented recursive directory traversal using `opendir()`, `readdir()`, and `lstat()`. The program displays directory contents and continues into subdirectories.

## Testing
Compiled the program using `make` and tested:
- `./bin/ls`
- `./bin/ls -l`
- `./bin/ls -x`
- `./bin/ls -R`

All these commands ran successfully in the local WSL environment.

## Conclusion
This assignment provided practical experience with Linux directory handling, file metadata, dynamic memory allocation, sorting, terminal output, recursive traversal, Makefiles, and Git branching and version tagging.
