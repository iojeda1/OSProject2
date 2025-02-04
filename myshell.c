#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h> // opendir
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <sys/ioctl.h>

void read_dir(const char *directory) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror ("Error: cannot open directory. \n");
        return;
    }
    struct dirent *entry;
    struct stat sb;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Skip parent and current directories
        }
        char path[PATH_MAX];
        // concatonate directory path
        snprintf(path, PATH_MAX, "%s/%s", directory, entry->d_name);
        if (lstat(path, &sb) == -1) {
            perror("Error getting metadata.\n");
            continue;
        }

        char *type;
        if (S_ISDIR(sb.st_mode)) {
            type = "directory"; 
        } else if (S_ISLINK(sb.st_mode)) {
            type = "link"; 
        } else if (S_ISREG(sb.st_mode)) {
            type = "file"; 
        } else {
            type = "other"; 
        }

        struct passwd *p = getpwuid(sb.st_uid);
        char *owner;
        if (p) {
            owner = p->pw_name;
        } else {
            owner = "unknown";
        }

        printf("%-20s %-8ld %-12s %-8o %-10s", entry->d_name, sb.st_size, type, sb.st_mode & 0777, owner);
        closedir(dir);
    printf("---------------------------------------------------------------------------------------\n");
} 

void list(char *input) {
    char *dir = input + 4; 
    while (*dir == " ") {
        dir++; 
    }
    if (*dir == '\0') {
        read_dir("."); 
    } else {
        read_dir(dir); 
    }
    }

int main() {
    char input[1024]; 
    while (1) {
        printf("myshell> "); 
        if fgets(input, 1024, stdin) == NULL) {
            printf("\n"); 
            break;
        }
        // remove fgets newline 
        input[strcspn(input, "\n")] == '\0'; 
        if (strcmp(input, "list", 4) == 0) {
            list(input); 
        } else {
            printf("Error processing command \n"); 
        }
    }

    return 0; 
}
