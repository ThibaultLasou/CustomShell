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
			fprintf(stderr, "touch: %s : %s\n" , args[i], strerror(errno));
		}
	}
	else if(argc == 2)
	{
		/* Dans le cas où : touch -d */
		if(strcmp(args[1],"-d") == 0)
		{
			fprintf(stderr, "touch: opérande de fichier manquant\n");			
		}
		/* Dans le cas où : touch fichierQuiN'existePas */
		else if(fopen(args[1], "r") == NULL)
		{
			if(fopen(args[1], "w") == NULL) // creer le fichier 
			{
				fprintf(stderr, "touch: %s : %s\n" , args[1], strerror(errno));
			}
		}
		/* Dans le cas où : touch fichierExistant */
		else
		{
			t.modtime = time(0);
			t.actime = time(0);
			if(utime(args[1], &t) == -1)
			{
				fprintf(stderr, "touch: %s : %s\n" , args[1], strerror(errno));
			}
		}
	}
	/* Dans le cas où touch sans arguments */
	else if(argc == 1)
	{
		fprintf(stderr, "touch: opérande de fichier manquant\n");
	}
}

void tail(char **args, int argc)
{
	int i = 1;
	int j = 0;
	int nbLine = 10;
	char line[BUF_SIZE];
	FILE *file = NULL;
	int *size, sum = 0, num = 0;

	if(argc>1)
	{
		if(strncmp(args[1],"-n=", 3) == 0)
		{
			nbLine = atoi(&((args[1])[3]));
			i++;
		}
		else if(strncmp(args[argc-1],"-n=", 3) == 0)
		{
			nbLine = atoi(&((args[argc-1])[3]));
			argc--;
		}
	}
	do
	{
		num = 0;
		sum = 0;
		if((args[1] == NULL) || /* tail sans arguments */
				//	(nbLine != -1 && args[2] == NULL) || /* tail -n=int  */
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
				fprintf(stderr, "tail: %s : %s\n" , args[i], strerror(errno));
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
	do
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
				fprintf(stderr, "cat: %s : %s\n" , args[i], strerror(errno));
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
		fprintf(stderr, "%s\n", strerror(errno));
	}
	/* Changer la variable d'environnement*/
	WD = getcwd(NULL, 0);
	setenv("PWD", WD, 1);
	free(WD);
}


void history(char *arg)
{
	char *args[3];
	if(arg == NULL)
	{
		args[0] = "cat";
		args[1] = "-n";
		args[2] = histPath;
		cat(args, 3);
	}
	else
	{
		args[0] = "tail";
		args[1] = malloc(sizeof(char)*(strlen(arg)+4));
		sprintf(args[1],"-n=%s", arg);
		args[2] = histPath;
		tail(args, 3);
		free(args[1]);
	}
}
