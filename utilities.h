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

#include <time.h>
#include <utime.h>

#define BUF_SIZE 512

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

/*  Fonction afficherContenuFichier
 *	Entrées : 
 *		- FILE *file : le fichier à afficher.
 *		- bool num : num de ligne on/off
 *
 *	Affiche dans stdout le fichier file. La lecture commence depuis la pos
 *	actuelle du pointeur de lecture dans le fichier.
 */
void afficherContenuFichier(FILE *file, bool num);

void setEnvironnement();
#endif
