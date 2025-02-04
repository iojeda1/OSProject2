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

// adjust window to new size
int width() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl was unsuccessful");
        exit(1);
      } else {
        return ws.ws_col;
      }
  }


 // handle control c
 void hndl_sig(int sig) {
    exit(0);
  }
 
// read directory
 void read_dir(const char *directory, int columnw) {
    // count variables
    int fcount = 0;
    int dcount = 0;
    int lcount = 0;
    // open directory
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
        // check the type of file in the path
        char *type;
        char contents[100] = ""; // file contents, better to use buffer
        if (S_ISDIR(sb.st_mode)) {
            type = "directory";
            snprintf(contents, sizeof(contents), "(directory)");
            dcount++;
        } else if (S_ISLNK(sb.st_mode)) {
            type = "link";
            char ltarget[PATH_MAX];
            ssize_t l = readlink(path, ltarget, sizeof(ltarget) -1);
            if (l == -1) {
                snprintf(contents, sizeof(contents), "(invalid link)");
            } else if (l > 10) { // some links are too long and disturb the table
                snprintf(contents, sizeof(contents), "-> %.20s...", ltarget);
            } else { // if not too long print normally
                ltarget[l] = '\0';
                snprintf(contents, sizeof(contents), "-> %.90s", ltarget);
            }
            lcount++;
        } else if (S_ISREG(sb.st_mode)) {
            type = "file";
            int f = open(path, O_RDONLY); // open file read only mode
            if (f == -1) { // error opening file
                snprintf(contents, sizeof(contents), "(reading error)");
            } else {
                char buff[100];
                ssize_t bytes = read(f, buff, sizeof(buff) - 1);
                if (bytes > 0) {
                    buff[bytes] = '\0';
                    for (ssize_t i = 0; i < bytes; i++) {
                        if (buff[i] == '\n') { // might not be needed but some file names where disrupting my table     with a \n at the end when testing
                            buff[i] = ' ';
                        }
                    }
                snprintf(contents, sizeof(contents), "%.20s...", buff);
                } else { // no bytes means empty file
                    snprintf(contents, sizeof(contents), "(empty)");
                }
            close(f);
            }
            fcount++;
        } else {
            type = "other";
            snprintf(contents, sizeof(contents), "(unknown)");
        }
        // get file owner
        struct passwd *p = getpwuid(sb.st_uid);
        char *owner;
        if (p) {
            owner = p->pw_name;
        } else {
            owner = "unknown";
        }
        int cw = columnw - 120;
      // SPACING
        printf("%-20s %-8ld %-12s %-8o %-10s", entry->d_name, sb.st_size, type, sb.st_mode & 0777, owner);
        printf("%*s\n", cw, contents);
    }
    closedir(dir);
    printf("---------------------------------------------------------------------------------------\n");
    printf("totals: %d files, %d directories, and %d symlinks\n", fcount, dcount, lcount);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, hndl_sig);
    // can only accept two arguments, else it is an error
    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments.\n");
        exit(1);
    }
    const char *directory = argv[1];
    int colw = width();
    printf("Directory %s\n", directory);
    while (1) {
        system("clear"); // clear screen
        printf("Contents of %s\n", directory);
        time_t x = time(NULL); // get current time and print it
        printf("Current time: %s", ctime(&x));
        // print header
        printf("NAME                  SIZE      TYPE      MODE      OWNER        CONTENTS\n");
        printf("------------------------------------------------------------------------------------\n");
        read_dir(directory, colw);
        colw = width();
        sleep(3); // refresh every 3 seconds
    }
    return 0;

<<<<<<< HEAD
}
=======
}
>>>>>>> 1315edf000bb3f7e84c293897f132661a7b694bf
