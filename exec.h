#ifndef  EXEC_INC
#define  EXEC_INC

#include "utilities.h"
#include "jobs2.h"

/* Fonction launch_process
 * Entrées :
 * 		- process *p : le processus à lancer
 * 		- pid_t pgid : id du groupe de processus associé
 * 		- int infile, outfile, errfile : redirection des flux standards
 *		- bool foreground : indique si le processus doit être lancé au premier plan
 *
 * Lance le processus p avec les pipes correspondants.
 */

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, bool foreground);

/* Fonction launch_process
 * Entrées :
 * 		- job *j : le job à lancer
 * 		- bool foreground : indique si le job doit être lancé au premier plan
 *
 * Met en place les pipes, et lance les différents processus qui composent le job j.
 */
void launch_job(job *j, bool foreground);

/*  Fontion launch
 *	Entrées :
 *		- char *buffer : la chaine entrée contenant la commande à exécuter
 *
 *	Parse la chaîne entrée selon ARGU_SEP et détermine si la commande entrée est une commande interne, sinon essaye d'exécuter un fichier à ce nom
 */
void launch(char *buffer);

/*  Fontion relaunch
 *	Entrées :
 *		- int line : la ligne de l'historique à exécuter
 *
 * Appelle launch avec la ligne numéro line de l'historique
 */
void relaunch(int line);

#endif
