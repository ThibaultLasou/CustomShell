#include "cmds.h"

void touch(char **args)
{

}

void cat(char **args, int argc)
{
	/* si path == NULL, lire stdin et l'afficher
	 * sinon afficher le contenu du fichier passé en paramètre
	 */
	int i = 1;
	bool num = false;
	if(strcmp(args[1],"-n") == 0)
	{
		num = true;
		i++;
	}
	else if(strcmp(args[argc-1],"-n") == 0)
	{
		num = true;
		argc--;
	}
	while(i < argc)
	{	
		/* Ouverture du fichier */
		FILE *fd = fopen(args[i], "r" );
		/* Si ce n'est pas un fichier ou erreur lors de l'ouverture*/
		if(fd == NULL)
		{
			printf("cat: %s : %s\n " , args[i], strerror(errno));
			//fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", args[i]);
		}
		/* Sinon afficher le contenu du fichier */
		else
		{
			afficherContenuFichier(fd, num);
			/* Fermeture du ficher */
			fclose(fd);
		}
		i++;
	}

}

void cd(char *path)
{
	char *WD;
	/* si cd sans arguments positionner dans HOME */
	if(path == NULL || path[0] == '\0')
	{
		chdir(getenv("HOME"));
	}
	/* si cd ~ positionner dans HOME */
	else if(path[0] == '~')
	{
		chdir(getenv("HOME"));
		/* si cd ~/Repertoire positionner dans Repertoire */
		if(strlen(path) > 2)
		{
			cd(&path[2]);
		}
	}
	/* si cd ~/Repertoire1/Repertoire2/...../RepertoireN positionner dans RepertoireN */
	else
	{
		if(chdir(path) == -1)
		{
			printf("%s\n", strerror(errno));
		}
	}
	/* Changer la variable d'environnement*/
	WD = getcwd(NULL, 0);
	setenv("PWD", WD, 1);
	free(WD);
}

void history(FILE *histo)
{
	char buffer[BUF_SIZE];
	int i = 0;
	fseek(histo, 0, SEEK_SET);
	afficherContenuFichier(histo, true);
}
