#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

#define MAX_CHEMIN 250 // 41 ??
#define MAX_BLOCS 16
#define MAX_OPERATION 23
#define MAX_CONTENU 256 // 250 ??
#define FICHIER_DISQUE "disque"
#define FICHIER_REPERTOIRES "repertoires"
#define FICHIER_INODES "inodes"
#define FICHIER_BLOCS "blocs"

int main(int argc, char *argv[]){
    char operation[MAX_OPERATION];
    //char donnees[1000];

    if(argc != 2){
        fprintf(stderr, "Nom de fichier manquant ou arguments en trop\n" );
        return EXIT_FAILURE;
    }

    // Fichier d'operations donne en parametre
    FILE *operations = fopen(argv[1], "r");
    // Fichier utilise pour le disque
    FILE *disque = fopen(FICHIER_DISQUE, "ab+");
    ftruncate(fileno(disque), 512 * 1000);
    // Fichier contenant une liste des repertoires
    FILE *repertoires = fopen(FICHIER_REPERTOIRES, "ab+");
    // Fichier contenant les i-nodes
    FILE *inodes = fopen(FICHIER_INODES, "ab+");
    // Fichier contenant une liste des blocs libres
    FILE *blocs = fopen(FICHIER_BLOCS, "ab+");

    // Creation du repertoire racine
    struct repertoire *r = malloc(sizeof(struct repertoire));
    strcpy(r->chemin, "/\0");
    fseek(repertoires, 0, SEEK_SET);
    fwrite(r, sizeof(struct repertoire), 1, repertoires);

    // Lecture des operations
    while(fscanf(operations, "%s", operation) != EOF){
        if(strcmp(operation, "creation_fichier")  == 0){
            printf("creation de fichier\n");
            creationFicher(operations, repertoires, inodes, blocs);
        } else if(strcmp(operation, "suppression_fichier\0") == 0){
            printf("suppression de fichier\n");
            suppressionFichier(operations, repertoires, inodes, blocs);
        } else if(strcmp(operation, "creation_repertoire") == 0){
            printf("creation de repertoire\n");
            creationRepertoire(operations, repertoires);
        } else if(strcmp(operation, "suppression_repertoire\0") == 0){
            printf("suppression de repertoire\n");
            suppressionRepertoire(operations, repertoires, inodes, blocs);
        } else if(strcmp(operation, "lire_fichier\0") == 0){
            printf("lire de fichier\n");
            lireFichier(operations, repertoires, inodes);
        } else {
            fprintf(stderr, "\n" );
            return EXIT_FAILURE;
        }
    }

    fclose(operations);
    fclose(disque);
    fclose(repertoires);
    fclose(inodes);
    fclose(blocs);

    return 0;
}

bool fichierExiste(char *nom, FILE *inodes){
    bool existe = false;
    struct inode buffer;

    fseek(inodes, 0, SEEK_SET);
    while((fread(&buffer, sizeof(struct inode), 1, inodes) != 0) && !existe){

        if(strcmp(buffer.nom, nom) == 0){
            fprintf(stderr, "Fichier existe deja.\n" );
            existe = true;
        }
    }
    return existe;
}

bool repertoireExiste(char *chemin, FILE *repertoires){
    bool existe = false;
    struct repertoire buffer;

    fseek(repertoires, 0, SEEK_SET);
    while((fread(&buffer, sizeof(struct repertoire), 1, repertoires) != 0) && !existe){

        if(strcmp(buffer.chemin, chemin) == 0){
            fprintf(stderr, "Repertoire existe deja.\n" );
            existe = true;

        }
    }
    return existe;
}

void lireNom(char *nom, char *chemin){
    char *ptrNom;

    ptrNom = strrchr(chemin, '/') + 1;
    strcpy(nom, ptrNom);
    printf("Nom : %s\n", nom);

    return;
}

void lireRepertoireParent(char *parent, char *chemin, char *nom){
    int longueur;

    longueur = strlen(chemin) - strlen(nom) - 1;
    if(longueur == 0){
        strcpy(parent, "/\0");
    } else {
        strncpy(parent, chemin, longueur);
        parent[longueur] = '\0';
    }
    printf("Repertoire parent : %s\n", parent);

    return;
}

bool repertoireParentExiste(char *chemin, FILE *repertoires){
    bool existeDeja = false;
    char repertoireParent[MAX_CHEMIN];
    char nom[MAX_CHEMIN];

    lireNom(nom, chemin);
    lireRepertoireParent(repertoireParent, chemin, nom);

    existeDeja = repertoireExiste(repertoireParent, repertoires);

    return existeDeja;
}

// char* nomFichier(char chemin[]){
//     int i;
//     char *path = malloc(sizeof(char)*(strlen(chemin) + 2));

//     strcpy(path, chemin);
//     strcat(path, "\0");

//     i = strlen(path);
//     while(path[i] != '/'){
//         printf("%c\n", path[i]);
//         --i;
//     }
//     i = strlen(path) - i - 1;
//     char *nomFichier = (path + strlen(chemin)) - i;
//     free(path);
//     return nomFichier;
// }

void lireChemin(FILE *operations, char *chemin){

    fscanf(operations, "%s", chemin);
    printf("Chemin : %s\n", chemin);

    if(strlen(chemin) > 40){
        fprintf(stderr, "Chemin absolu trop long\n" );
        exit(EXIT_FAILURE);
    }

    return;
}

void lireContenu(FILE *operations, char *contenu){
    
    fseek(operations, 1, SEEK_CUR);
    fgets(contenu, MAX_CONTENU, operations);

    if(strlen(contenu) > 256){
        fprintf(stderr, "Contenu du fichier trop long\n" );
        exit(EXIT_FAILURE);
    } else if(strlen(contenu) == 0){
        fprintf(stderr, "Le fichier ne peut pas etre vide\n" );
        exit(EXIT_FAILURE);
    }
    printf("Contenu : %s\n", contenu);

    return;
}

void creationFicher(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs){
    char nom[MAX_CHEMIN];
    char contenu[MAX_CONTENU];
    struct inode *i = malloc(sizeof(struct inode));

    // Verifie si le disque est plein

    lireChemin(operations, nom);

    if(!fichierExiste(nom, inodes)){
        if(repertoireParentExiste(nom, repertoires)){
            strcpy(i->nom, nom);

            lireContenu(operations, contenu);
            
            // Verifie s'il reste assez d'espace sur le disque

            // Cree un i-node pour le fichier

            // Enregistre le fichier sur le disque

            // Indique que les blocs sont utilises

        } else {
        fprintf(stderr, "Le repertoire n'existe pas\n" );
        exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Le fichier existe deja\n" );
        exit(EXIT_FAILURE);
    }
}

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs){
    char nom[MAX_CHEMIN];

    lireChemin(operations, nom);

    if(fichierExiste(nom, inodes)){
        // Libere les blocs du fichier

        // Supprime l'i-node du fichier

    } else {
        fprintf(stderr, "Le fichier n'existe pas\n" );
        exit(EXIT_FAILURE);
    }

}

void creationRepertoire(FILE *operations, FILE *repertoires){
    char chemin[MAX_CHEMIN];
    struct repertoire *r = malloc(sizeof(struct repertoire));

    lireChemin(operations, chemin);

    if(repertoireParentExiste(chemin, repertoires)){
        if(!repertoireExiste(chemin, repertoires)){
            strcpy(r->chemin, chemin);
            fseek(repertoires, 0, SEEK_END);
            fwrite(r, sizeof(struct repertoire), 1, repertoires);
        } else {
            fprintf(stderr, "Le repertoire existe deja\n" );
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Le repertoire parent n'existe pas\n" );
        exit(EXIT_FAILURE);
    }

}

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs){
    char chemin[MAX_CHEMIN];

    lireChemin(operations, chemin);

    if(repertoireExiste(chemin, repertoires)){
        // Supprimer son contenu

        // Supprime le repertoire du fichier de repertoires

    } else {
        fprintf(stderr, "Le repertoire n'existe pas\n" );
        exit(EXIT_FAILURE);
    }

}

void lireFichier(FILE *operations, FILE *repertoires, FILE *inodes){
    char nom[MAX_CHEMIN];

    lireChemin(operations, nom);

    if(fichierExiste(nom, inodes)){
        // Affichage du contenu des blocs associes

    } else {
        fprintf(stderr, "Le fichier n'existe pas\n" );
        exit(EXIT_FAILURE);
    }

}
