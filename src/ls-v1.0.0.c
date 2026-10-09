
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <errno.h>

#define BLUE  "\033[1;34m"
#define GREEN "\033[1;32m"
#define CYAN  "\033[1;36m"
#define RESET "\033[0m"

int recursive_mode = 0;

/* Compare filenames alphabetically */
int compare_names(const void *a, const void *b)
{
    const char *name1 = *(const char **)a;
    const char *name2 = *(const char **)b;
    return strcasecmp(name1, name2);
}

/* Print a filename with color */
void print_name(const char *path, const char *name)
{
    struct stat info;

    if (lstat(path, &info) == -1) {
        printf("%s", name);
        return;
    }

    if (S_ISDIR(info.st_mode))
        printf(BLUE "%s" RESET, name);
    else if (S_ISLNK(info.st_mode))
        printf(CYAN "%s" RESET, name);
    else if (info.st_mode & S_IXUSR)
        printf(GREEN "%s" RESET, name);
    else
        printf("%s", name);
}

/* Print file permissions */
void print_permissions(mode_t mode)
{
    printf("%c", S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' : '-');
    printf("%c", mode & S_IRUSR ? 'r' : '-');
    printf("%c", mode & S_IWUSR ? 'w' : '-');
    printf("%c", mode & S_IXUSR ? 'x' : '-');
    printf("%c", mode & S_IRGRP ? 'r' : '-');
    printf("%c", mode & S_IWGRP ? 'w' : '-');
    printf("%c", mode & S_IXGRP ? 'x' : '-');
    printf("%c", mode & S_IROTH ? 'r' : '-');
    printf("%c", mode & S_IWOTH ? 'w' : '-');
    printf("%c", mode & S_IXOTH ? 'x' : '-');
}

/* Print long listing */
void print_long(const char *dir, const char *name)
{
    char path[PATH_MAX];
    struct stat info;

    if (snprintf(path, sizeof(path), "%s/%s", dir, name)
        >= (int)sizeof(path)) {
        fprintf(stderr, "Path too long: %s/%s\n", dir, name);
        return;
    }

    if (lstat(path, &info) == -1) {
        perror(path);
        return;
    }

    struct passwd *user = getpwuid(info.st_uid);
    struct group *group = getgrgid(info.st_gid);
    char *date = ctime(&info.st_mtime);

    print_permissions(info.st_mode);

    printf(" %lu %-8s %-8s %8ld ",
           (unsigned long)info.st_nlink,
           user ? user->pw_name : "?",
           group ? group->gr_name : "?",
           (long)info.st_size);

    if (date) {
        date[strlen(date) - 1] = '\0';
        printf("%s ", date);
    }

    print_name(path, name);
    printf("\n");
}

/* Get terminal width */
int get_terminal_width(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
        ws.ws_col > 0)
        return ws.ws_col;

    return 80;
}

/* Default display: down then across */
void print_columns(char **names, int count, int max_len,
                   const char *dir)
{
    int width = get_terminal_width();
    int col_width = max_len + 2;
    int columns = width / col_width;

    if (columns < 1)
        columns = 1;
    if (columns > count)
        columns = count;

    int rows = (count + columns - 1) / columns;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            int index = col * rows + row;

            if (index < count) {
                char path[PATH_MAX];

                if (snprintf(path, sizeof(path), "%s/%s",
                             dir, names[index]) >= (int)sizeof(path)) {
                    printf("%s", names[index]);
                } else {
                    print_name(path, names[index]);
                }

                int padding = col_width - (int)strlen(names[index]);
                for (int p = 0; p < padding; p++)
                    putchar(' ');
            }
        }
        putchar('\n');
    }
}

/* Horizontal display: -x */
void print_horizontal(char **names, int count, int max_len,
                      const char *dir)
{
    int width = get_terminal_width();
    int col_width = max_len + 2;
    int position = 0;

    for (int i = 0; i < count; i++) {
        if (position > 0 && position + col_width > width) {
            putchar('\n');
            position = 0;
        }

        char path[PATH_MAX];

        if (snprintf(path, sizeof(path), "%s/%s",
                     dir, names[i]) >= (int)sizeof(path)) {
            printf("%s", names[i]);
        } else {
            print_name(path, names[i]);
        }

        int padding = col_width - (int)strlen(names[i]);
        for (int p = 0; p < padding; p++)
            putchar(' ');

        position += col_width;
    }

    if (count > 0)
        putchar('\n');
}

/* List a directory; recurse into its subdirectories when -R is used */
void do_ls(const char *dir, int mode)
{
    DIR *dp = opendir(dir);

    if (dp == NULL) {
        perror(dir);
        return;
    }

    int capacity = 10;
    int count = 0;
    int max_len = 0;

    char **names = malloc(capacity * sizeof(char *));

    if (names == NULL) {
        perror("malloc");
        closedir(dp);
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0 ||
            entry->d_name[0] == '.')
            continue;

        if (count == capacity) {
            if (capacity > 2147483647 / 2) {
                fprintf(stderr, "Too many entries\n");
                break;
            }

            int new_capacity = capacity * 2;
            char **temp = realloc(names,
                                  new_capacity * sizeof(char *));

            if (temp == NULL) {
                perror("realloc");
                break;
            }

            names = temp;
            capacity = new_capacity;
        }

        names[count] = strdup(entry->d_name);

        if (names[count] == NULL) {
            perror("strdup");
            break;
        }

        int len = (int)strlen(names[count]);
        if (len > max_len)
            max_len = len;

        count++;
    }

    closedir(dp);

    /* Alphabetical sorting */
    qsort(names, count, sizeof(char *), compare_names);

    if (recursive_mode) {
        printf("%s:\n", dir);
    }

    if (mode == 1) {
        for (int i = 0; i < count; i++)
            print_long(dir, names[i]);
    } else if (mode == 2) {
        print_horizontal(names, count, max_len, dir);
    } else if (count > 0) {
        print_columns(names, count, max_len, dir);
    }

    if (recursive_mode) {
        for (int i = 0; i < count; i++) {
            char path[PATH_MAX];
            struct stat info;

            if (snprintf(path, sizeof(path), "%s/%s",
                         dir, names[i]) >= (int)sizeof(path))
                continue;

            /* lstat avoids following directory symlinks */
            if (lstat(path, &info) == 0 && S_ISDIR(info.st_mode)) {
                putchar('\n');
                do_ls(path, mode);
            }
        }
    }

    for (int i = 0; i < count; i++)
        free(names[i]);

    free(names);
}

/* Main function */
int main(int argc, char *argv[])
{
    int mode = 0;
    int option;

    while ((option = getopt(argc, argv, "lxR")) != -1) {
        if (option == 'l')
            mode = 1;
        else if (option == 'x')
            mode = 2;
        else if (option == 'R')
            recursive_mode = 1;
        else {
            fprintf(stderr,
                    "Usage: %s [-l] [-x] [-R] [directory]\n",
                    argv[0]);
            return 1;
        }
    }

    if (optind == argc) {
        do_ls(".", mode);
    } else {
        for (int i = optind; i < argc; i++) {
            if (argc - optind > 1)
                printf("%s:\n", argv[i]);

            do_ls(argv[i], mode);

            if (i < argc - 1)
                putchar('\n');
        }
    }

    return 0;
}
