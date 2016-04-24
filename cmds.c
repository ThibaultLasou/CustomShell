#include "cmds.h"

void touch(char **args, int argc)
{
	struct stat sb;
	struct utimbuf t;
	int i = 1;
	
	/* touch avec l'option -d : affichage du dernier accès au fichier ainsi que la dernière modification */
	if (argc == 3)
	{
		/* placer le i en fonction de l'emplacement de l'option -d*/
		if(strcmp(args[1], "-d") == 0)
		{
			i++;
		}
		if(stat(args[i],&sb) != -1)
		{
			printf("Dernier accès au fichier :         %s", ctime(&sb.st_atime));
			printf("Dernière modification du fichier : %s", ctime(&sb.st_mtime));
		}
		else
		{
			printf("touch: %s : %s\n" , args[i], strerror(errno));
		}
	}
	else if(argc == 2)
	{
		/* Dans le cas où : touch -d */
		if(strcmp(args[1],"-d") == 0)
		{
			printf("touch: opérande de fichier manquant\n");			
		}
		/* Dans le cas où : touch fichierQuiN'existePas */
		else if(fopen(args[1], "r") == NULL)
		{
			if(fopen(args[1], "w") == NULL) // creer le fichier 
			{
				printf("touch: %s : %s\n" , args[1], strerror(errno));
			}
		}
		/* Dans le cas où : touch fichierExistant */
		else
		{
			t.modtime = time(0);
			t.actime = time(0);
			if(utime(args[1], &t) == -1)
			{
				printf("touch: %s : %s\n" , args[1], strerror(errno));
			}
		}
	}
	/* Dans le cas où touch sans arguments */
	else if(argc == 1)
	{
		printf("touch: opérande de fichier manquant\n");
	}
}

void cat(char **args, int argc)
{
	int i = 1;
	int j = 1;
	bool num = false;
	char line[BUF_SIZE];
	FILE *file = NULL;
	
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
		file = stdin;
		afficherContenuFichier(file, num);
		return;
	}
	while(i < argc)
	{	
		if((args[1] == NULL) || /* cat sans arguments */
			(num == true && args[2] == NULL) || /* cat -n  */
			(strcmp(args[i], "-") == 0)) /* f - g */
		{
			file = stdin;
		}
		else
		{
			/* Ouverture du fichier */
			file = fopen(args[i], "r" );
			/* Si ce n'est pas un fichier ou erreur lors de l'ouverture*/
			if(file == NULL)
			{
				printf("cat: %s : %s\n" , args[i], strerror(errno));
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
	}
}

void cd(char *path)
{
	char *WD;
	/* si cd sans arguments positionner dans HOME */
	if(path == NULL)
	{
		path = getenv("HOME");
	}
	if(chdir(path) == -1)
	{
		printf("%s\n", strerror(errno));
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
