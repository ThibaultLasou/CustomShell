#include "cmds.h"

/*  Fonction touch
 *	Entrées : 
 *		- char **args : tableau d'arguments
 *		- int argc : nombre d'arguments
 *
 *	Met à jour les dates d'accès et de modification du fichier passé en argument.
 *  Si le fichier n'existe pas il sera crée. touch -d fichier affiche 
 *  les dernières dates du fichier et ne modifie rien.
 */
int main(int argc, char **argv)
{
	struct stat sb;
	struct utimbuf t;
	int i = 1;

	/* touch avec l'option -d : affichage du dernier accès au fichier ainsi que la dernière modification */
	if (argc == 3)
	{
		/* placer le i en fonction de l'emplacement de l'option -d*/
		if(strcmp(argv[1], "-d") == 0)
		{
			i++;
		}
		if(stat(argv[i],&sb) != -1)
		{
			printf("Dernier accès au fichier :         %s", ctime(&sb.st_atime));
			printf("Dernière modification du fichier : %s", ctime(&sb.st_mtime));
		}
		else
		{
			fprintf(stderr, "touch: %s : %s\n" , argv[i], strerror(errno));
		}
	}
	else if(argc == 2)
	{
		/* Dans le cas où : touch -d */
		if(strcmp(argv[1],"-d") == 0)
		{
			fprintf(stderr, "touch: opérande de fichier manquant\n");			
		}
		/* Dans le cas où : touch fichierQuiN'existePas */
		else if(fopen(argv[1], "r") == NULL)
		{
			if(fopen(argv[1], "w") == NULL) // creer le fichier 
			{
				fprintf(stderr, "touch: %s : %s\n" , argv[1], strerror(errno));
			}
		}
		/* Dans le cas où : touch fichierExistant */
		else
		{
			t.modtime = time(0);
			t.actime = time(0);
			if(utime(argv[1], &t) == -1)
			{
				fprintf(stderr, "touch: %s : %s\n" , argv[1], strerror(errno));
			}
		}
	}
	/* Dans le cas où touch sans arguments */
	else if(argc == 1)
	{
		fprintf(stderr, "touch: opérande de fichier manquant\n");
	}
}
