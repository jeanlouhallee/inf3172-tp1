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

void creationFicher(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs);

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs);

void creationRepertoire(FILE *operations, FILE *repertoires);

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs);

void lireFichier(FILE *operations, FILE *repertoires, FILE *inodes);

bool cheminAbsoluAcceptable(char *chemin, FILE *repertoires);

bool lireChemin(FILE *operations, char *chemin);

bool lireContenu(FILE *operations, char *contenu);

void lireRepertoireParent(char *parent, char *chemin, char *nom);

bool repertoireParentExiste(char *chemin, FILE *repertoires);

bool fichierExiste(char *chemin, FILE *inodes);

bool repertoireExiste(char *chemin, FILE *repertoires);

int * creerTableBits();

void  SetBit(int tab[],  int index);

int TestBit(int tab[],  int index);

void chargerTableBits(int *tab, FILE *blocs);

void sauvegarderTableBits(int *tab, FILE *blocs);
