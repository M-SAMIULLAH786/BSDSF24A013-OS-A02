# \# Operating Systems Lab — Programming Assignment 02

# \## Re-implementing the Linux `ls` Command

# 

# \*\*Student Name:\*\* M. Sami Ullah  

# \*\*Repository:\*\* BSDSF24A013-OS-A02  

# \*\*Language:\*\* C  

# \*\*Build Tool:\*\* GNU Make and GCC  

# \*\*Environment:\*\* Ubuntu on WSL

# 

# \---

# 

# \## Feature 1: Project Setup and Initial Build

# 

# \### Implementation

# 

# I created a GitHub repository named `BSDSF24A013-OS-A02` and cloned it to my local machine. I added the starter source code, `Makefile`, `README.md`, and `REPORT.md`, along with the required project directories.

# 

# The `Makefile` automates compilation of the C source file into the executable `bin/ls`.

# 

# \### Questions and Answers

# 

# \*\*Q1. What is the purpose of a Makefile?\*\*

# 

# A Makefile automates the build process. It specifies the source files, compilation commands, targets, and dependencies. Instead of typing the complete GCC command every time, I can run `make` to compile the program.

# 

# \*\*Q2. Why is Git branching useful in this assignment?\*\*

# 

# Git branches allow each feature to be developed separately. They keep development organized and preserve the history of changes. Version tags such as `v1.1.0` and `v1.6.0` identify particular versions of the program.

# 

# \---

# 

# \## Feature 2: Version 1.1.0 — Complete Long Listing Format (`-l`)

# 

# \### Implementation

# 

# The `-l` option displays detailed information about each file. The implementation uses file metadata to display permissions, link count, owner, group, size, modification time, and filename.

# 

# The main functions and system interfaces used include:

# 

# \- `lstat()` — obtains file metadata without following symbolic links.

# \- `getpwuid()` — converts a user ID into a username.

# \- `getgrgid()` — converts a group ID into a group name.

# \- `ctime()` — converts a modification timestamp into a readable date and time.

# \- `print\_permissions()` — converts permission bits into an `rwx`-style string.

# 

# \### Question 1: What is the difference between `stat()` and `lstat()`? When is `lstat()` more appropriate for `ls`?

# 

# `stat()` follows a symbolic link and returns metadata about the file or directory to which the link points.

# 

# `lstat()` returns metadata about the symbolic link itself.

# 

# For an `ls` implementation, `lstat()` is useful when we need to identify a symbolic link as a link instead of displaying the metadata of its target. It allows the program to distinguish links from regular files and directories.

# 

# \### Question 2: How does `st\_mode` store file type and permissions? How can bitwise operators and macros extract this information?

# 

# The `st\_mode` field in `struct stat` contains information about both file type and permission bits.

# 

# Predefined macros make it easier to test this information:

# 

# \- `S\_ISDIR(mode)` checks whether the file is a directory.

# \- `S\_ISREG(mode)` checks whether it is a regular file.

# \- `S\_ISLNK(mode)` checks whether it is a symbolic link.

# \- `S\_IRUSR` represents the owner's read permission.

# \- `S\_IWUSR` represents the owner's write permission.

# \- `S\_IXUSR` represents the owner's execute permission.

# 

# The bitwise AND operator (`\&`) checks whether a particular permission bit is set. For example:

# 

# ```c

# if (info.st\_mode \& S\_IRUSR)

# &#x20;   printf("Owner can read the file\\n");

# ```

# 

# If the result is nonzero, the corresponding permission is set.

# 

# Special permission bits, such as setuid, setgid, and the sticky bit, require additional checks and special formatting if they are to be displayed in the permission string.

# 

# \---

# 

# \## Feature 3: Version 1.2.0 — Down-Then-Across Column Display

# 

# \### Implementation

# 

# The default display arranges filenames in multiple columns. The program first reads directory entries into a dynamically allocated array and tracks the longest filename.

# 

# It uses `ioctl()` with `TIOCGWINSZ` to obtain the terminal width. The maximum filename length plus spacing determines the width required for each column.

# 

# The number of columns is calculated from the terminal width and column width. The number of rows is then calculated from the number of filenames and columns.

# 

# \### Question 1: Explain the logic of down-then-across printing. Why is a single loop insufficient?

# 

# In down-then-across printing, filenames are filled vertically in the first column, then vertically in the second column, and so on.

# 

# A single loop that prints every filename in sequence produces a row-major order instead. To produce down-then-across order, the program needs to calculate the number of rows and use nested loops.

# 

# For example, if there are three rows, the first row prints entries at indexes `0`, `3`, `6`, and so on. The next row prints entries at indexes `1`, `4`, `7`, and so on.

# 

# The index formula is:

# 

# ```c

# index = column \* rows + row;

# ```

# 

# This allows the program to select the correct filename for each position in the output.

# 

# \### Question 2: What is the purpose of `ioctl()`? What are the limitations of using a fixed width of 80 columns?

# 

# `ioctl()` is a system call that performs device-specific operations. In this program, it is used with `TIOCGWINSZ` to retrieve the terminal's current dimensions, including its width in columns.

# 

# Using the actual terminal width allows the program to adapt its output when the terminal is resized.

# 

# A fixed width of 80 columns is less flexible. On a wider terminal, it may leave unused space. On a narrower terminal, filenames may wrap unexpectedly or the output may become difficult to read.

# 

# A fallback width is useful if terminal-size detection fails, but it cannot adapt as accurately as detecting the actual width.

# 

# \---

# 

# \## Feature 4: Version 1.3.0 — Horizontal Column Display (`-x`)

# 

# \### Implementation

# 

# The `-x` option enables horizontal, or row-major, display. Filenames are printed from left to right, and the program moves to the next line when the current line reaches the available terminal width.

# 

# The implementation uses `print\_horizontal()` for this mode and `print\_columns()` for the default down-then-across mode.

# 

# \### Question 1: Compare the complexity of down-then-across and horizontal printing. Which requires more pre-calculation and why?

# 

# Down-then-across printing requires more pre-calculation because the program needs to determine the number of columns, the number of rows, and the correct filename index for each position.

# 

# Horizontal printing is simpler. It can iterate through the filenames in sequence, track the current line position, and wrap to the next line when the next filename would exceed the terminal width.

# 

# Therefore, down-then-across printing generally requires more layout calculations.

# 

# \### Question 2: How are the different display modes (`-l`, `-x`, and default) managed?

# 

# The program uses `getopt()` to parse command-line options. A mode variable records which display mode has been selected.

# 

# \- `-l` selects long listing.

# \- `-x` selects horizontal listing.

# \- No display option selects the default down-then-across layout.

# 

# After reading the directory entries, the program checks the mode and calls the corresponding display function.

# 

# This separates option parsing from the code responsible for formatting the output.

# 

# \---

# 

# \## Feature 5: Version 1.4.0 — Alphabetical Sorting

# 

# \### Implementation

# 

# The program stores filenames in a dynamically allocated array and uses `qsort()` to sort them before displaying them.

# 

# A comparison function uses `strcasecmp()` to compare two filenames without distinguishing uppercase and lowercase letters.

# 

# Sorting the filenames before displaying them means the same sorted order can be used by the default, horizontal, and long-listing modes.

# 

# \### Question 1: Why must all directory entries be read into memory before sorting? What are the drawbacks for directories containing millions of files?

# 

# Sorting requires access to the collection of filenames being sorted. Therefore, the program reads the directory entries into an array before calling `qsort()`.

# 

# This allows the sorting algorithm to compare filenames in different positions and rearrange the array.

# 

# The disadvantage is memory consumption. A directory containing millions of entries may require a large amount of memory to store all filenames and their associated array elements. Allocating and sorting such a large collection may also take considerable time.

# 

# \### Question 2: Explain the purpose and signature of the `qsort()` comparison function. Why does it use `const void \*` arguments?

# 

# The comparison function tells `qsort()` how to compare two array elements.

# 

# Its general signature is:

# 

# ```c

# int compare(const void \*a, const void \*b);

# ```

# 

# The function returns:

# 

# \- A negative value if the first element should come before the second.

# \- Zero if the elements are considered equal.

# \- A positive value if the first element should come after the second.

# 

# The arguments are `const void \*` because `qsort()` is a generic sorting function that can sort arrays containing different data types. The comparison function converts these generic pointers to the appropriate type before comparing the values.

# 

# In this project, the elements are pointers to strings, so the function interprets the arguments as pointers to string pointers and compares the filenames.

# 

# \---

# 

# \## Feature 6: Version 1.5.0 — Colorized Output

# 

# \### Implementation

# 

# The program uses file metadata to determine how filenames should be displayed. ANSI escape sequences are printed before and after a filename to change the terminal's text color and then restore the default style.

# 

# The current implementation contains color-printing logic for directories, executable files, and symbolic links.

# 

# \### Question 1: How do ANSI escape codes produce color in a Linux terminal? Show the code sequence for green text.

# 

# ANSI escape codes are special character sequences interpreted by a compatible terminal. They change text attributes, such as foreground color, background color, and brightness.

# 

# For example, this sequence prints text in green and then resets the terminal style:

# 

# ```c

# printf("\\033\[1;32mHello\\033\[0m");

# ```

# 

# Here:

# 

# \- `\\033` represents the escape character.

# \- `\[1;32m` selects bold or bright green text.

# \- `Hello` is the text being displayed.

# \- `\\033\[0m` resets the terminal formatting.

# 

# The reset sequence is important because it prevents subsequent text from unintentionally retaining the same color.

# 

# \### Question 2: Which permission bits identify whether a file is executable by the owner, group, or others?

# 

# The execute permission bits in `st\_mode` are:

# 

# \- `S\_IXUSR` — execute permission for the owner.

# \- `S\_IXGRP` — execute permission for the group.

# \- `S\_IXOTH` — execute permission for others.

# 

# The program can test these bits using the bitwise AND operator:

# 

# ```c

# if (info.st\_mode \& S\_IXUSR)

# &#x20;   printf("Owner execute permission is set\\n");

# ```

# 

# If any of these bits is set, the corresponding user category has execute permission.

# 

# For a complete colorized `ls` implementation, file types should be checked in an appropriate order. For example, symbolic links should be identified before treating a file as an executable or directory. The assignment also specifies red for archive files such as `.tar`, `.gz`, and `.zip`, pink for symbolic links, and reverse video for special files.

# 

# \*\*Implementation note:\*\* The current source code should be checked against every required color category. The presence of ANSI codes alone does not prove that all required file types have been implemented.

# 

# \---

# 

# \## Feature 7: Version 1.6.0 — Recursive Listing (`-R`)

# 

# \### Implementation

# 

# The `-R` option lists a directory and then recursively lists its subdirectories.

# 

# The implementation uses directory traversal functions such as `opendir()` and `readdir()`. It uses `lstat()` to identify directories and constructs full paths before descending into subdirectories.

# 

# For example, running:

# 

# ```bash

# ./bin/ls -R testdir

# ```

# 

# lists the contents of `testdir` and then lists the contents of nested directories, such as `testdir/subdir`.

# 

# \### Question 1: What is a base case in recursion? What stops recursive `ls` from continuing forever?

# 

# A base case is a condition that stops a recursive function from making further recursive calls.

# 

# In recursive directory listing, recursion stops when the current directory has no more eligible subdirectories to visit. The program must also skip the special directory entries `.` and `..`, because they refer to the current directory and its parent.

# 

# Skipping these entries prevents the program from repeatedly traversing the same directories and recursing indefinitely.

# 

# \### Question 2: Why must the program construct a full path before making a recursive call? What would happen if it called `do\_ls("subdir")` from within `do\_ls("parent\_dir")`?

# 

# A full path identifies the location of a subdirectory relative to the current directory being listed.

# 

# For example, if the current directory is `parent\_dir` and the subdirectory is `subdir`, the full path is:

# 

# ```text

# parent\_dir/subdir

# ```

# 

# The recursive call should use this full path so the program opens the correct directory.

# 

# If it simply calls `do\_ls("subdir")`, the operating system interprets the path relative to the process's current working directory, not automatically relative to `parent\_dir`. The program might fail to find the directory or open a different directory with the same name.

# 

# Constructing the full path ensures that recursive traversal follows the correct directory hierarchy.

# 

# \---

# 

# \## Testing

# 

# The program was compiled using `make` in the WSL environment. The following commands were used during testing:

# 

# ```bash

# make

# ./bin/ls

# ./bin/ls -l

# ./bin/ls -x

# ./bin/ls -R testdir

# ```

# 

# The long-listing command displayed file permissions, link counts, owner and group names, sizes, modification times, and filenames.

# 

# The horizontal display command ran successfully, and the output differed from the default layout in the test directory. Alphabetical sorting was checked by examining the displayed filenames. Color-printing code was inspected, and colored directory names were visible in the terminal. Recursive listing was tested with a directory containing a nested subdirectory and files.

# 

# Testing confirms that these commands compiled and ran in the local environment. Exact output correctness and complete compliance with every required file type, permission bit, and terminal layout should be verified against the implementation before final submission.

# 

# \---

# 

# \## Git Workflow and Releases

# 

# The project uses separate feature branches and version tags for the incremental development of the `ls` utility.

# 

# The intended release versions are:

# 

# \- `v1.1.0` — Complete Long Listing Format

# \- `v1.2.0` — Column Display

# \- `v1.3.0` — Horizontal Column Display

# \- `v1.4.0` — Alphabetical Sorting

# \- `v1.5.0` — Colorized Output

# \- `v1.6.0` — Recursive Listing

# 

# Each GitHub release must use the corresponding version tag and include the compiled `ls` executable as an asset, in addition to GitHub's automatically generated source archives.

# 

# The final submission requires the completed feature branch to be merged into `main`, the updated `main` branch to be pushed to GitHub, and all required feature branches to be pushed.

# 

# \---

# 

# \## Conclusion

# 

# This assignment provided practical experience with Linux directory handling, file metadata, permission bits, dynamic memory allocation, sorting, terminal output formatting, ANSI escape sequences, recursive traversal, Makefiles, and Git version control.

# 

# The project demonstrates how a basic directory-listing program can be extended incrementally with additional options and more advanced output formatting. Completing the report and verifying every requirement helps ensure that the implementation is correct and that the underlying concepts can be explained during the viva-voce.



