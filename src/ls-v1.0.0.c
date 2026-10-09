
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

void do_ls(const char *dir, int long_format);

void print_permissions(mode_t mode)
{
    printf("%c", S_ISDIR(mode) ? 'd' :
           S_ISLNK(mode) ? 'l' : '-');

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

void do_ls(const char *dir, int long_format)
{
    struct dirent *entry;
    DIR *dp = opendir(dir);

    if (dp == NULL) {
        perror(dir);
        return;
    }

    errno = 0;

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (long_format)
            print_long(dir, entry->d_name);
        else
            printf("%s\n", entry->d_name);
    }

    if (errno != 0)
        perror("readdir");

    closedir(dp);
}

int main(int argc, char *argv[])
{
    int long_format = 0;
    int option;

    while ((option = getopt(argc, argv, "l")) != -1) {
        if (option == 'l')
            long_format = 1;
        else {
            fprintf(stderr, "Usage: %s [-l] [directory]\n",
                    argv[0]);
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


