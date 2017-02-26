#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

int main(int argc, char *argv[]){
    // Fichier d'operations donne en parametre
    if(argc != 2){
        fprintf(stderr, "\n" );
        return EXIT_FAILURE;
    }
    FILE *operations = fopen(argv[1], "r");
    char operation[23];

    // Fichier utilise pour le disque
    FILE *disque = fopen("disque", "ab+");
    ftruncate(fileno(disque), 512 * 1000);

    // Lecture des operations
    while(fscanf(operations, "%s", operation) != EOF){        
        if(strcmp(operation, "creation_fichier\0")){
            creationFicher();
        } else if(strcmp(operation, "suppression_fichier\0")){
            suppressionFichier();
        } else if(strcmp(operation, "creation_repertoire\0")){
            creationRepertoire();
        } else if(strcmp(operation, "suppression_repertoire\0")){
            suppressionRepertoire();
        } else if(strcmp(operation, "lire_fichier\0")){
            lireFichier();
        } else {
            fprintf(stderr, "\n" );
            return EXIT_FAILURE;
        }
    }

    fclose(operations);
    fclose(disque);
}

void creationFicher(){

}

void suppressionFichier(){

}

void creationRepertoire(){

}

void suppressionRepertoire(){

}

void lireFichier(){

}
