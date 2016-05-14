#ifndef  EXEC_INC
#define  EXEC_INC

#include "utilities.h"

enum {FIRST = 0, MID, LAST};

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

/*  Fonction : execute
 *	Entrées : 
 *			- char **newArgv : tableau qui contient les arguments pour execv
 *	
 *	Crée un nouveau processus et exécute le programme passé dans la case 0 du tableau 
 *	avec les arguments dans les autres cases.
 */
void execute(char **newArgv, bool piped, int place, int pipeInfd[2], int pipeOutfd[2]);

/*  Fontion launch
 *	Entrées :
 *		- FILE *hist : le fichier contenant l'historique
 *		- char *buffer : la chaine entrée contenant la commande à exécuter
 *
 *	Parse la chaîne entrée selon ' ' et détermine si al commande entrée est une commande interne, sinon essaye d'exécuter un fichier à ce nom
 */
void launch(FILE* hist, char *buffer, bool piped, int place, int pipeInfd[2], int pipeOutfd[2]);

/*  Fontion relaunch
 *	Entrées :
 *		- FILE *hist : le fichier contenant l'historique
 *		- int line : la ligne de l'historique à exécuter
 *
 * Appelle launch avec la ligne numéro line	de l'historique
 */
void relaunch(FILE *histo, int line);

void setPipe(FILE* hist, char *buffer);
#endif
