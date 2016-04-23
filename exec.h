#ifndef  EXEC_INC
#define  EXEC_INC

#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "utilities.h"

/*  Fonction : makePath
 *	Entrées : 
 *			- char **paths : tableau qui contient les différents chemins du path
 *			- char *exec	: nom du programme à lancer
 *			- char ***finalPaths : tableau qui contiendra les différents chemins possibles
 *	
 *	Crée les différents chemins possibles pour exec 
 */
void makePaths(char **paths, char *exec, char ***finalPaths, int nbPaths);

/*  Fonction : execute
 *	Entrées : 
 *			- char **newArgv : tableau qui contient les arguments pour execv
 *	
 *	Crée un nouveau processus et exécute le programme passé dans la case 0 du tableau 
 *	avec les arguments dans les autres cases.
 */
void execute(char **newArgv);

/*  Fontion launch
 *	Entrées :
 *		- FILE *hist : le fichier contenant l'historique
 *		- char *buffer : la chaine entrée contenant la commande à exécuter
 *
 *	Parse la chaîne entrée selon ' ' et détermine si al commande entrée est une commande interne, sinon essaye d'exécuter un fichier à ce nom
 */
void launch(FILE* hist, char *buffer);

/*  Fontion relaunch
 *	Entrées :
 *		- FILE *hist : le fichier contenant l'historique
 *		- int line : la ligne de l'historique à exécuter
 *
 * Appelle launch avec la ligne numéro line	
 */
void relaunch(FILE *histo, int line);
#endif
