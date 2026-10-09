
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

/* Compare filenames alphabetically, ignoring case */
int compare_names(const void *a, const void *b)
{
    const char *name1 = *(const char **)a;
    const char *name2 = *(const char **)b;

    return strcasecmp(name1, name2);
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
    char path[4096];
    struct stat info;

    snprintf(path, sizeof(path), "%s/%s", dir, name);

    if (lstat(path, &info) == -1) {
        perror(name);
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

    printf("%s\n", name);
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
void print_columns(char **names, int count, int max_len)
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

            if (index < count)
                printf("%-*s", col_width, names[index]);
        }
        printf("\n");
    }
}

/* Horizontal display: -x */
void print_horizontal(char **names, int count, int max_len)
{
    int width = get_terminal_width();
    int col_width = max_len + 2;
    int position = 0;

    for (int i = 0; i < count; i++) {
        if (position > 0 && position + col_width > width) {
            printf("\n");
            position = 0;
        }

        printf("%-*s", col_width, names[i]);
        position += col_width;
    }

    if (count > 0)
        printf("\n");
}

/* Read and display directory contents */
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
        if (entry->d_name[0] == '.')
            continue;

        if (count == capacity) {
            capacity *= 2;

            char **temp = realloc(names,
                                  capacity * sizeof(char *));

            if (temp == NULL) {
                perror("realloc");
                break;
            }

            names = temp;
        }

        names[count] = strdup(entry->d_name);

        if (names[count] == NULL) {
            perror("strdup");
            break;
        }

        int len = strlen(names[count]);

        if (len > max_len)
            max_len = len;

        count++;
    }

    closedir(dp);

    /* Feature 5: Sort filenames alphabetically */
    qsort(names, count, sizeof(char *), compare_names);

    if (mode == 1) {
        for (int i = 0; i < count; i++)
            print_long(dir, names[i]);
    } else if (mode == 2) {
        print_horizontal(names, count, max_len);
    } else if (count > 0) {
        print_columns(names, count, max_len);
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

    while ((option = getopt(argc, argv, "lx")) != -1) {
        if (option == 'l')
            mode = 1;
        else if (option == 'x')
            mode = 2;
        else {
            fprintf(stderr,
                    "Usage: %s [-l|-x] [directory]\n",
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
                puts("");
        }
    }

    return 0;
}
