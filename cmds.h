#ifndef  CMDS_INC
#define  CMDS_INC

#include "utilities.h"

extern char *histPath; /* Variable globale, chemin vers le fichier d'historique */

/*  Fonction touch
 *	Entrées : 
 *		- char **args : tableau d'arguments
 *		- int argc : nombre d'arguments
 *
 *	Met à jour les dates d'accès et de modification du fichier passé en argument.
 *  Si le fichier n'existe pas il sera crée. touch -d fichier affiche 
 *  les dernières dates du fichier et ne modifie rien.
 */
void touch(char **args, int argc);

/*  Fonction cat
 *	Entrées : 
 *		- char **args : tableau d'arguments
 *		- int argc : nombre d'arguments
 *
 *	Affiche les 10 dernières lignes des fichiers passés en argument. 
 *  Avec l'argument -n=K les K dernières lignes sont affichées.
 *  Sans argument, tail lit stdin et l'écrit dans stdout
 */
void tail(char **args, int argc);

/*  Fonction cat
 *	Entrées : 
 *		- char **args : tableau d'arguments
 *		- int argc : nombre d'arguments
 *
 *	Affiche à la suite les fichiers passés en argument. 
 *  Avec l'argument -n les numéros de lignes sont affichés.
 *  Sans argument, cat lit stdin et l'écrit dans stdout
 */
void cat(char **args, int argc);

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
#endif
