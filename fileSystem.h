struct repertoire {
    char chemin[41];
};

struct inode {
    int id;
    char nom[41];
};

void creationFicher(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs);

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs);

void creationRepertoire(FILE *operations, FILE *repertoires);

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *inodes, FILE *blocs);

void lireFichier(FILE *operations, FILE *repertoires, FILE *inodes);

bool cheminAbsoluAcceptable(char *chemin, FILE *repertoires);

void lireChemin(FILE *operations, char *chemin);

void lireRepertoireParent(char *parent, char *chemin, char *nom);

bool repertoireParentExiste(char *chemin, FILE *repertoires);

bool fichierExiste(char *chemin, FILE *inodes);

bool repertoireExiste(char *chemin, FILE *repertoires);

// char* nomFichier(char *chemin);
