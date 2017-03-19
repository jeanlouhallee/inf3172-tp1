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


// ---------- //
// Constantes //
//------------//

#define TAILLE_DISQUE 512000 // Taille du fichier utilise pour le disque en octets
#define MAX_OPERATION 23 // Longueur maximale d'une chaine de caracteres representant une operation
#define MAX_CHEMIN 41 // Longueur maximale d'un chemin absolu
#define MAX_CONTENU 256 // Longueur maximale du contenu d'un fichier
#define NB_OCTETS 16 // Nombre d'octets par bloc
#define NB_BLOCS 8 // Nombre de blocs que peut contenir un inode ou un bloc d'indirection simple
#define TAB_BITS 32000 // Nombre de bits dans la table de bits
#define TAB_INT 1000 // Nombre d'entiers utilises pour la table de bits
#define TAILLE_INT 32 // Taille d'un entier en octets
#define FICHIER_DISQUE "disque" // Nom du fichier utilise pour le disque
#define FICHIER_REPERTOIRES "repertoires" // Nom du fichier contenant la liste les des repertoires
#define FICHIER_INODES "inodes" // Nom du fichier contenant les inodes
#define FICHIER_BLOCS "blocs" // Nom du fichier contenant la table des blocs libres


//-----------------------//
// Structures de donnees //
//-----------------------//

/*
 * Structure representant un repertoire
 */
struct repertoire {
    char chemin[MAX_CHEMIN + 1];
};

/*
 * Struture representant un bloc d'indirection simple
 */
struct indirection {
    int blocs[NB_BLOCS];
};

/*
 * Structure representant un i-node
 */
struct inode {
    char nom[MAX_CHEMIN + 1];
    int nbFragments;
    int blocs[NB_BLOCS];
    struct indirection indirect;
};

/*
 * Structure representant un bloc
 */
struct bloc {
    char contenu[NB_OCTETS];
};


// ---------- //
// Prototypes //
//------------//

/*
 * Cree le repertoire racine s'il n'existe pas deja
 *
 * @param repertoires : fichier contenant la liste des repertoires existants
 *
 * @return void
 */
void creerRepertoireRacine(FILE *repertoires);

/*
 * Lis les operations a partir du fichier d'operations et les execute
 *
 * @param operations : fichier contenant les operations a executer
 * @param  disque : fichier utilise pour le disque
 * @param repertoires : fichier contenant la liste des repertoires existants
 * @param inodes : fichier contenant la liste des i-nodes
 * @param tab : table de bits indiquant les blocs libres
 *
 * @return void
 */
void lectureOperations(FILE *operations, FILE *disque, FILE *repertoires, FILE *inodes, int *tab);

/*
 * Charge la table de bits a partir d'un fichier
 *
 * @param tab : table de bits indiquant les blocs libres
 * @param blocs : fichier contenant la table de bits
 *
 * @return void
 */
void chargerTableBits(int tab[], FILE *blocs);

/*
 * Sauvegarde la table de bits dans un fichier
 *
 * @param tab : table de bits indiquant les blocs libres
 * @param blocs : fichier contenant la table de bits
 *
 * @return void
 */
void sauvegarderTableBits(int tab[], FILE *blocs);

/*
 * Cree un fichier
 *
 * @param disque : fichier utilise pour le disque
 * @param repertoires : fichier contenant la liste des repertoires existants
 * @param inodes : fichier contenant la liste des i-nodes
 * @param tab : table de bits indiquant les blocs libres
 * @param nom : nom du fichier a creer
  * @param contenu : contenu du fichier a creer
 *
 * @return void
 */
void creationFicher(FILE *disque, FILE *repertoires, FILE *inodes, int tab[], char *nom, char *contenu);

/*
 * Supprime un fichier
 *
 * @param disque : fichier utilise pour le disque
 * @param inodes : fichier contenant la liste des i-nodes
 * @param tab : table de bits indiquant les blocs libres
  * @param nom : nom du fichier a supprimer
 *
 * @return void
 */
void suppressionFichier(FILE *disque, FILE *inodes, int tab[], char *nom);

/*
 * Cree un repertoire
 *
 * @param repertoires : fichier contenant la liste des repertoires existants
  * @param chemin : chemin du repertoire a creer
 *
 * @return void
 */
void creationRepertoire(FILE *repertoires, char *chemin);

/*
 * Supprime un repertoire et son contenu
 *
 * @param repertoires : fichier contenant la liste des repertoires existants
 * @param inodes : fichier contenant la liste des i-nodes
 * @param disque : fichier utilise pour le disque
 * @param tab : table de bits indiquant les blocs libres
 * @param chemin : chemin du repertoire a supprimer
 *
 * @return void
 */
void suppressionRepertoire(FILE *repertoires, FILE *inodes, FILE *disque, int tab[], char *chemin);

/*
 * Supprime le contenu d'un repertoire
 *
 * @param repertoires : fichier contenant la liste des repertoires existants
 * @param inodes : fichier contenant la liste des i-nodes
 * @param disque : fichier utilise pour le disque
 * @param tab : table de bits indiquant les blocs libres
 * @param chemin : chemin du repertoire a supprimer
 *
 * @return void
 */
void suppressionContenu(FILE *repertoires, FILE *inodes, FILE *disque, int tab[], char *chemin);

/*
 * Affiche le contenu d'un fichier
 *
 * @param repertoires : fichier contenant la liste des repertoires existants
 * @param inodes : fichier contenant la liste des i-nodes
 * @param disque : fichier utilise pour le disque
 * @param nom : nom du fichier a lire
 *
 * @return void
 */
void lireFichier(FILE *repertoires, FILE *inodes, FILE *disque, char *nom);

/*
 * Lit un chemin absolu a partir d'un fichier
 *
 * @param operations : fichier contenant les operations a effectuer
 * @param chemin : chemin absolu du fichier ou du repertoire
 *
 * @return vrai si un chemin absolu valide a ete lu
 *         faux si le chemin absolu est trop long
 */
bool lireChemin(FILE *operations, char *chemin);

/*
 * Extrait le nom d'un fichier ou d'un repertoire a partir de son chemin absolu
 *
 * @param nom : nom du fichier ou du repertoire
 * @param chemin : chemin absolu du fichier ou du repertoire
 *
 * @return void
 */
void lireNom(char *nom, char *chemin);

/*
 * Extrait le chemin absolu du repertoire parent d'un fichier ou d'un repertoire
 *
 * @param parent : chemin absolu du repertoire parent
 * @param chemin : chemin absolu du fichier ou du repertoire
 * @param nom : nom du fichier ou du repertoire
 *
 * @return void
 */
void lireRepertoireParent(char *parent, char *chemin, char *nom);

/*
 * Lit une chaine de caracteres a partir d'un fichier
 *
 * @param operations : fichier contenant les operations a effectuer
 * @param contenu : contenu du fichier
 *
 * @return vrai si le contenu du fichier est valide
 *         faux si le fichier est vide ou trop volumineux
 */
bool lireContenu(FILE *operations, char *contenu);

/*
 * Verifie si le fichier existe deja dans le systeme
 *
 * @param chemin : chemin absolu du fichier
 * @param inodes : fichier contenant la liste des i-nodes
 * @param inode : l'inode representant le fichier
 * @param position : position de l'inode dans le fichier d'inodes
 *
 * @return vrai si le fichier existe
 *         faux sinon
 */
bool fichierExiste(char *chemin, FILE *inodes, struct inode *inode, int *position);

/*
 * Verifie si le repertoire existe deja dans le systeme
 *
 * @param chemin : chemin absolu du repertoire
 * @param repertoires : fichier contenant la liste des repertoires existants
 * @param position : position du repertoire dans le fichier de repertoires
 *
 * @return vrai si le repertoire existe
 *         faux sinon
 */
bool repertoireExiste(char *chemin, FILE *repertoires, int *position);

/*
 * Verifie si le repertoire parent d'un fichier ou d'un repertoire existe deja dans le systeme
 *
 * @param chemin : chemin absolu du repertoire
 * @param repertoires : fichier contenant la liste des repertoires existants
 *
 * @return vrai si le repertoire parent existe
 *         faux sinon
 */
bool repertoireParentExiste(char *chemin, FILE *repertoires);



/*
 * Ecrit sur le disque
 *
 * @param disque : fichier utilise pour le disque
 * @param inodes : fichier contenant la liste des i-nodes
 * @param fragments : les fragments de texte a ecrire sur le disque
 * @param inode : l'inode pour le fichier qu'on ecris
 * @param tab : table de bits indiquant les blocs libres
 *
 * @return void
 */
void ecritureFichier(FILE *disque, FILE *inodes, char **fragments, struct inode *inode, int tab[]);

/*
 * Effectue une division et retourne le plafond du resultat
 *
 * @param num : numerateur
 * @param den : denominateur
 *
 * @return un entier, le plafond du resultat de la divison
 */
int divisionPlafond(int num, int den);

/*
 * Fragmente le contenu d'un fichier en blocs de 16 octets
 *
 * @param contenu : contenu du fichier
 *
 * @return un tableau de chaines de caracteres, le contenu de chaque bloc
 */
char ** fragmenterContenu(const char *contenu, struct inode *inode);

/*
 * Verifie si le disque est plein
 *
 * @param tab : table de bits indiquant les blocs libres
 *
 * @return void
 */
void disqueEstPlein(int tab[]);

/*
 * Cherche l'index du prochain bloc libre
 *
 * @param tab : table de bits indiquant les blocs libres
 *
 * @return un entier, l'index du prochain bloc libre
 */
int prochainBlocLibre(int tab[]);

/*
 *
 *
 * @param tab : table de bits indiquant les blocs libres
 * @param inode :
 *
 * @return void
 */
void libererBlocs(int tab[], struct inode *inode);


/*
 * Met le bit a 1
 *
 * @param tab : table de bits indiquant les blocs libres
 * @param index : index du bit dans la table
 *
 * @return void
 */
void  activerBit(int tab[],  int index);

/*
 * Met le bit a 0
 *
 * @param tab : table de bits indiquant les blocs libres
 * @param index : index du bit dans la table
 *
 * @return void
 */
void  desactiverBit(int tab[],  int index);

/*
 * Verifie la valeur d'un bit dans une table
 *
 * @param tab : table de bits indiquant les blocs libres
 * @param index : index du bit dans la table
 *
 * @return un entier, la valeur du bit
 */
int testerBit(int tab[],  int index);
