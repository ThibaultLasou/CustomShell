#ifndef  UTILITIES_INC
#define  UTILITIES_INC

#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <utime.h>
#include <limits.h>

#define BUF_SIZE 512
#define ARGU_SEP ' '
#define REDI_SEP '>'
#define PATH_SEP ':'
#define PIPE_SEP '|'

extern FILE *hist;

enum {HISTORY, CAT, TOUCH, TAIL, CP};

extern char cmdsPath[5][PATH_MAX]; /* Variable globale, chemin vers les commandes */

/*  Fonction parser
 *	Entrées : 
 *		- char *buffer : la chaine de caracteres à parser.
 *		- char ***elem : tableau contenant les éléments résultants.
 *						 Alloué dynamiquement par la fonction.
 *		- char sep : le caractère séparateur
 *	Sorties : le nombre de chaînes obtenues.
 *
 *	Divise une chaîne de caractères en plusieurs chaînes selon un caractère
 *	séparateur
 */
int parser(char *buffer, char ***elem, char sep);

/*  Fonction setEnvironnement
 *
 *  S'assure que les variables d'environnement utilisées existent, sinon les initialisent
 */
void setEnvironnement();

/*  Fonction replaceTilde
 *  Entrées
 *		- char **args : les chaînes de caractères où il faut remplacer ~
 *
 *  Remplace les caractères ~ en début de chaîne par le contenu de la variable d'environnement HOME
 */
void replaceTilde(char **args);

/*  Fonction printPrefix
 *
 *  Affiche l'invite de commandes 
 */
void printPrefix();

/*  Fonction : makePaths
 *	Entrées : 
 *			- char **paths : tableau qui contient les différents chemins du path
 *			- char *exec	: nom du programme à lancer
 *			- char ***finalPaths : tableau qui contiendra les différents chemins possibles
 *			- int nbPaths : taille du tableau paths
 *	
 *	Crée les différents chemins possibles pour exec 
 */
void makePaths(char **paths, char *exec, char ***finalPaths, int nbPaths);
#endif
