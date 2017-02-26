#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

#define MAX_CHEMIN 41

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
            creationFicher(operations);
        } else if(strcmp(operation, "suppression_fichier\0")){
            suppressionFichier(operations);
        } else if(strcmp(operation, "creation_repertoire\0")){
            creationRepertoire(operations);
        } else if(strcmp(operation, "suppression_repertoire\0")){
            suppressionRepertoire(operations);
        } else if(strcmp(operation, "lire_fichier\0")){
            lireFichier(operations);
        } else {
            fprintf(stderr, "\n" );
            return EXIT_FAILURE;
        }
    }

    fclose(operations);
    fclose(disque);
}

void lireCheminAbsolu(FILE *operations, char *chemin){
    char repertoire[MAX_CHEMIN];

    fscanf(operations, "/");
    while(fscanf(operations, "%s/", repertoire)){
        //Verifie si le repertoire existe

        //Verifie si le chemin est trop long
        if((strlen(chemin) + strlen(repertoire)) <= 40){
            strcat(chemin, repertoire);
            strcat(chemin, "/");
        } else {
            fprintf(stderr, "\n" );
            exit(EXIT_FAILURE);
        }
    }

    return;
}

void creationFicher(FILE *operations){
    char chemin[MAX_CHEMIN] = "";
    char nom[MAX_CHEMIN];

    // Verifie si le disque est plein

    lireCheminAbsolu(operations, chemin);

    // Lecture du nom de fichier
    fscanf(operations, "%s", nom);

    // Verifie si le fichier existe deja

    // Lecture du contenu du fichier
    fscanf(operations, "%[^\n]", );

    // Verifie si le fichier est trop volumineux
}

void suppressionFichier(FILE *operations){
    char chemin[MAX_CHEMIN];

    lireCheminAbsolu(operations, chemin);
}

void creationRepertoire(FILE *operations){
    char chemin[MAX_CHEMIN];

    lireCheminAbsolu(operations, chemin);
}

void suppressionRepertoire(FILE *operations){
    char chemin[MAX_CHEMIN];

    lireCheminAbsolu(operations, chemin);
}

void lireFichier(FILE *operations){
    char chemin[MAX_CHEMIN];

    lireCheminAbsolu(operations, chemin);
}
