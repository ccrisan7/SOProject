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
#include <time.h>

#define ARGC_ERROR 0
#define OPENDIR_ERROR 1
#define OPENFIS_ERROR 2
#define BMP_ERROR 3
#define NOBMP_ERROR 4
#define LEGSIMB_ERROR 5
#define FILETARG_ERROR 6
#define PROCDIR_ERROR 7

#define BUFFER_SIZE 256
#define PATH_SIZE 4096

void proces_bmp(char *entry_path, int fis) {
    int latime, inaltime, dimensiune, bmp;
    char buffer[BUFFER_SIZE];
    struct stat st, file_stat;
    struct tm *tm_info;

    sprintf(buffer, "Nume fisier: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));

    if ((bmp = open(entry_path, O_RDONLY)) < 0) {
        perror("Eroare la deschiderea fisierului BMP.\n");
        exit(BMP_ERROR);
    }

    lseek(bmp, 18, SEEK_SET);
    read(bmp, &latime, sizeof(int));
    read(bmp, &inaltime, sizeof(int));

    sprintf(buffer, "Inaltime: %d\n", inaltime);
    write(fis, buffer, strlen(buffer));
    sprintf(buffer, "Latime: %d\n", latime);
    write(fis, buffer, strlen(buffer));

    lseek(bmp, 2, SEEK_SET);
    read(bmp, &dimensiune, sizeof(int));
    sprintf(buffer, "Dimensiune: %d B\n", dimensiune);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Identificatorul utilizatorului: %d\n", getuid());
    write(fis, buffer, strlen(buffer));

    if (stat(entry_path, &file_stat) == 0) {
        tm_info = localtime(&(file_stat.st_mtime));
        sprintf(buffer, "Timpul ultimei modificari: %d.%d.%d\n", tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900);
        write(fis, buffer, strlen(buffer));
    }

    sprintf(buffer, "Contorul de legaturi: %ld\n", st.st_nlink);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
            (st.st_mode & S_IRUSR) ? 'R' : '-',
            (st.st_mode & S_IWUSR) ? 'W' : '-',
            (st.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces grup: %c%c%c\n",
            (st.st_mode & S_IRGRP) ? 'R' : '-',
            (st.st_mode & S_IWGRP) ? 'W' : '-',
            (st.st_mode & S_IXGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces altii: %c%c%c\n",
            (st.st_mode & S_IROTH) ? 'R' : '-',
            (st.st_mode & S_IWOTH) ? 'W' : '-',
            (st.st_mode & S_IXOTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis, "\n", 1);

    close(bmp);
}

void proces_nobmp(char *entry_path, int fis) {
    char buffer[BUFFER_SIZE];
    struct stat file_stat;
    struct tm *tm_info;

    if (stat(entry_path, &file_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre fisier.\n");
        exit(NOBMP_ERROR);
    }

    sprintf(buffer, "Nume fisier: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Dimensiune: %ld B\n", file_stat.st_size);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Identificatorul utilizatorului: %d\n", file_stat.st_uid);
    write(fis, buffer, strlen(buffer));

    tm_info = localtime(&file_stat.st_mtime);
    strftime(buffer, sizeof(buffer), "Timpul ultimei modificari: %d.%m.%Y\n", tm_info);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Contorul de legaturi: %ld\n", file_stat.st_nlink);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (file_stat.st_mode & S_IRUSR) ? 'R' : '-',
        (file_stat.st_mode & S_IWUSR) ? 'W' : '-',
        (file_stat.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (file_stat.st_mode & S_IRGRP) ? 'R' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'W' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (file_stat.st_mode & S_IROTH) ? 'R' : '-',
        (file_stat.st_mode & S_IROTH) ? 'W' : '-',
        (file_stat.st_mode & S_IROTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis, "\n", 1);
}

void proces_legsimb(char *entry_path, int fis) {
    char buffer[BUFFER_SIZE];
    struct stat file_stat, target_stat;
    
    if (lstat(entry_path, &file_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre legatura simbolica.\n");
        exit(LEGSIMB_ERROR);
    }

    sprintf(buffer, "Nume legatura: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Dimensiune legatura: %ld bytes\n", file_stat.st_size);
    write(fis, buffer, strlen(buffer));

    if (stat(entry_path, &target_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre fisierul tinta.\n");
        exit(FILETARG_ERROR);
    }

    sprintf(buffer, "Dimensiune fisier tinta: %ld bytes\n", target_stat.st_size);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user legatura: %c%c%c\n",
        (file_stat.st_mode & S_IRUSR) ? 'R' : '-',
        (file_stat.st_mode & S_IWUSR) ? 'W' : '-',
        (file_stat.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user legatura: %c%c%c\n",
        (file_stat.st_mode & S_IRGRP) ? 'R' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'W' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user legatura: %c%c%c\n",
        (file_stat.st_mode & S_IROTH) ? 'R' : '-',
        (file_stat.st_mode & S_IROTH) ? 'W' : '-',
        (file_stat.st_mode & S_IROTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis, "\n", 1);
}


void proces_dir(char *entry_path, int fis) {
    char buffer[BUFFER_SIZE];
    struct stat dir_stat;

    if (stat(entry_path, &dir_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre director.\n");
        exit(PROCDIR_ERROR);
    }

    sprintf(buffer, "Nume director: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Identificatorul utilizatorului: %d\n", getuid());
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (dir_stat.st_mode & S_IRUSR) ? 'R' : '-',
        (dir_stat.st_mode & S_IWUSR) ? 'W' : '-',
        (dir_stat.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (dir_stat.st_mode & S_IRGRP) ? 'R' : '-',
        (dir_stat.st_mode & S_IRGRP) ? 'W' : '-',
        (dir_stat.st_mode & S_IRGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (dir_stat.st_mode & S_IROTH) ? 'R' : '-',
        (dir_stat.st_mode & S_IROTH) ? 'W' : '-',
        (dir_stat.st_mode & S_IROTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis, "\n", 1);
}

int main(int argc, char *argv[]) {
    int fis;
    char *dir_path;
    DIR *dir;
    struct stat st;

    if(argc != 2) {
        perror("Nu e ok.\n");
        exit(ARGC_ERROR);
    }

    dir_path = argv[1];

    if((dir = opendir(dir_path)) == 0) {
        perror("Usage ./program <fisier_intrare>\n");
        exit(OPENDIR_ERROR);
    }
    
    if((fis = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }

    for(struct dirent* entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        char entry_path[PATH_SIZE] = {0};
        sprintf(entry_path, "%s/%s", dir_path, entry->d_name);

        if (lstat(entry_path, &st) == -1) {
            perror("Eroare la obtinerea informatiilor despre fisier.\n");
            exit(NOBMP_ERROR);
        }

        if (S_ISREG(st.st_mode)) {
            char *dot = strrchr(entry_path, '.');
            if (dot != NULL && !strcmp(dot, ".bmp")) {
                proces_bmp(entry_path, fis);
            } 
            else {
                proces_nobmp(entry_path, fis);
            }
        }
        else if (S_ISLNK(st.st_mode)) {
            proces_legsimb(entry_path, fis);
        }
        else if (S_ISDIR(st.st_mode)) {
            proces_dir(entry_path, fis);
        }
    }

    closedir(dir);
    close(fis);

    return 0;
}
