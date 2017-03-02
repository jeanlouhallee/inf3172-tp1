#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

#define MAX_CHEMIN 41
#define MAX_BLOC 16

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

    // Fichier contenant une liste des repertoires
    FILE *repertoires = fopen("repertoires", "ab+");

    // Fichier contenant une liste des blocs libres
    FILE *blocs = fopen("blocs", "ab+");

    // Lecture des operations
    while(fscanf(operations, "%s", operation) != EOF){
        if(strcmp(operation, "creation_fichier\0")){
            creationFicher(operations, repertoires, blocs);
        } else if(strcmp(operation, "suppression_fichier\0")){
            suppressionFichier(operations, repertoires, blocs);
        } else if(strcmp(operation, "creation_repertoire\0")){
            creationRepertoire(operations, repertoires);
        } else if(strcmp(operation, "suppression_repertoire\0")){
            suppressionRepertoire(operations, repertoires, blocs);
        } else if(strcmp(operation, "lire_fichier\0")){
            lireFichier(operations, repertoires);
        } else {
            fprintf(stderr, "\n" );
            return EXIT_FAILURE;
        }
    }

    fclose(operations);
    fclose(disque);
    fclose(repertoires);
}

void verifierCheminAbsolu(char *chemin){
    if(strlen(chemin) > 40){
        fprintf(stderr, "\n" );
        exit(EXIT_FAILURE);
    }

    return;
}

void lireCheminAbsolu(FILE *operations, FILE *repertoires, char *chemin){
    char repertoire[MAX_CHEMIN];

    while(fscanf(operations, "/%[^/]", repertoire)){
        strcat(chemin, "/");
        verifierCheminAbsolu(strcat(chemin, repertoire));

        //Verifie si le repertoire existe
    }

    return;
}

void creationFicher(FILE *operations, FILE *repertoires, FILE *blocs){
    char chemin[MAX_CHEMIN] = "";
    char nom[MAX_CHEMIN];

    // Verifie si le disque est plein

    lireCheminAbsolu(operations, repertoires, chemin);

    // Lecture du nom du fichier
    fscanf(operations, "%s", nom);

    // Verifie si le fichier existe deja

    // Lecture du contenu du fichier
    //fscanf(operations, "%[^\n]", );

    // Verifie si le fichier est trop volumineux

    // Verifie s'il reste assez d'espace sur le disque

    // Cree un i-node pour le fichier

    // Enregistre le fichier sur le disque

    // Indique que les blocs sont utilises
}

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *blocs){
    char chemin[MAX_CHEMIN];

    lireCheminAbsolu(operations, repertoires, chemin);

    // Lecture du nom du fichier

    // Verifie si le fichier existe

    // Libere les blocs du fichier

    // Supprime l'i-node du fichier
}

void creationRepertoire(FILE *operations, FILE *repertoires){
    char chemin[MAX_CHEMIN];
    char nom[MAX_CHEMIN];

    lireCheminAbsolu(operations, repertoires, chemin);

    // Lecture du nom du nouveau repertoire

    // Verifie si le repertoire existe deja

    verifierCheminAbsolu(strcat(chemin, repertoires));

    // Enregistre dans le fichier de repertoires
}

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *blocs){
    char chemin[MAX_CHEMIN];

    lireCheminAbsolu(operations, repertoires, chemin);

    // Lecture du nom du repertoire

    // Verifie si le repertoire existe

    // Supprimer son contenu

    // Supprime le repertoire du fichier
}

void lireFichier(FILE *operations, FILE *repertoires){
    char chemin[MAX_CHEMIN];

    lireCheminAbsolu(operations, repertoires, chemin);

    // Lecture du nom du fichier

    // Verifie si le fichier existe

    // Affichage du contenu des blocs associes
}
