#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

#define MAX_CHEMIN 1000
#define MAX_BLOC 16
#define MAX_OPERATION 23
#define MAX_CONTENU 250

int main(int argc, char *argv[]){
    char operation[MAX_OPERATION];
    //char chemin[MAX_CHEMIN];
    //char donnees[1000];

    // Fichier d'operations donne en parametre
    if(argc != 2){
        fprintf(stderr, "\n" );
        return EXIT_FAILURE;
    }

    FILE *operations = fopen(argv[1], "r");
    // Fichier utilise pour le disque
    FILE *disque = fopen("disque", "ab+");
    ftruncate(fileno(disque), 512 * 1000);
    // Fichier contenant une liste des repertoires
    FILE *repertoires = fopen("repertoires", "ab+");
    // Fichier contenant une liste des blocs libres
    FILE *blocs = fopen("blocs", "ab+");

    // Lecture des operations
    while(fscanf(operations, "%s", operation) != EOF){
        if(strcmp(operation, "creation_fichier")  == 0){
            printf("creation de fichier\n");
            creationFicher(operations, repertoires, blocs);
        } else if(strcmp(operation, "suppression_fichier\0") == 0){
            printf("suppression de fichier\n");
            suppressionFichier(operations, repertoires, blocs);
        } else if(strcmp(operation, "creation_repertoire") == 0){
            printf("creation de repertoire\n");
            creationRepertoire(operations, repertoires);
        } else if(strcmp(operation, "suppression_repertoire\0") == 0){
            printf("suppression de repertoire\n");
            suppressionRepertoire(operations, repertoires, blocs);
        } else if(strcmp(operation, "lire_fichier\0") == 0){
            printf("lire de fichier\n");
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
        fprintf(stderr, "Chemin trop long\n" );
        exit(EXIT_FAILURE);
    }

    return;
}

void lireCheminAbsolu(FILE *operations, FILE *repertoires, char *chemin){
    //char repertoire[MAX_CHEMIN];

    fscanf(operations, "%s", chemin);
    printf("%s\n", chemin);
    /**
    while(fscanf(operations, "/%[^/]", repertoire)){
        strcat(chemin, "/");
        verifierCheminAbsolu(strcat(chemin,(char*)repertoire));

        //Verifie si le repertoire existe
    }
    **/
    return;
}

void creationFicher(FILE *operations, FILE *repertoires, FILE *blocs){
    char chemin[MAX_CHEMIN];
    char nom[MAX_CHEMIN];
    char contenu[MAX_CONTENU];

    // Verifie si le disque est plein

    lireCheminAbsolu(operations, repertoires, chemin);

    // Lecture du nom du fichier
    fscanf(operations, "%s", nom);
    fgets(contenu, MAX_CONTENU, operations);
    printf("%s", contenu);
    verifierCheminAbsolu(chemin);

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
    //char nom[MAX_CHEMIN];

    lireCheminAbsolu(operations, repertoires, chemin);

    // Lecture du nom du nouveau repertoire

    // Verifie si le repertoire existe deja

    //***verifierCheminAbsolu(strcat(chemin, (char*)repertoires));
    printf("Test chemin : %s\n", chemin);
    verifierCheminAbsolu(chemin);

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