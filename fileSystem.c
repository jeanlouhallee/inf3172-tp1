/*
 * TP1 : Simulation d'un systeme de fichiers
 *
 * Cours : INF3172 Systeme d'exploitation
 *
 * Auteurs : Etienne Bergeron BERE08089101
 *           Jean-Lou Hallee  HALJ05129309
 *
 * Dernières modifications : 12 mars 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileSystem.h"

int main(int argc, char *argv[]){
    char operation[MAX_OPERATION];
    int tab[TAB_INT] = {0};

    if(argc != 2){
        fprintf(stderr, "Nom de fichier manquant ou arguments en trop.\n" );
        return EXIT_FAILURE;
    }

    // Fichier d'operations donne en parametre
    FILE *operations = fopen(argv[1], "r");

    // Fichier utilise pour le disque
    FILE *disque = fopen(FICHIER_DISQUE, "r");
    if(!disque){
        disque = fopen(FICHIER_DISQUE, "wb+");
        ftruncate(fileno(disque), TAILLE_DISQUE);
    }else{
        fclose(disque);
        disque = fopen(FICHIER_DISQUE, "rb+");
    }

    // Fichier contenant une liste des repertoires
    FILE *repertoires = fopen(FICHIER_REPERTOIRES, "r");
    if(!repertoires){
        repertoires = fopen(FICHIER_REPERTOIRES, "wb+");
    }else{
        fclose(repertoires);
        repertoires = fopen(FICHIER_REPERTOIRES, "rb+");
    }

    // Fichier contenant les i-nodes
    FILE *inodes = fopen(FICHIER_INODES, "r");
    if(!inodes){
        inodes = fopen(FICHIER_INODES, "wb+");
    }else{
        fclose(inodes);
        inodes = fopen(FICHIER_INODES, "rb+");
    }

    // Fichier contenant une liste des blocs libres
    FILE *blocsCharge = fopen(FICHIER_BLOCS, "ab+");
    chargerTableBits(tab, blocsCharge);
    fclose(blocsCharge);

    creerRepertoireRacine(repertoires);

    // Lecture des operations
    while(fscanf(operations, "%s", operation) != EOF){
        if(strcmp(operation, "creation_fichier")  == 0){
            printf("creation de fichier.\n");
            creationFicher(disque, operations, repertoires, inodes, tab);
            printf("\n");
        } else if(strcmp(operation, "suppression_fichier") == 0){
            printf("suppression de fichier.\n");
            suppressionFichier(operations, repertoires, inodes, tab);
            printf("\n");
        } else if(strcmp(operation, "creation_repertoire") == 0){
            printf("creation de repertoire.\n");
            creationRepertoire(operations, repertoires);
            printf("\n");
        } else if(strcmp(operation, "suppression_repertoire") == 0){
            printf("suppression de repertoire.\n");
            suppressionRepertoire(operations, repertoires, inodes, disque, tab);
            printf("\n");
        } else if(strcmp(operation, "lire_fichier") == 0){
            printf("lecture de fichier.\n");
            lireFichier(operations, repertoires, inodes, disque);
            printf("\n");
        } else {
            fprintf(stderr, "Erreur dans le fichier d'operations." );
            return EXIT_FAILURE;
        }
    }
    printf("FIN DU PROGRAMME.\n");
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
    fseek(repertoires, 0, SEEK_END);
    if(ftell(repertoires) == 0){
        struct repertoire *r = malloc(sizeof(struct repertoire));
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
        resultat = (num / den) + 1;
    }else{
        resultat = num / den;
    }

    return resultat;
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

void lireNom(char *nom, char *chemin){
    char *ptrNom;

    ptrNom = strrchr(chemin, '/') + 1;
    strcpy(nom, ptrNom);
    //printf("Nom : %s\n", nom);

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
    //printf("Repertoire parent : %s\n", parent);

    return;
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

bool lireChemin(FILE *operations, char *chemin){
    bool estOK = true;

    fscanf(operations, "%41s", chemin);
    //printf("Chemin : %s\n", chemin);

    if(strlen(chemin) >= MAX_CHEMIN){
        fprintf(stderr, "Chemin absolu trop long.\n");
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
        fprintf(stderr, "Le fichier ne peut pas etre vide.\n");
        fseek(operations, -(strlen(contenu)), SEEK_CUR);
        return estOK = false;
    } else if(strlen(contenu) > MAX_CONTENU){
        fprintf(stderr, "Contenu du fichier trop long.\n");
        fscanf(operations, "%*[^\n]");
        return estOK = false;
    }
    printf("Contenu : %s", contenu);

    return estOK;
}


int prochainBlocLibre(int tab[]){
    int i = 0;
    while(testBit(tab, i) == 1 && i < TAB_BITS){
        ++i;
    }
    setBit(tab, i);

    if(i == TAB_BITS){
        fprintf(stderr, "Le disque est plein: arrêt du programe.\n");
        exit(EXIT_FAILURE);
    }

    return i;
}


void ecritureFichier(FILE *disque, FILE *inodes, char **fragments, struct inode *inode, int tab[]){
    fseek(disque, 0, SEEK_SET);
    inode->blocs[0] = inode->id;
    struct bloc *fragment = calloc(1, sizeof(struct bloc));
    strcpy(fragment->contenu, fragments[0]);
    fseek(disque, inode->blocs[0] * NB_OCTETS, SEEK_SET);
    fwrite(fragment, sizeof(struct bloc), 1, disque);
    free(fragment);

    for(int i = 1; i < inode->nbFragments; ++i){
        struct bloc *fragment = calloc(1, sizeof(struct bloc));
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

    for(int i = 0; i < nbFragments; ++i){
        fragments[i] = calloc(NB_OCTETS, sizeof(char));
        if(i == nbFragments - 1){
            memcpy(fragments[i], contenu + ((NB_OCTETS) * i), reste);
            strcpy(&fragments[i][reste], "\0");;
        }else{
            memcpy(fragments[i], contenu + ((NB_OCTETS) * i) , NB_OCTETS);
        }
    }

    return fragments;
}


void creationFicher(FILE *disque, FILE *operations, FILE *repertoires, FILE *inodes, int tab[]){
    char nom[MAX_CHEMIN + 1];
    char contenu[MAX_CONTENU + 1];
    bool cheminOk, fichierOk, repertoireParentOk = false;
    int position = 0;

    cheminOk = lireChemin(operations, nom);
    fichierOk = !fichierExiste(nom, inodes, NULL, &position);
    repertoireParentOk = repertoireParentExiste(nom, repertoires);

    if(lireContenu(operations, contenu) && cheminOk && fichierOk && repertoireParentOk){
        struct inode *i = calloc(1, sizeof(struct inode));
        char ** fragments = fragmenterContenu(contenu, i);
        i->id = prochainBlocLibre(tab);
        strcpy(i->nom, nom);
        ecritureFichier(disque, inodes, fragments, i, tab);
        for(int j = 0; j < i->nbFragments; ++j){
            free(fragments[j]);
        }
        free(fragments);
        free(i);
    }else if(!fichierOk && cheminOk){
        fprintf(stderr, "Le fichier existe deja.\n");
    } else if(!repertoireParentOk && cheminOk){
        fprintf(stderr, "Le repertoire n'existe pas.\n");
    }

    return;
}

void libererBlocs(int tab[], struct inode *inode){
    for(int i = 0; i < inode->nbFragments; ++i){
        if(i < NB_OCTETS){
            clearBit(tab, inode->blocs[i]);
        }else{
            clearBit(tab, inode->indirect.blocs[i - NB_OCTETS]);
        }
    }

    return;
}

void suppression(FILE *disque, FILE *inodes, int tab[], char *nom){
    struct inode *inode = malloc(sizeof(struct inode));
    int position = 0;

    if(fichierExiste(nom, inodes, inode, &position)){
        libererBlocs(tab, inode);
        struct inode *inodeVide = malloc(sizeof(struct inode));
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

void suppressionFichier(FILE *operations, FILE *disque, FILE *inodes, int tab[]){
    char nom[MAX_CHEMIN + 1];

    if(lireChemin(operations, nom)){
        suppression(disque, inodes, tab, nom);
    }

    return;
}

void creationRepertoire(FILE *operations, FILE *repertoires){
    char chemin[MAX_CHEMIN + 1];
    int position = 0;

    if(lireChemin(operations, chemin)){
        if(repertoireParentExiste(chemin, repertoires)){
            if(!repertoireExiste(chemin, repertoires, &position)){
                struct repertoire *r = malloc(sizeof(struct repertoire));
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
            suppression(disque, inodes, tab, i.nom);
            fsetpos(inodes, &position);
        }
    }
    fseek(repertoires, 0, SEEK_SET);
    while((fread(&r, sizeof(struct repertoire), 1, repertoires)) != 0){
        if(strlen(r.chemin) >= longueur && memcmp(r.chemin, chemin, longueur) == 0){
            fgetpos(repertoires, &position);
            suppressionRecursive(repertoires, inodes, disque, tab, r.chemin);
            fsetpos(repertoires, &position);
        }

    }
    return;
}

void suppressionRecursive(FILE *repertoires, FILE *inodes, FILE *disque, int tab[], char *chemin){
    int position = 0;

    if(repertoireExiste(chemin, repertoires, &position)){
        struct repertoire *r = malloc(sizeof(struct repertoire));
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

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *inodes, FILE *disque, int tab[]){
    char chemin[MAX_CHEMIN + 1];

    if(lireChemin(operations, chemin)){
        suppressionRecursive(repertoires, inodes, disque, tab, chemin);
    }
    return;
}

void lireFichier(FILE *operations, FILE *repertoires, FILE *inodes, FILE *disque){
    char nom[MAX_CHEMIN + 1];
    struct inode *inode = calloc(1, sizeof(struct inode));
    int position = 0;

    if(lireChemin(operations, nom)){
        if(fichierExiste(nom, inodes, inode, &position)){
            for(int i = 0; i < inode->nbFragments; ++i){
                struct bloc *fragment = calloc(1, sizeof(struct bloc));
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
    }
    free(inode);
    return;
}

void setBit(int tab[],  int index){
    tab[index / TAILLE_INT] |= 1 << (index % TAILLE_INT);
    return;
}

int testBit(int tab[],  int index){
    return ((tab[index / TAILLE_INT] & (1 << (index % TAILLE_INT))) != 0);
}

void clearBit(int tab[],  int index){
    tab[index / TAILLE_INT] &= ~(1 << (index % TAILLE_INT));
    return;
 }
