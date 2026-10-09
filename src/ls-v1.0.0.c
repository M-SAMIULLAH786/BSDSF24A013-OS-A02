
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/ioctl.h>

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

int get_terminal_width(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
        ws.ws_col > 0)
        return ws.ws_col;

    return 80;
}

void print_columns(char **names, int count, int max_len)
{
    int terminal_width = get_terminal_width();
    int column_width = max_len + 2;
    int columns = terminal_width / column_width;

    if (columns < 1)
        columns = 1;

    if (columns > count)
        columns = count;

    int rows = (count + columns - 1) / columns;

    /* Print down, then across */
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            int index = col * rows + row;

            if (index >= count)
                continue;

            printf("%-*s", column_width, names[index]);
        }
        printf("\n");
    }
}

void do_ls(const char *dir, int long_format)
{
    DIR *dp = opendir(dir);

    if (dp == NULL) {
        perror(dir);
        return;
    }

    char **names = NULL;
    int count = 0;
    int capacity = 10;
    int max_len = 0;

    names = malloc(capacity * sizeof(char *));
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
            char **temp = realloc(names, capacity * sizeof(char *));
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

    if (long_format) {
        for (int i = 0; i < count; i++)
            print_long(dir, names[i]);
    } else if (count > 0) {
        print_columns(names, count, max_len);
    }

    for (int i = 0; i < count; i++)
        free(names[i]);

    free(names);
}

int main(int argc, char *argv[])
{
    int long_format = 0;
    int option;

    while ((option = getopt(argc, argv, "l")) != -1) {
        if (option == 'l') {
            long_format = 1;
        } else {
            fprintf(stderr, "Usage: %s [-l] [directory]\n", argv[0]);
            return 1;
        }
    }

    if (optind == argc) {
        do_ls(".", long_format);
    } else {
        for (int i = optind; i < argc; i++) {
            if (argc - optind > 1)
                printf("%s:\n", argv[i]);

            do_ls(argv[i], long_format);

            if (i < argc - 1)
                puts("");
        }
    }

    return 0;
}
