#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

int main(int argc, char *argv[]){
    char operation[MAX_OPERATION];
    int tab[1000] = {0};

    if(argc != 2){
        fprintf(stderr, "--Nom de fichier manquant ou arguments en trop--\n" );
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
    FILE *blocsCharge = fopen(FICHIER_BLOCS, "ab+");

    chargerTableBits(tab, blocsCharge);
    fclose(blocsCharge);

    creerRepertoireRacine(repertoires);
    
    // Lecture des operations
    while(fscanf(operations, "%s", operation) != EOF){
        if(strcmp(operation, "creation_fichier")  == 0){
            printf("creation de fichier\n");
            creationFicher(operations, repertoires, inodes, tab);
            printf("\n");
        } else if(strcmp(operation, "suppression_fichier") == 0){
            printf("suppression de fichier\n");
            suppressionFichier(operations, repertoires, inodes);
            printf("\n");
        } else if(strcmp(operation, "creation_repertoire") == 0){
            printf("creation de repertoire\n");
            creationRepertoire(operations, repertoires);
            printf("\n");
        } else if(strcmp(operation, "suppression_repertoire") == 0){
            printf("suppression de repertoire\n");
            suppressionRepertoire(operations, repertoires, inodes);
            printf("\n");
        } else if(strcmp(operation, "lire_fichier") == 0){
            printf("lire de fichier\n");
            lireFichier(operations, repertoires, inodes);
            printf("\n");
        } else {
            fprintf(stderr, "--Erreur dans le fichier d'operations--" );
            return EXIT_FAILURE;
        }
    }

    FILE *blocsSauvegarde = fopen(FICHIER_BLOCS, "wb");
    sauvegarderTableBits(tab, blocsSauvegarde);

    fclose(operations);
    fclose(disque);
    fclose(repertoires);
    fclose(inodes);
    fclose(blocsSauvegarde);

    return 0;
}

void creerRepertoireRacine(FILE *repertoires){
    struct repertoire *r = malloc(sizeof(struct repertoire));

    fseek(repertoires, 0, SEEK_END);
    if(ftell(repertoires) == 0){      
        strcpy(r->chemin, "/\0");
        fseek(repertoires, 0, SEEK_SET);
        fwrite(r, sizeof(struct repertoire), 1, repertoires);
        free(r);
    }

    return;
}

int divisionPlafond(int num, int den){
    int resultat;
    if(num % den != 0){
        resultat = num / den + 1;
    }else{
        resultat = num / den;
    }
    return resultat;
}

char ** fragmenterContenu(const char *contenu){
    int taille = strlen(contenu) - 1;
    int reste = taille % MAX_BLOCS;
    int nbFragments = divisionPlafond(taille, MAX_BLOCS);
    char **fragments;
    fragments = (char**) malloc(nbFragments*sizeof(char*));
    for(int i = 0; i < nbFragments; ++i){
        fragments[i] = (char*) malloc(MAX_BLOCS*sizeof(char) + 1);
        if(i == nbFragments - 1){

            memcpy(fragments[i], contenu + (MAX_BLOCS * i), reste);
            strcpy(&fragments[i][reste], "\0");
            //printf("Fragment:%s\n", fragments[i]);
        }else {
            memcpy(fragments[i], contenu + (MAX_BLOCS * i), MAX_BLOCS);
            strcpy(&fragments[i][MAX_BLOCS], "\0");
            //printf("Fragment:%s\n", fragments[i]);
        }
    }
    return fragments;
}

void chargerTableBits(int *tab, FILE *blocs){
    fseek(blocs, 0, SEEK_END);
    if(ftell(blocs) != 0){
        fseek(blocs, 0, SEEK_SET);
        fread(tab, sizeof(int), 1000, blocs);
    }
}

void sauvegarderTableBits(int *tab, FILE *blocs){
    rewind(blocs);
    for(int i = 0; i < 1000; ++i){
        fwrite(&tab[i], sizeof(int), 1, blocs);
    }
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
        fprintf(stderr, "--Chemin absolu trop long--\n");
        return estOK = false;
    }

    return estOK;
}

bool lireContenu(FILE *operations, char *contenu){
    bool estOK = true;

    fseek(operations, 1, SEEK_CUR);
    fgets(contenu, MAX_CONTENU + 1, operations);

    if(strstr(contenu, "creation_fichier") != NULL ||
    strstr(contenu, "suppression_fichier") != NULL ||
    strstr(contenu, "creation_repertoire") != NULL ||
    strstr(contenu, "suppression_repertoire") != NULL ||
    strstr(contenu, "lire_fichier") != NULL){
        fprintf(stderr, "--Le fichier ne peut pas etre vide--\n");
        fseek(operations, -(strlen(contenu)), SEEK_CUR);
        return estOK = false;
    } else if(strlen(contenu) >= MAX_CONTENU){
        fprintf(stderr, "--Contenu du fichier trop long--\n");
        fscanf(operations, "%*[^\n]");
        return estOK = false;
    }

    printf("Contenu : %s", contenu);

    return estOK;
}

int assignerId(int *tab){
    int i = 0;
    while(testBit(tab, i) == 1 && i <= 32000){
        ++i;
    }
    return i;
}

void creationFicher(FILE *operations, FILE *repertoires, FILE *inodes, int *tab){
    char nom[MAX_CHEMIN + 1];
    char contenu[MAX_CONTENU + 1];
    bool cheminOk, fichierOk, repertoireParentOk = false;
    int nbFragments;
    struct inode *i = malloc(sizeof(struct inode));
    // Verifie si le disque est plein
    cheminOk = lireChemin(operations, nom);
    fichierOk = !fichierExiste(nom, inodes);
    repertoireParentOk = repertoireParentExiste(nom, repertoires);
    if(lireContenu(operations, contenu) && cheminOk && fichierOk && repertoireParentOk){
        i->id = assignerId(tab);
        nbFragments = divisionPlafond(strlen(contenu) - 1, MAX_BLOCS);
        char ** fragments = fragmenterContenu(contenu);
        free(fragments);
    }else if(!fichierOk){
        printf("--Le fichier existe deja--\n");
    } else if(!repertoireParentOk){
        printf("--Le repertoire n'existe pas--\n");
    }
    printf("----------------------------ID %d", i->id);
    free(i);
}

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *inodes){
    char nom[MAX_CHEMIN + 1];

    if(lireChemin(operations, nom)){
        if(fichierExiste(nom, inodes)){
            // Libere les blocs du fichier

            // Supprime l'i-node du fichier

        } else {
            fprintf(stderr, "--Le fichier n'existe pas--\n");
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
                fprintf(stderr, "--Le repertoire existe deja--\n");
            }
        } else {
            fprintf(stderr, "--Le repertoire parent n'existe pas--\n");
        }
    }

}

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *inodes){
    char chemin[MAX_CHEMIN + 1];

    if(lireChemin(operations, chemin)){
        if(repertoireExiste(chemin, repertoires)){
            // Supprimer son contenu

            // Supprime le repertoire du fichier de repertoires

        } else {
            fprintf(stderr, "--Le repertoire n'existe pas--\n");
        }
    }

}

void lireFichier(FILE *operations, FILE *repertoires, FILE *inodes){
    char nom[MAX_CHEMIN + 1];

    if(lireChemin(operations, nom)){
        if(fichierExiste(nom, inodes)){
            // Affichage du contenu des blocs associes

        } else {
            fprintf(stderr, "--Le fichier n'existe pas--\n");
        }
    }

}

void  setBit(int tab[],  int index){
      tab[index / 32] |= 1 << (index % 32);
}

int testBit(int tab[],  int index){
      return ( (tab[index / 32] & (1 << (index % 32) )) != 0 ) ;
}

void  clearBit(int tab[],  int index){
      tab[index / 32] &= ~(1 << (index % 32));
 }
