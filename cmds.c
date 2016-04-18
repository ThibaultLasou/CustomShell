#include "cmds.h"

void touch(char **args , int argc)
{
	struct stat sb;
	struct utimbuf t;
	int i = 1;
	
	/* touch avec l'option -d : affichage du dernier accès au fichier ainsi que la dernière modification */
	if (argc == 3)
	{
		/* placer le i en fonction de l'emplacement de l'option -d*/
		if(strcmp(args[1],"-d") == 0)
		{
			i++;
		}
		stat(args[i],&sb);
		printf("Dernier accès au fichier :         %s", ctime(&sb.st_atime));
		printf("Dernière modification du fichier:  %s", ctime(&sb.st_mtime));
	}
	
	else if(argc ==2)
	{
		/* Dans le cas où : touch -d */
		if(strcmp(args[1],"-d") == 0)
		{
			printf("touch: opérande de fichier manquant\n ");			
		}
		/* Dans le cas où : touch fichierQuiN'existePas */
		else if (fopen(args[1],"r")==NULL)
		{
			fopen(args[1], "w" ); // creer le fichier 
		}

		/* Dans le cas où : touch fichierExistant */
		else
		{
			t.modtime = time(0);
			t.actime = time(0);
			utime(args[1], &t);
		}
		
	}
	/* Dans le cas où touch sans arguments */
	else if(argc == 1 )
	{
		printf("touch: opérande de fichier manquant \n ");
	}
}

void cat(char **args, int argc)
{
	int i = 1;
	int j = 1;
	bool num = false;
	char line[BUF_SIZE];
	FILE *fd = NULL;
	
	if(argc>1)
	{
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
	}
	if(i == argc)
	{
		fd = stdin;
		afficherContenuFichier(fd, num);
		return;
	}
	while(i < argc)
	{	
		/* Ouverture du fichier */
		fd = fopen(args[i], "r" );
		/* Si ce n'est pas un fichier ou erreur lors de l'ouverture*/
		if(fd == NULL)
		{
			printf("cat: %s : %s\n " , args[i], strerror(errno));
			//fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", args[i]);
		}
		/* Sinon afficher le contenu du fichier */
		else
		{
			while(fgets(line, BUF_SIZE, fd) != NULL) /* lire une ligne */
			{
				if(num == true)
				{
					printf("%d\t", j);/* afficher le numero de la ligne */
					j++;
				}
				printf("%s", line); /* ecrire la ligne  */
			}
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
	fseek(histo, 0, SEEK_SET);
	afficherContenuFichier(histo, true);
}
