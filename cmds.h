#ifndef  CMDS_INC
#define  CMDS_INC

#include "utilities.h"

extern FILE *hist;

enum {HISTORY, CAT, TOUCH, TAIL, CP};

extern char cmdsPath[5][PATH_MAX]; /* Variable globale, chemin vers les commandes */

/*  Fonction cd
 *  Entrées :
 *		- char *path : le chemin auquel on veut accéder
 *
 *	Change le répertoire.
 */
void cd(char *path);

/*  Fonction history
 *  Entrées :
 *		- char *arg : le nombre de lignes à afficher en partant de la fin (optionnel)
 *
 *	Affiche l'historique ou les n dernières lignes de l'historique
 *		 */
void history(char *arg);

/*  Fonction makeCmdsPath
 *
 *  Initialise cmdsPath
 */
bool makeCmdsPath(char *launchPath);
#endif
