#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

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
        switch(operation){
            case "creation_fichier\0":
                creationFicher();
                break;
            case "suppression_fichier\0":
                suppressionFichier();
                break;
            case "creation_repertoire\0":
                creationRepertoire();
                break;
            case "suppression_repertoire\0":
                suppressionRepertoire();
                break;
            case "lire_fichier\0":
                lireFichier();
                break;
            default:
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
