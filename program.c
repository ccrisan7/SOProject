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

#define BUFF_SIZE 256

int main(int argc, char *argv[]) {
    struct stat var;
    int fin, fout, rd, counter = 0;
    char buffer[BUFF_SIZE], buff2[BUFF_SIZE], buff3[BUFF_SIZE], path[201];
    DIR *dir;

    if(argc != 1) {
        perror("Nu e ok.\n");
        exit(ARGC_ERROR);
    }

    if((dir = opendir(argv)) == 0) {
        perror("Eroare la deschidere director.\n");
        exit(OPENDIR_ERROR);
    }
    
    if((fis = open("statistica.txt")) == 0) {
        perror("Eroare la deschidere fisier de iesire.\n");
        exit(OPENFIS_ERROR);
    }
    
    while(readdir(dir)) != 0) {

    }

    int offset;

    offset = fseek(fis, 2, SEEK_SET);
    printf("%d", offset);

    if((fin = open(argv[1], O_RDONLY)) < 0) {
        perror("Nu se poate deschide fisierul de intrare.\n");
    }

    if((fout = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU)) < 0) {
        perror("Nu se poate deschide fisierul de iesire.\n");
    }

    while((rd = read(fin, buffer, BUFF_SIZE)) > 0) {
        sprintf(buff2, "%s", buffer);
        for(int i = 0; i < rd; i ++) {
            if(isalnum(buff2[i])) {
                counter ++;
            }
        }
    }

    sprintf(buff3, "Conter: %d\n", counter);

    if(write(fout, &buff3, strlen(buff3)) < 0) {
        perror("Nu se poate efectua scrierea.\n");
    }

    if(fstat(fin, &var)) {
        perror("Eroare\n");
    }
    else {
        sprintf(buff3, "User ID: %d\n", var.st_uid);
    }

    if(write(fout, &buff3, strlen(buff3)) < 0) {
        perror("Nu se poate efectua scrierea.\n");
    }

    close(fin);
    close(fout);

    return 0;
}