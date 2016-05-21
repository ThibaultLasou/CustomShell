#include "cmds.h"

/*  Fonction cat
 *	Entrées : 
 *		- char **args : tableau d'arguments
 *		- int argc : nombre d'arguments
 *
 *	Affiche à la suite les fichiers passés en argument. 
 *  Avec l'argument -n les numéros de lignes sont affichés.
 *  Sans argument, cat lit stdin et l'écrit dans stdout
 */
int main(int argc, char **argv)
{
	int i = 1;
	int j = 1;
	bool num = false;
	char line[BUF_SIZE];
	FILE *file = NULL;

	if(argc>1)
	{
		if(strcmp(argv[1],"-n") == 0)
		{
			num = true;
			i++;
		}
		else if(strcmp(argv[argc-1],"-n") == 0)
		{
			num = true;
			argc--;
		}
	}
	do
	{	
		if((argv[1] == NULL) || /* cat sans arguments */
				(num == true && argv[2] == NULL) || /* cat -n  */
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
				fprintf(stderr, "cat: %s : %s\n" , argv[i], strerror(errno));
				return;
			}
		}
		while(fgets(line, BUF_SIZE, file) != NULL) /* lire une ligne */
		{
			if(num == true)
			{
				printf("%d\t", j);/* afficher le numero de la ligne */
				j++;
			}
			printf("%s", line); /* ecrire la ligne  */
		}
		if(file != stdin)
		{
			/* Fermeture du ficher */
			fclose(file);
		}
		i++;
	}while(i < argc);
}
