#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

#define ARGC_ERROR 0
#define OPENDIR_ERROR 1
#define OPENFIS_ERROR 2

#define PATH_MAX 256

int main(int argc, char *argv[]) {
    int fin, fout, rd, counter = 0;
    DIR *dir;
    FILE *fis;
    char *dir_path = argv[1];

    if(argc != 2) {
        perror("Nu e ok.\n");
        exit(ARGC_ERROR);
    }

    if((dir = opendir(dir_path)) == 0) {
        perror("Usage ./program <fisier_intrare>\n");
        exit(OPENDIR_ERROR);
    }
    
    if((fis = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }

    for(struct dirent* entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        char entry_path[PATH_MAX] = {0};
        int rc = snprintf(entry_path, sizeof entry_path, "%s/%s", dir_path, entry->d_name);
        if (rc < 0 || rc >= sizeof entry_path) {
            fprintf(stderr, "Fisier/Director necunoscut\n", entry->d_name);
            continue;
        }

        char *dot = strrchr(entry_path, '.');
        struct stat file_info;

        if (dot) {
            if (!strcmp(dot, ".bmp")) {
                proces_bmp(entry_path);
            } 
            else {
                proces_nobmp(entry_path);
            }
        } 
        if (lstat(entry_path, &file_info) == 0 && S_ISLNK(file_info.st_mode)) {
            proces_legsimb(entry_path);
        } 
        if (S_ISDIR(file_info.st_mode)) {
            proces_dir(entry_path);
        }
    }

    closedir(dir);
    close(fis);

    return 0;
}