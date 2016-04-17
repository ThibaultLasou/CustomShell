#ifndef  UTILITIES_INC
#define  UTILITIES_INC

#include "cmds.h"

/*  Fonction parser
 *	Entrées : 
 *		- char *buffer : la chaine de caracteres à parser.
 *		- char ***elem : tableau contenant les éléments résultants.
 *						 Alloué dynamiquement par la fonction.
 *		- char sep : le caractère séparateur
 *	Sorties : le nombre de chaînes obtenues.
 *
 *	Divise une chaîne de caractères en plusieurs chaînes selon un caractère séparateur
 *			
 */
int parser(char *buffer, char ***elem, char sep);

#endif
