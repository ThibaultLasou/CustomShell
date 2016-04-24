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

#define BUF_SIZE 512

extern char *histPath; /* Variable globale, chemin vers le fichier d'historique */

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

/*  Fonction histoPath
 *
 *  Retourne le chemin du fichier d'historique
 */
char *histoPath();

/*  Fonction printPrefix
 *
 *  Affiche l'invite de commandes 
 */
void printPrefix();
#endif
