#ifndef  CMDS_INC
#define  CMDS_INC

#include "utilities.h"
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
 *	Affiche à la suite les fichiers passés en argument. 
 *  Avec l'argument -n les numéros de lignes sont affichés.
 *  Sans argument , cat lit stdin et l'écrit dans stdout
 */
void cat(char **args, int argc);
void cd(char *path);
void history(FILE *histo);
#endif
