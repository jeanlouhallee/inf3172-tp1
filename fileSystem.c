#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

#define MAX_CHEMIN 250
#define MAX_BLOC 16
#define MAX_OPERATION 23
#define MAX_CONTENU 250
#define FICHIER_DISQUE "disque"
#define FICHIER_REPERTOIRES "repertoires"
#define FICHIER_BLOCS "blocs"

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
    FILE *disque = fopen(FICHIER_DISQUE, "ab+");
    ftruncate(fileno(disque), 512 * 1000);
    // Fichier contenant une liste des repertoires
    FILE *repertoires = fopen(FICHIER_REPERTOIRES, "ab+");
    // Fichier contenant une liste des blocs libres
    FILE *blocs = fopen(FICHIER_BLOCS, "ab+");

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
    fclose(blocs);

    return 0;
}
bool fichierExisteDeja(char *chemin, FILE *repertoires,struct repertoire *r){
    bool fichierExisteDeja = false;
    struct repertoire buffer;
    fseek(repertoires, 0, SEEK_SET);
    while((fread(&buffer, sizeof(struct repertoire), 1, repertoires) != 0) && !fichierExisteDeja){

        if(strcmp(buffer.chemin, r->chemin) == 0){
            fichierExisteDeja = true;
        }
    }
    return fichierExisteDeja;
}

bool cheminAbsoluAcceptable(char *chemin, FILE *repertoires, struct repertoire *r){
    bool estOk = true;
    if(strlen(chemin) > 40){
        fprintf(stderr, "Chemin trop long\n" );
        estOk = false;
    }else if(fichierExisteDeja(chemin, repertoires, r)){
        estOk = false;
        fprintf(stderr, "Fichier deja existant\n");
    }

    return estOk;
}

char* nomFichier(char chemin[]){
    int i;
    char *path = malloc(sizeof(char)*(strlen(chemin) + 2));

    strcpy(path, chemin);
    strcat(path, "\0");

    i = strlen(path);
    while(path[i] != '/'){
        printf("%c\n", path[i]);
        --i;
    }
    i = strlen(path) - i - 1;
    char *nomFichier = (path + strlen(chemin)) - i;
    free(path);
    return nomFichier;
}

void lireCheminAbsolu(FILE *operations, FILE *repertoires, char *chemin){
    //char repertoire[MAX_CHEMIN];

    fscanf(operations, "%s", chemin);
    printf("%s\n", chemin);
    /**
    while(fscanf(operations, "/%[^/]", repertoire)){
        strcat(chemin, "/");
        cheminAbsoluAcceptable(strcat(chemin,(char*)repertoire));
        printf("Test : %s\n", chemin);

        //Verifie si le repertoire existe
    }
    **/
    return;
}

void creationFicher(FILE *operations, FILE *repertoires, FILE *blocs){
    char chemin[MAX_CHEMIN];
    char contenu[MAX_CONTENU];
    struct repertoire *r = malloc(sizeof(struct repertoire));

    // Verifie si le disque est plein

    lireCheminAbsolu(operations, repertoires, chemin);
    strcpy(r->chemin, chemin);
    // Lecture du nom du fichier

    // Lecture du contenu
    //fscanf(operations, "%s", nom);
    fseek(operations, 1, SEEK_CUR);
    fgets(contenu, MAX_CONTENU, operations);
    printf("%s\n", contenu);
    char *nom = nomFichier(chemin);
    printf("Nom du fichier: %s\n\n", nom);
    //if(cheminAbsoluAcceptable(chemin, repertoires, r)){


    //    printf("on ecris!\n");
    //}

    // Verifie si le fichier existe deja

    // Lecture du contenu du fichier
    //fscanf(operations, "%[^\n]", );

    // Verifie si le fichier est trop volumineux

    // Verifie s'il reste assez d'espace sur le disque

    // Cree un i-node pour le fichier

    // Enregistre le fichier sur le disque

    // Indique que les blocs sont utilises
    free(r);
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
    struct repertoire *r = malloc(sizeof(struct repertoire));
    //char nom[MAX_CHEMIN];

    lireCheminAbsolu(operations, repertoires, chemin);
    strcpy(r->chemin, chemin);
    // Lecture du nom du nouveau repertoire

    // Verifie si le repertoire existe deja

    //***cheminAbsoluAcceptable(strcat(chemin, (char*)repertoires));
    if(cheminAbsoluAcceptable(chemin, repertoires, r)){
        fseek(repertoires, 0, SEEK_END);
        if(repertoires != NULL){
            fwrite(r, sizeof(struct repertoire), 1, repertoires);
        }
    }

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
