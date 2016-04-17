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
#include <fcntl.h>
#include <stdbool.h>


#define BUF_SIZE 512

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
void afficherContenuFichier(FILE* file, bool num);


#endif
