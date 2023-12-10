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
#include <sys/wait.h>

#define ARGC_ERROR 0
#define OPENDIR_ERROR 1
#define OPENFIS_ERROR 2
#define BMP_ERROR 3
#define NOBMP_ERROR 4
#define LEGSIMB_ERROR 5
#define FILETARG_ERROR 6
#define PROCDIR_ERROR 7
#define OPENDIR2_ERROR 8
#define FORK_ERROR 9
#define PROCFIU_ERROR 10
#define COMMAND_ERROR 11

#define BUFFER_SIZE 256
#define ARRAY_SIZE 2048
#define PATH_SIZE 4096

int verif_dircrt(const char *str) {
    int count = 0;
    while (*str) {
        if (*str == '/') {
            count++;
            if (*(str + 1) == '.')
                return 2;
        }
        str++;
    }
    return count;
}

void proces_bmp(char *entry_path, int fis) {
    int latime, inaltime, dimensiune, bmp, fis2;
    char buffer[BUFFER_SIZE], fis_out[PATH_SIZE];
    struct stat st, file_stat;
    struct tm *tm_info;

    sprintf(fis_out, "%s_statistica.txt", entry_path);
    if((fis2 = open(fis_out, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }

    sprintf(buffer, "Nume fisier: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    if ((bmp = open(entry_path, O_RDONLY)) < 0) {
        perror("Eroare la deschiderea fisierului BMP.\n");
        exit(BMP_ERROR);
    }

    lseek(bmp, 18, SEEK_SET);
    read(bmp, &latime, sizeof(int));
    read(bmp, &inaltime, sizeof(int));

    sprintf(buffer, "Inaltime: %d\n", inaltime);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));
    sprintf(buffer, "Latime: %d\n", latime);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    lseek(bmp, 2, SEEK_SET);
    read(bmp, &dimensiune, sizeof(int));
    sprintf(buffer, "Dimensiune: %d B\n", dimensiune);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Identificatorul utilizatorului: %d\n", getuid());
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    if (stat(entry_path, &file_stat) == 0) {
        tm_info = localtime(&(file_stat.st_mtime));
        sprintf(buffer, "Timpul ultimei modificari: %d.%d.%d\n", tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900);
        write(fis, buffer, strlen(buffer));
        write(fis2, buffer, strlen(buffer));
    }

    sprintf(buffer, "Contorul de legaturi: %ld\n", st.st_nlink);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
            (st.st_mode & S_IRUSR) ? 'R' : '-',
            (st.st_mode & S_IWUSR) ? 'W' : '-',
            (st.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces grup: %c%c%c\n",
            (st.st_mode & S_IRGRP) ? 'R' : '-',
            (st.st_mode & S_IWGRP) ? 'W' : '-',
            (st.st_mode & S_IXGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces altii: %c%c%c\n",
            (st.st_mode & S_IROTH) ? 'R' : '-',
            (st.st_mode & S_IWOTH) ? 'W' : '-',
            (st.st_mode & S_IXOTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));
    write(fis, "\n", 1);
    write(fis2, "\n", 1);

    close(bmp);
    close(fis2);
}

void proces_nobmp(char *entry_path, int fis, char caracter) {
    int fis2, status;
    char buffer[BUFFER_SIZE], *fis_out;
    struct stat file_stat;
    struct tm *tm_info;

    if (stat(entry_path, &file_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre fisier.\n");
        exit(NOBMP_ERROR);
    }

    sprintf(fis_out, "%s_statistica.txt", entry_path);
    if((fis2 = open(fis_out, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }

    sprintf(buffer, "Nume fisier: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Dimensiune: %ld B\n", file_stat.st_size);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Identificatorul utilizatorului: %d\n", file_stat.st_uid);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    tm_info = localtime(&file_stat.st_mtime);
    sprintf(buffer, "Timpul ultimei modificari: %d.%d.%d\n", tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Contorul de legaturi: %ld\n", file_stat.st_nlink);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (file_stat.st_mode & S_IRUSR) ? 'R' : '-',
        (file_stat.st_mode & S_IWUSR) ? 'W' : '-',
        (file_stat.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (file_stat.st_mode & S_IRGRP) ? 'R' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'W' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (file_stat.st_mode & S_IROTH) ? 'R' : '-',
        (file_stat.st_mode & S_IROTH) ? 'W' : '-',
        (file_stat.st_mode & S_IROTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));
    write(fis, "\n", 1);
    write(fis2, "\n", 1);

    pid_t pid = fork();
    if (pid < 0) {
        perror("Eroare la fork().\n");
        exit(FORK_ERROR);
    } 
    else if (pid == 0) { 
        char command[ARRAY_SIZE];
        snprintf(command, sizeof(command), "bash script.sh %s %c", entry_path, caracter);

        int status = system(command);

        if (status == -1) {
            printf("Eroare la executarea comenzii!\n");
        } 
        else {
            printf("Comanda executată cu succes!\n");
            exit(COMMAND_ERROR);
        }
    }
    else {
        pid_t fiu = wait(&status);
        if(fiu < 0) {
            perror("Procesul fiu nu s-a terminat.\n");
                exit(PROCFIU_ERROR);
        }
        if(WIFEXITED(status)) {
            printf("S-a încheiat procesul cu pid-ul %d și codul %d.\n", fiu, WEXITSTATUS(status));              
        }
    }

    close(fis2);
}

void proces_legsimb(char *entry_path, int fis) {
    int fis2;
    char buffer[BUFFER_SIZE], *fis_out;
    struct stat file_stat, target_stat;
    
    if (lstat(entry_path, &file_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre legatura simbolica.\n");
        exit(LEGSIMB_ERROR);
    }

    sprintf(fis_out, "%s_statistica.txt", entry_path);
    if((fis2 = open(fis_out, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }

    sprintf(buffer, "Nume legatura: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Dimensiune legatura: %ld bytes\n", file_stat.st_size);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    if (stat(entry_path, &target_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre fisierul tinta.\n");
        exit(FILETARG_ERROR);
    }

    sprintf(buffer, "Dimensiune fisier tinta: %ld bytes\n", target_stat.st_size);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user legatura: %c%c%c\n",
        (file_stat.st_mode & S_IRUSR) ? 'R' : '-',
        (file_stat.st_mode & S_IWUSR) ? 'W' : '-',
        (file_stat.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user legatura: %c%c%c\n",
        (file_stat.st_mode & S_IRGRP) ? 'R' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'W' : '-',
        (file_stat.st_mode & S_IRGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user legatura: %c%c%c\n",
        (file_stat.st_mode & S_IROTH) ? 'R' : '-',
        (file_stat.st_mode & S_IROTH) ? 'W' : '-',
        (file_stat.st_mode & S_IROTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));
    write(fis, "\n", 1);
    write(fis2, "\n", 1);

    close(fis2);
}


void proces_dir(char *entry_path, int fis) {
    int fis2;
    char buffer[BUFFER_SIZE], *fis_out;
    struct stat dir_stat;

    if (stat(entry_path, &dir_stat) == -1) {
        perror("Eroare la obtinerea informatiilor despre director.\n");
        exit(PROCDIR_ERROR);
    }

    sprintf(fis_out, "%s_statistica.txt", entry_path);
    if((fis2 = open(fis_out, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }

    sprintf(buffer, "Nume director: %s\n", entry_path);
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Identificatorul utilizatorului: %d\n", getuid());
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (dir_stat.st_mode & S_IRUSR) ? 'R' : '-',
        (dir_stat.st_mode & S_IWUSR) ? 'W' : '-',
        (dir_stat.st_mode & S_IXUSR) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (dir_stat.st_mode & S_IRGRP) ? 'R' : '-',
        (dir_stat.st_mode & S_IRGRP) ? 'W' : '-',
        (dir_stat.st_mode & S_IRGRP) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));

    sprintf(buffer, "Drepturi de acces user: %c%c%c\n",
        (dir_stat.st_mode & S_IROTH) ? 'R' : '-',
        (dir_stat.st_mode & S_IROTH) ? 'W' : '-',
        (dir_stat.st_mode & S_IROTH) ? 'X' : '-');
    write(fis, buffer, strlen(buffer));
    write(fis2, buffer, strlen(buffer));
    write(fis, "\n", 1);
    write(fis2, "\n", 1);

    close(fis2);
}

void convertire_color_table(FILE *bmp) {
    uint8_t colorTableEntry[4], blue, green, red, grayscale;

    fseek(bmp, 54, SEEK_SET);

    while (fread(colorTableEntry, sizeof(uint8_t), 4, bmp) == 4) {
        blue = colorTableEntry[0];
        green = colorTableEntry[1];
        red = colorTableEntry[2];

        grayscale = 0.299 * red + 0.587 * green + 0.114 * blue;

        fseek(bmp, -4, SEEK_CUR);
        colorTableEntry[0] = grayscale;
        colorTableEntry[1] = grayscale;
        colorTableEntry[2] = grayscale;

        fwrite(colorTableEntry, sizeof(uint8_t), 4, bmp);
    }
}


void convertire_raster_data(FILE *bmp) {
    int width, height;

    fseek(bmp, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, bmp);
    fread(&height, sizeof(int), 1, bmp);
    fseek(bmp, 54, SEEK_SET);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            uint8_t blue, green, red, grayscale;

            fread(&blue, sizeof(uint8_t), 1, bmp);
            fread(&green, sizeof(uint8_t), 1, bmp);
            fread(&red, sizeof(uint8_t), 1, bmp);

            grayscale = 0.299 * red + 0.587 * green + 0.114 * blue;

            fseek(bmp, -3, SEEK_CUR);
            fwrite(&grayscale, sizeof(uint8_t), 1, bmp);
            fwrite(&grayscale, sizeof(uint8_t), 1, bmp);
            fwrite(&grayscale, sizeof(uint8_t), 1, bmp);
        }
    }
}


void convertire_gri(const char *bmp_file) {
    FILE *bmp = fopen(bmp_file, "r+b");
    short bitcount;

    if (bmp == NULL) {
        perror("Eroare la deschiderea fisierului BMP.\n");
        exit(BMP_ERROR);
    }

    fseek(bmp, 28, SEEK_SET);

    fread(&bitcount, sizeof(short), 1, bmp);

    if(bitcount <= 8) {
        convertire_color_table(bmp);
    }
    else {
        convertire_raster_data(bmp);
    }

    fclose(bmp);
}


int main(int argc, char *argv[]) {
    int fis, i = 0, status, num_lines[ARRAY_SIZE];
    char *dir_path, *dir_out, caracter;
    DIR *dir, *dir2;
    struct stat st;
    struct dirent* entry;

    if(argc != 4) {
        perror("Numar incorect de argumente.\n");
        exit(ARGC_ERROR);
    }

    dir_path = argv[1];
    dir_out = argv[2];
    caracter = argv[3][0];

    if((dir = opendir(dir_path)) == 0) {
        perror("Usage ./program <fisier_intrare> <fisier_iesire>\n");
        exit(OPENDIR_ERROR);
    }

    if((dir2 = opendir(dir_out)) == 0) {
        perror("Usage ./program <fisier_intrare> <fisier_iesire>\n");
        exit(OPENDIR2_ERROR);
    }
    
    if((fis = open("statistica.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }

    while((entry = readdir(dir)) != 0) {
        char entry_path[PATH_SIZE] = {0};
        sprintf(entry_path, "%s/%s", dir_path, entry->d_name);

        if (lstat(entry_path, &st) == -1) {
            perror("Eroare la obtinerea informatiilor despre fisier.\n");
            exit(NOBMP_ERROR);
        }

        if(verif_dircrt(entry_path) < 2) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("Eroare la fork().\n");
                exit(FORK_ERROR);
            } 
            else if (pid == 0) { 
                if (S_ISREG(st.st_mode)) {
                    char *dot = strrchr(entry_path, '.');
                    if (dot != NULL && !strcmp(dot, ".bmp")) {
                        num_lines[i] = 11;
                        proces_bmp(entry_path, fis);
                        convertire_gri(entry_path);
                    } 
                    else {
                        num_lines[i] = 9;
                        proces_nobmp(entry_path, fis, caracter);
                    }
                }
                else if (S_ISLNK(st.st_mode)) {
                    num_lines[i] = 7;
                    proces_legsimb(entry_path, fis);
                }
                else if (S_ISDIR(st.st_mode)) {
                    num_lines[i] = 6;
                    proces_dir(entry_path, fis);
                }
            }
            else {
                pid_t fiu = wait(&status);
                if(fiu < 0) {
                    perror("Procesul fiu nu s-a terminat.\n");
                    exit(PROCFIU_ERROR);
                }
                if(WIFEXITED(status)) {
                    printf("Procesul fiu a scris %d linii în fișier.\n", num_lines[i]);  
                    printf("S-a încheiat procesul cu pid-ul %d și codul %d.\n", fiu, WEXITSTATUS(status));              
                }
            }
        }
    }

    closedir(dir);
    closedir(dir2);
    close(fis);

    return 0;
}
