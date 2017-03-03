struct repertoire {
    char chemin[41];
};

struct inode {
    int id;
};

void creationFicher(FILE *operations, FILE *repertoires, FILE *blocs);

void suppressionFichier(FILE *operations, FILE *repertoires, FILE *blocs);

void creationRepertoire(FILE *operations, FILE *repertoires);

void suppressionRepertoire(FILE *operations, FILE *repertoires, FILE *blocs);

void lireFichier(FILE *operations, FILE *repertoires);

bool cheminAbsoluAcceptable(char *chemin, FILE *repertoires, struct repertoire *r);

void lireCheminAbsolu(FILE *operations, FILE *repertoires, char *chemin, char *nom);

bool fichierExisteDeja(char *chemin, FILE *repertoires, struct repertoire *r);

char* nomFichier(char *chemin);
