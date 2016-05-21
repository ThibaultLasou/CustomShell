#include "cmds.h"

/*  Fonction tail 
 *	Entrées : 
 *		- char **args : tableau d'arguments
 *		- int argc : nombre d'arguments
 *
 *	Affiche les 10 dernières lignes des fichiers passés en argument. 
 *  Avec l'argument -n=K les K dernières lignes sont affichées.
 *  Sans argument, tail lit stdin et l'écrit dans stdout
 */
int main(int argc, char **argv)
{
	int i = 1;
	int j = 0;
	int nbLine = 10;
	char line[BUF_SIZE];
	FILE *file = NULL;
	int *size, sum = 0, num = 0;

	if(argc>1)
	{
		if(strncmp(argv[1],"-n=", 3) == 0)
		{
			nbLine = atoi(&((argv[1])[3]));
			i++;
		}
		else if(strncmp(argv[argc-1],"-n=", 3) == 0)
		{
			nbLine = atoi(&((argv[argc-1])[3]));
			argc--;
		}
	}
	do
	{
		num = 0;
		sum = 0;
		if((argv[1] == NULL) || /* tail sans arguments */
				//	(nbLine != -1 && argv[2] == NULL) || /* tail -n=int  */
				(strcmp(argv[i], "-") == 0)) /* f - g */
		{
			file = stdin;
		}
		else
		{
			/* Ouverture du fichier */
			file = fopen(argv[i], "r" );
			/* Si ce n'est pas un fichier ou erreur lors de l'ouverture*/
			if(file == NULL)
			{
				fprintf(stderr, "tail: %s : %s\n" , argv[i], strerror(errno));
				return;
			}
		}
		size = malloc(nbLine*sizeof(int));
		if(size == NULL)
		{
			return;
		}
		for(j=0;j<nbLine;j++)
		{
			size[j] = 0;
		}
		j = 0;

		while(fgets(line, BUF_SIZE, file) != NULL) /* lire une ligne */
		{
			size[j] = strlen(line);
			j++;
			if(j == nbLine)
			{
				j=0;
			}
			num++;
		}
		for(j=0;j<nbLine;j++)
		{
			sum += size[j];
		}
		num -= nbLine-1;
		if(num <= 0)
		{
			num = 1;
		}
		fseek(file, -(sum*sizeof(char)), SEEK_END);
		while(fgets(line, BUF_SIZE, file) != NULL) /* lire une ligne */
		{
			printf("%d\t%s", num, line);
			num++;
		}
		if(file != stdin)
		{
			/* Fermeture du ficher */
			fclose(file);
		}
		i++;
		free(size);
	}while(i < argc);
}
