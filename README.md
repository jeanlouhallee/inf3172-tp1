# Travail pratique 1

## Description

Simulation d'un système de fichiers.

Dans le cadre du cours Système d'exploitation (INF3172).

Université du Québec à Montréal, hiver 2017.

## Auteurs

Étienne Bergeron (BERE08089101)

Jean-Lou Hallée (HALJ05129309)

## Fonctionnement

Le programme doit être lancé à la console avec un fichier en paramètre. Ce fichier doit contenir les opérations à effectuer par le système de fichier.

Les opérations possibles sont :

* `creation_repertoire <nom du répertoire>` : Permet la création d'un répertoire.
* `suppression_repertoire <nom du répertoire>` : Permet de supprimer un répertoire et tout son contenu.
* `creation_fichier <nom du fichier> <contenu du fichier>` : Permet de créer un fichier dans le système.
* `suppression_fichier <nom du fichier>` : Permet de supprimer un fichier.
* `lire_fichier <nom du fichier>` : Permet d'afficher le contenu du fichier à l'écran.


## Contenu du projet

* fileSystem.h : Fichier d'entête où sont définis les constantes, les structures et les prototypes.
* fileSystem.c : Fichier source du projet contenant le code.
* Makefile : Fichier générant la compilation à l'aide de la commande "make" et nettoyage des fichiers non désirés à l'aide de la commande "make clean".
* README.md : Fichier décrivant la totalité du projet.

# Références

* [Stack Overflow - How to define and work with an array of bits in C?](http://stackoverflow.com/questions/2525310/how-to-define-and-work-with-an-array-of-bits-in-c) : Utilisé comme référence pour manipuler une table de bits.

# Statut

Projet complété : aucun bogue reconnu pour l'instant.