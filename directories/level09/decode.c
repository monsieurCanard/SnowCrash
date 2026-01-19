#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int ac, char** av) {
    if (ac != 2) return 1;

    // Lecture du fichier token
    int fd = open(av[1], O_RDONLY);
    char buf[1024];
    int ret = read(fd, buf, 1024);
    
    // Déchiffrement
    for(int i = 0; i < ret - 1; i++) { // ret - 1 pour ignorer le \n final si présent
        printf("%c", buf[i] - i);
    }
    printf("\n");
    return 0;
}