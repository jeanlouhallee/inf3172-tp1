/*
 * TP1 : Simulation d'un systeme de fichiers
 *
 * Cours : INF3172 Systeme d'exploitation
 *
 * Auteurs : Etienne Bergeron BERE08089101
 *           Jean-Lou Hallee  HALJ05129309
 *
 * Dernières modifications : 19 mars 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

int main(int argc, char *argv[]){
    int tab[TAB_INT] = {0};

    if(argc != 2){
        fprintf(stderr, "Nom de fichier manquant ou arguments en trop.\n");
        return EXIT_FAILURE;
    }
    FILE *operations = fopen(argv[1], "r");
    if(operations == NULL){
        perror("Erreur d'ouverture de fichier : ");
        exit(EXIT_FAILURE);
    }

    FILE *disque = fopen(FICHIER_DISQUE, "r");
    if(!disque){
        disque = fopen(FICHIER_DISQUE, CREER_FIC);
        ftruncate(fileno(disque), TAILLE_DISQUE);
    }else{
        fclose(disque);
        disque = fopen(FICHIER_DISQUE, METTREAJOUR_FIC);
    }
    if(disque == NULL){
        perror("Erreur d'ouverture de fichier : ");
        exit(EXIT_FAILURE);
    }

    FILE *repertoires = fopen(FICHIER_REPERTOIRES, "r");
    if(!repertoires){
        repertoires = fopen(FICHIER_REPERTOIRES, CREER_FIC);
    }else{
        fclose(repertoires);
        repertoires = fopen(FICHIER_REPERTOIRES, METTREAJOUR_FIC);
    }
    if(repertoires == NULL){
        perror("Erreur d'ouverture de fichier : ");
        exit(EXIT_FAILURE);
    }

    FILE *inodes = fopen(FICHIER_INODES, "r");
    if(!inodes){
        inodes = fopen(FICHIER_INODES, CREER_FIC);
    }else{
        fclose(inodes);
        inodes = fopen(FICHIER_INODES, METTREAJOUR_FIC);
    }
    if(inodes == NULL){
        perror("Erreur d'ouverture de fichier : ");
        exit(EXIT_FAILURE);
    }

    FILE *blocsCharge = fopen(FICHIER_BLOCS, "ab+");
    if(blocsCharge == NULL){
        perror("Erreur d'ouverture de fichier : ");
        exit(EXIT_FAILURE);
    }
    chargerTableBits(tab, blocsCharge);
    fclose(blocsCharge);

    creerRepertoireRacine(repertoires);

    lectureOperations(operations, disque, repertoires, inodes, tab);

    FILE *blocsSauvegarde = fopen(FICHIER_BLOCS, "wb");
    if(blocsSauvegarde == NULL){
        perror("Erreur d'ouverture de fichier : ");
        exit(EXIT_FAILURE);
    }
    sauvegarderTableBits(tab, blocsSauvegarde);
    fclose(blocsSauvegarde);

    printf("FIN DU PROGRAMME.\n");

    fclose(operations);
    fclose(disque);
    fclose(repertoires);
    fclose(inodes);

    return 0;
}

void lectureOperations(FILE *operations, FILE *disque, FILE *repertoires, FILE *inodes, int *tab){
    char operation[MAX_OPERATION];
    char chemin[MAX_CHEMIN + 1];

    while(fscanf(operations, "%s", operation) != EOF){
        if(strcmp(operation, "creation_fichier")  == 0){
            if(lireChemin(operations, chemin)){
                creationFicher(operations, disque, repertoires, inodes, tab, chemin);
            }
        } else if(strcmp(operation, "suppression_fichier") == 0){
            if(lireChemin(operations, chemin)){
                suppressionFichier(repertoires, inodes, tab, chemin);
            }
        } else if(strcmp(operation, "creation_repertoire") == 0){
            if(lireChemin(operations, chemin)){
                creationRepertoire(repertoires, chemin);
            }
        } else if(strcmp(operation, "suppression_repertoire") == 0){
            if(lireChemin(operations, chemin)){
                suppressionRepertoire(repertoires, inodes, disque, tab, chemin);
            }
        } else if(strcmp(operation, "lire_fichier") == 0){
            if(lireChemin(operations, chemin)){
                lireFichier(repertoires, inodes, disque, chemin);
            }
        } else {
            fprintf(stderr, "Erreur dans le fichier d'operations.\n");
            exit(EXIT_FAILURE);
        }
    }

    return;
}

void creerRepertoireRacine(FILE *repertoires){
    fseek(repertoires, 0, SEEK_END);
    if(ftell(repertoires) == 0){
        struct repertoire *r = malloc(sizeof(struct repertoire));
        if(r == NULL){
            perror("Erreur d'allocation de memoire : ");
            exit(EXIT_FAILURE);
        }
        strcpy(r->chemin, "/\0");
        fseek(repertoires, 0, SEEK_SET);
        fwrite(r, sizeof(struct repertoire), 1, repertoires);
        free(r);
    }

    return;
}

void chargerTableBits(int tab[], FILE *blocs){
    fseek(blocs, 0, SEEK_END);
    if(ftell(blocs) != 0){
        fseek(blocs, 0, SEEK_SET);
        fread(tab, sizeof(int), 1000, blocs);
    }

    return;
}

void sauvegarderTableBits(int tab[], FILE *blocs){
    rewind(blocs);
    for(int i = 0; i < TAB_INT; ++i){
        fwrite(&tab[i], sizeof(int), 1, blocs);
    }

    return;
}

bool lireChemin(FILE *operations, char *chemin){
    bool estOK = true;

    memset(chemin, '\0', MAX_CHEMIN + 1);
    fscanf(operations, "%41s", chemin);

    if(strlen(chemin) >= MAX_CHEMIN){
        fprintf(stderr, "Chemin absolu trop long.\n");
        return estOK = false;
    }

    return estOK;
}

bool lireContenu(FILE *operations, char *contenu){
    bool estOK = true;

    memset(contenu, '\0', MAX_CONTENU + 1);
    fseek(operations, 1, SEEK_CUR);
    fgets(contenu, MAX_CONTENU + 1, operations);

    if(strstr(contenu, "creation_fichier") != NULL ||
    strstr(contenu, "suppression_fichier") != NULL ||
    strstr(contenu, "creation_repertoire") != NULL ||
    strstr(contenu, "suppression_repertoire") != NULL ||
    strstr(contenu, "lire_fichier") != NULL){
        fprintf(stderr, "Le fichier ne peut pas etre vide.\n");
        fseek(operations, -(strlen(contenu)), SEEK_CUR);
        return estOK = false;
    } else if(strlen(contenu) > MAX_CONTENU){
        fprintf(stderr, "Contenu du fichier trop long.\n");
        fscanf(operations, "%*[^\n]");
        return estOK = false;
    }

    return estOK;
}

void creationFicher(FILE *operations, FILE *disque, FILE *repertoires, FILE *inodes, int tab[], char *nom){
    int position = 0;
    char contenu[MAX_CONTENU + 1];

    if(lireContenu(operations, contenu)){
        if(!fichierExiste(nom, inodes, NULL, &position)){
            if(repertoireParentExiste(nom, repertoires)){
                struct inode *i = calloc(1, sizeof(struct inode));
                if(i == NULL){
                    perror("Erreur d'allocation de memoire : ");
                    exit(EXIT_FAILURE);
                }
                char ** fragments = fragmenterContenu(contenu, i);
                strcpy(i->nom, nom);
                ecritureFichier(disque, inodes, fragments, i, tab);
                for(int j = 0; j < i->nbFragments; ++j){
                    free(fragments[j]);
                }
                free(fragments);
                free(i);
            }else{
                fprintf(stderr, "Le repertoire n'existe pas.\n");
            }
        }else{
            fprintf(stderr, "Le fichier existe deja.\n");
        }
    }

    return;
}



void suppressionFichier(FILE *disque, FILE *inodes, int tab[], char *nom){
    int position = 0;

    struct inode *inode = malloc(sizeof(struct inode));
    if(inode == NULL){
        perror("Erreur d'allocation de memoire : ");
        exit(EXIT_FAILURE);
    }

    if(fichierExiste(nom, inodes, inode, &position)){
        struct inode *inodeVide = malloc(sizeof(struct inode));
        if(inodeVide == NULL){
            perror("Erreur d'allocation de memoire : ");
            exit(EXIT_FAILURE);
        }
        libererBlocs(tab, inode);
        memset(inodeVide->nom, '\0', MAX_CHEMIN + 1);
        fseek(inodes, (position  - 1) * sizeof(struct inode), SEEK_SET);
        fwrite(inodeVide, sizeof(struct inode), 1, inodes);
        free(inodeVide);
    } else {
        fprintf(stderr, "Le fichier n'existe pas.\n");
    }
    free(inode);

    return;
}

void creationRepertoire(FILE *repertoires, char *chemin){
    int position = 0;

    if(repertoireParentExiste(chemin, repertoires)){
        if(!repertoireExiste(chemin, repertoires, &position)){
            struct repertoire *r = malloc(sizeof(struct repertoire));
            if(r == NULL){
                perror("Erreur d'allocation de memoire : ");
                exit(EXIT_FAILURE);
            }
            strcpy(r->chemin, chemin);
            fseek(repertoires, 0, SEEK_END);
            fwrite(r, sizeof(struct repertoire), 1, repertoires);
            free(r);
        } else {
            fprintf(stderr, "Le repertoire existe deja.\n");
        }
    } else {
        fprintf(stderr, "Le repertoire parent n'existe pas.\n");
    }

    return;
}

void suppressionRepertoire(FILE *repertoires, FILE *inodes, FILE *disque, int tab[], char *chemin){
    int position = 0;

    if(repertoireExiste(chemin, repertoires, &position)){
        struct repertoire *r = malloc(sizeof(struct repertoire));
        if(r == NULL){
            perror("Erreur d'allocation de memoire : ");
            exit(EXIT_FAILURE);
        }
        memset(r->chemin, '\0', MAX_CHEMIN + 1);
        fseek(repertoires, (position - 1) * sizeof(struct repertoire), SEEK_SET);
        fwrite(r, sizeof(struct repertoire), 1, repertoires);
        free(r);

        suppressionContenu(repertoires, inodes, disque, tab, chemin);
    } else {
        fprintf(stderr, "Le repertoire n'existe pas.\n");
    }

    return;
}

void suppressionContenu(FILE *repertoires, FILE *inodes, FILE *disque, int tab[], char *chemin){
    struct inode i;
    struct repertoire r;
    int longueur = strlen(chemin);
    fpos_t position;

    fseek(inodes, 0, SEEK_SET);
    while((fread(&i, sizeof(struct inode), 1, inodes)) != 0){
        if(strlen(i.nom) >= longueur && memcmp(i.nom, chemin, longueur) == 0){
            fgetpos(inodes, &position);
            suppressionFichier(disque, inodes, tab, i.nom);
            fsetpos(inodes, &position);
        }
    }
    fseek(repertoires, 0, SEEK_SET);
    while((fread(&r, sizeof(struct repertoire), 1, repertoires)) != 0){
        if(strlen(r.chemin) >= longueur && memcmp(r.chemin, chemin, longueur) == 0){
            fgetpos(repertoires, &position);
            suppressionRepertoire(repertoires, inodes, disque, tab, r.chemin);
            fsetpos(repertoires, &position);
        }

    }

    return;
}

void lireFichier(FILE *repertoires, FILE *inodes, FILE *disque, char *nom){    
    int position = 0;

    struct inode *inode = calloc(1, sizeof(struct inode));
    if(inode == NULL){
        perror("Erreur d'allocation de memoire : ");
        exit(EXIT_FAILURE);
    }

    if(fichierExiste(nom, inodes, inode, &position)){
        for(int i = 0; i < inode->nbFragments; ++i){
            struct bloc *fragment = calloc(1, sizeof(struct bloc));
            if(fragment == NULL){
                perror("Erreur d'allocation de memoire : ");
                exit(EXIT_FAILURE);
            }
            if(i < NB_BLOCS){
                fseek(disque, inode->blocs[i] * NB_OCTETS, SEEK_SET);
                fread(fragment, sizeof(struct bloc), 1, disque);
                printf("%s", fragment->contenu);
            }else{
                fseek(disque, inode->indirect.blocs[i - NB_BLOCS] * NB_OCTETS, SEEK_SET);
                fread(fragment, sizeof(struct bloc), 1, disque);
                printf("%s", fragment->contenu);
            }
            free(fragment);
        }
    } else {
        fprintf(stderr, "Le fichier n'existe pas.\n");
    }
    free(inode);

    return;
}

bool fichierExiste(char *nom, FILE *inodes, struct inode *inode, int *position){
    bool existe = false;
    struct inode buffer;

    fseek(inodes, 0, SEEK_SET);
    while((fread(&buffer, sizeof(struct inode), 1, inodes) != 0) && !existe){
        ++(*position);
        if(strcmp(buffer.nom, nom) == 0){
            existe = true;
            if(inode != NULL){
                memcpy(inode, &buffer, sizeof(struct inode));
            }
        }
    }

    return existe;
}

bool repertoireExiste(char *chemin, FILE *repertoires, int *position){
    bool existe = false;
    struct repertoire buffer;

    fseek(repertoires, 0, SEEK_SET);
    while((fread(&buffer, sizeof(struct repertoire), 1, repertoires) != 0) && !existe){
        ++(*position);
        if(strcmp(buffer.chemin, chemin) == 0){
            existe = true;
        }
    }

    return existe;
}

bool repertoireParentExiste(char *chemin, FILE *repertoires){
    bool existe = false;
    char repertoireParent[MAX_CHEMIN + 1];
    char nom[MAX_CHEMIN + 1];
    int position = 0;

    lireNom(nom, chemin);
    lireRepertoireParent(repertoireParent, chemin, nom);
    existe = repertoireExiste(repertoireParent, repertoires, &position);

    return existe;
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

    return;
}

void lireNom(char *nom, char *chemin){
    char *ptrNom;

    ptrNom = strrchr(chemin, '/') + 1;
    strcpy(nom, ptrNom);

    return;
}

void ecritureFichier(FILE *disque, FILE *inodes, char **fragments, struct inode *inode, int tab[]){
    fseek(disque, 0, SEEK_SET);
    for(int i = 0; i < inode->nbFragments; ++i){
        struct bloc *fragment = calloc(1, sizeof(struct bloc));
        if(fragment == NULL){
            perror("Erreur d'allocation de memoire : ");
            exit(EXIT_FAILURE);
        }
        strcpy(fragment->contenu, fragments[i]);
        if(i < NB_BLOCS){
            inode->blocs[i] = prochainBlocLibre(tab);
            fseek(disque, inode->blocs[i] * NB_OCTETS, SEEK_SET);
            fwrite(fragment, sizeof(struct bloc), 1, disque);
        }else{
            inode->indirect.blocs[i - NB_BLOCS] = prochainBlocLibre(tab);
            fseek(disque, inode->indirect.blocs[i - NB_BLOCS] * NB_OCTETS, SEEK_SET);
            fwrite(fragment, sizeof(struct bloc), 1, disque);
        }
        free(fragment);
    }
    fseek(inodes, 0, SEEK_END);
    fwrite(inode, sizeof(struct inode), 1, inodes);

    return;
}

char ** fragmenterContenu(const char *contenu, struct inode *inode){
    int taille = strlen(contenu) - 1;
    int nbFragments = divisionPlafond(taille, NB_OCTETS);
    inode->nbFragments = nbFragments;
    int reste = taille % (NB_OCTETS);

    char **fragments;
    fragments = calloc(nbFragments, sizeof(char*));
    if(fragments == NULL){
        perror("Erreur d'allocation de memoire : ");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < nbFragments; ++i){
        fragments[i] = calloc(NB_OCTETS, sizeof(char));
        if(&fragments[i] == NULL){
            perror("Erreur d'allocation de memoire : ");
            exit(EXIT_FAILURE);
        }
        if(i == nbFragments - 1){
            memcpy(fragments[i], contenu + ((NB_OCTETS) * i), reste);
            strcpy(&fragments[i][reste], "\0");;
        }else{
            memcpy(fragments[i], contenu + ((NB_OCTETS) * i) , NB_OCTETS);
        }
    }

    return fragments;
}

int divisionPlafond(int num, int den){
    int resultat;

    if(num % den != 0){
        resultat = (num / den) + 1;
    }else{
        resultat = num / den;
    }

    return resultat;
}

int prochainBlocLibre(int tab[]){
    int i = 0;
    while(testerBit(tab, i) == 1 && i < TAB_BITS){
        ++i;
    }
    activerBit(tab, i);

    if(i == TAB_BITS){
        fprintf(stderr, "Le disque est plein: arrêt du programe.\n");
        exit(EXIT_FAILURE);
    }

    return i;
}

void libererBlocs(int tab[], struct inode *inode){
    for(int i = 0; i < inode->nbFragments; ++i){
        if(i < NB_OCTETS){
            desactiverBit(tab, inode->blocs[i]);
        }else{
            desactiverBit(tab, inode->indirect.blocs[i - NB_OCTETS]);
        }
    }

    return;
}

void activerBit(int tab[],  int index){
    tab[index / TAILLE_INT] |= 1 << (index % TAILLE_INT);
    return;
}

void desactiverBit(int tab[],  int index){
    tab[index / TAILLE_INT] &= ~(1 << (index % TAILLE_INT));
    return;
 }

int testerBit(int tab[],  int index){
    return ((tab[index / TAILLE_INT] & (1 << (index % TAILLE_INT))) != 0);
}
