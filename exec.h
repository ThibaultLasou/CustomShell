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

/*  Fonction : makePath
 *	Entrées : 
 *			- char ***paths : tableau qui contiendra les différents chemins possibles
 *			- char *exec	: nom du programme à lancer
 *	// Sortie  : le nombre de chemins
 *	
 *	Crée les différents chemins possibles avec la variable d'environnement PATH.
 */
void makePaths(char ***paths, char *exec);

/*  Fonction : execute
 *	Entrées : 
 *			- char **newArgv : tableau qui contient les arguments pour execv
 *	
 *	Crée un nouveau processus et exécute le programme passé dans la case 0 du tableau 
 *	avec les arguments dans les autres cases.
 */
void execute(char **newArgv);

#endif
