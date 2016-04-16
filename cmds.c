#include "cmds.h"

void touch(char **args)
{

}

void cat(char **args)
{
	/* si path == NULL, lire stdin et l'afficher
	 * sinon afficher le contenu du fichier passé en paramètre
	 */
 	int i = 1;
 	int nbcarac;
	char buffer[512];
	while(args[i] != NULL)
	{	
		/* Ouverture du fichier */
		int fd = open(args[i], O_RDONLY);
		
			/* Si ce n'est pas un fichier ou erreur lors de l'ouverture*/
			if(fd == -1)
			{
				printf("cat: %s : Aucun fichier ou dossier de ce type" , args[i]);
				//fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", args[i]);
				i++;
				continue;
			}
			/* Sinon afficher le contenu du fichier */
			else
			{
				while((nbcarac = read(fd, buffer, 512)) > 0)
				{
					write(1, buffer, nbcarac); 
				}
			}
			
			/* Si erreur lors de la fermeture */
			if(close(fd) == -1)
			{
				fprintf(stderr, "Erreur fermeture %s\n", args[i]);
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
	while(fgets(buffer, BUF_SIZE, histo) != NULL)
	{
		printf("%d %s", i, buffer);
		i++;
	}
}

void makeCmd(char *buffer, char ***args)
{
	int i, finMot=0, debutMot=0, nbArgs=1, numArg = 0;
	/* Calculer le nombre d'arguments dans la ligne de commande */
	for(i=0;buffer[i]!='\n';i++)
	{
		if(buffer[i] == ' ')
		{
			nbArgs++;
		}
	}
	/* Allouer nbArgs+1 cases au tableau args */
	*args = malloc(sizeof(char *)*(nbArgs+1));
	/* Remplissage du tableau "args" : Parcourir le tableau "buffer" et copier les chaines de carateres séparées par des espaces dans le tableau "args" */
	do
	{
		for(i=debutMot;buffer[i]!='\n';i++)
		{
			if(buffer[i] == ' ')
			{
				if(buffer[i-1] == '\\')
				{
					strcpy(&(buffer[i-1]), &(buffer[i]));
				}
				else
				{
					break;
				}
			}	
		}
		finMot = i;
		(*args)[numArg] = malloc(sizeof(char)*finMot-debutMot);
		strncpy((*args)[numArg], &(buffer[debutMot]), finMot-debutMot);
		(*args)[numArg][finMot-debutMot] = '\0';
		numArg++;
		debutMot = finMot+1;
	}while(buffer[i] != '\n');
	(*args)[numArg] = NULL;
}
