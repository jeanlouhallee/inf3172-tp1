#define MAX_CHEMIN 41
#define MAX_BLOCS 16
#define MAX_OPERATION 23
#define MAX_CONTENU 256
#define FICHIER_DISQUE "disque"
#define FICHIER_REPERTOIRES "repertoires"
#define FICHIER_INODES "inodes"
#define FICHIER_BLOCS "blocs"

struct repertoire {
    char chemin[42];
};

struct inode {
    int id;
    char nom[42];
    int blocs[8];
    struct indirection *indirect;

};

struct indirection {
    int blocs[8];
};

struct bloc {
    char contenu[16];
};

void creationFicher(FILE *operations, FILE *repertoires, FILE *inodes, int *tab);

int assignerId(int *tab);

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *inodes);

void creationRepertoire(FILE *operations, FILE *repertoires);

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *inodes);

void lireFichier(FILE *operations, FILE *repertoires, FILE *inodes);

bool cheminAbsoluAcceptable(char *chemin, FILE *repertoires);

bool lireChemin(FILE *operations, char *chemin);

bool lireContenu(FILE *operations, char *contenu);

void lireRepertoireParent(char *parent, char *chemin, char *nom);

bool repertoireParentExiste(char *chemin, FILE *repertoires);

bool fichierExiste(char *chemin, FILE *inodes);

bool repertoireExiste(char *chemin, FILE *repertoires);

void creerRepertoireRacine(FILE *repertoires);

int divisionPlafond(int num, int den);

char ** fragmenterContenu(const char *contenu);

int * creerTableBits();

void  setBit(int tab[],  int index);

int testBit(int tab[],  int index);

void  clearBit(int tab[],  int index);

void chargerTableBits(int *tab, FILE *blocs);

void sauvegarderTableBits(int *tab, FILE *blocs);
