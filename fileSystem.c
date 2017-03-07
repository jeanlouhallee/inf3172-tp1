#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

#define MAX_CHEMIN 41
#define MAX_BLOCS 16
#define MAX_OPERATION 23
#define MAX_CONTENU 256
#define FICHIER_DISQUE "disque"
#define FICHIER_REPERTOIRES "repertoires"
#define FICHIER_INODES "inodes"
#define FICHIER_BLOCS "blocs"

int main(int argc, char *argv[]){
    char operation[MAX_OPERATION];
    int tab[1000] = {0};

    //SetBit(tab, 24355);
    //printf("%d\n", TestBit(tab, 24355));
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

    chargerTableBits(tab, blocs);
    printf("%d\n", TestBit(tab, 24355));
    // Creation du repertoire racine
    fseek(repertoires, 0, SEEK_END);
    if(ftell(repertoires) == 0){
        struct repertoire *r = malloc(sizeof(struct repertoire));
        strcpy(r->chemin, "/\0");
        fseek(repertoires, 0, SEEK_SET);
        fwrite(r, sizeof(struct repertoire), 1, repertoires);
    }

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

    sauvegarderTableBits(tab, blocs);

    fclose(operations);
    fclose(disque);
    fclose(repertoires);
    fclose(inodes);
    fclose(blocs);

    return 0;
}

void chargerTableBits(int *tab, FILE *blocs){
    fseek(blocs, 0, SEEK_END);
    if(ftell(blocs) != 0){
            fread(tab, sizeof(int), 1000, blocs);
    }

}

void sauvegarderTableBits(int *tab, FILE *blocs){
    rewind(blocs);
    fwrite(tab, sizeof(int), 1000, blocs);
}

bool fichierExiste(char *nom, FILE *inodes){
    bool existe = false;
    struct inode buffer;

    fseek(inodes, 0, SEEK_SET);
    while((fread(&buffer, sizeof(struct inode), 1, inodes) != 0) && !existe){

        if(strcmp(buffer.nom, nom) == 0){
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
    bool existe = false;
    char repertoireParent[MAX_CHEMIN + 1];
    char nom[MAX_CHEMIN + 1];

    lireNom(nom, chemin);
    lireRepertoireParent(repertoireParent, chemin, nom);

    existe = repertoireExiste(repertoireParent, repertoires);

    return existe;
}

bool lireChemin(FILE *operations, char *chemin){
    bool estOK = true;

    fscanf(operations, "%41s", chemin);
    printf("Chemin : %s\n", chemin);

    if(strlen(chemin) >= MAX_CHEMIN){
        fprintf(stderr, "Chemin absolu trop long\n");
        return estOK = false;
    }

    return estOK;
}

bool lireContenu(FILE *operations, char *contenu){
    bool estOK = true;

    fseek(operations, 1, SEEK_CUR);
    fgets(contenu, MAX_CONTENU + 1, operations);

    if(strlen(contenu) >= MAX_CONTENU){
        fprintf(stderr, "Contenu du fichier trop long\n");
        return estOK = false;
    } else if(strlen(contenu) == 0){
        fprintf(stderr, "Le fichier ne peut pas etre vide\n");
        return estOK = false;
    }
    printf("Contenu : %s\n", contenu);

    return estOK;
}

void creationFicher(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs){
    char nom[MAX_CHEMIN + 1];
    char contenu[MAX_CONTENU + 1];
    struct inode *i = malloc(sizeof(struct inode));

    // Verifie si le disque est plein

    if(lireChemin(operations, nom)){
        if(!fichierExiste(nom, inodes)){
            if(repertoireParentExiste(nom, repertoires)){
                if(lireContenu(operations, contenu)){
                    strcpy(i->nom, nom);

                    // Verifie s'il reste assez d'espace sur le disque

                    // Cree un i-node pour le fichier

                    // Enregistre le fichier sur le disque

                    // Indique que les blocs sont utilises
                }
            } else {
            fprintf(stderr, "Le repertoire n'existe pas\n");
            //exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Le fichier existe deja\n");
            //exit(EXIT_FAILURE);
        }
    }
}

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs){
    char nom[MAX_CHEMIN + 1];

    if(lireChemin(operations, nom)){
        if(fichierExiste(nom, inodes)){
            // Libere les blocs du fichier

            // Supprime l'i-node du fichier

        } else {
            fprintf(stderr, "Le fichier n'existe pas\n");
            //exit(EXIT_FAILURE);
        }
    }

}

void creationRepertoire(FILE *operations, FILE *repertoires){
    char chemin[MAX_CHEMIN + 1];
    struct repertoire *r = malloc(sizeof(struct repertoire));

    if(lireChemin(operations, chemin)){
        if(repertoireParentExiste(chemin, repertoires)){
            if(!repertoireExiste(chemin, repertoires)){
                strcpy(r->chemin, chemin);
                fseek(repertoires, 0, SEEK_END);
                fwrite(r, sizeof(struct repertoire), 1, repertoires);
            } else {
                fprintf(stderr, "Le repertoire existe deja\n");
                //exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Le repertoire parent n'existe pas\n");
            //exit(EXIT_FAILURE);
        }
    }

}

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs){
    char chemin[MAX_CHEMIN + 1];

    if(lireChemin(operations, chemin)){
        if(repertoireExiste(chemin, repertoires)){
            // Supprimer son contenu

            // Supprime le repertoire du fichier de repertoires

        } else {
            fprintf(stderr, "Le repertoire n'existe pas\n");
            //exit(EXIT_FAILURE);
        }
    }

}

void lireFichier(FILE *operations, FILE *repertoires, FILE *inodes){
    char nom[MAX_CHEMIN + 1];

    if(lireChemin(operations, nom)){
        if(fichierExiste(nom, inodes)){
            // Affichage du contenu des blocs associes

        } else {
            fprintf(stderr, "Le fichier n'existe pas\n");
            //exit(EXIT_FAILURE);
        }
    }

}

void  SetBit(int tab[],  int index){
      tab[index / 32] |= 1 << (index % 32);
}

int TestBit(int tab[],  int index){
      return ( (tab[index / 32] & (1 << (index % 32) )) != 0 ) ;
}

void  ClearBit(int tab[],  int index){
      tab[index / 32] &= ~(1 << (index % 32));
 }
