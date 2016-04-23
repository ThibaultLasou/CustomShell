#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#define BUFFER_SIZE 512 

char *makePath(char *dir, char *file) // cree les chemins d'accès corrects
{
	char *res;
	if(dir[strlen(dir)-1] != '/')
	{
		res = malloc(sizeof(char)*(strlen(dir)+strlen(file)+1));
		sprintf(res, "%s/%s", dir, file);
	}
	else
	{
		res = malloc(sizeof(char)*(strlen(dir)+strlen(file)));
		sprintf(res, "%s%s", dir, file);
	}
	return res;
}

int fileCopy(int ori, int copy, struct stat statOri)
{
	char buf[BUFFER_SIZE];
	int n;

	fchmod(copy, statOri.st_mode);
	while((n = read(ori, buf, BUFFER_SIZE)) > 0)
	{
		if(write(copy, buf, n) != n)
		{
			return -1;
		}
	}		
	return 1;
}

int copieDoDansDo(char *source, char *dest, struct stat statOri)
{
	DIR* dp;
	struct dirent* d;
	char *arg[3];

	dp = opendir(source);
	if(dp == NULL)
	{
		return 0;
	}

	struct stat stbufdir;

	while((d = readdir(dp)) != NULL)
	{
		if((d->d_ino != 0) && (!(strncmp(d->d_name, ".", 1) == 0 || strncmp(d->d_name, "..", 2) == 0)))
		{
			arg[1] = makePath(source, d->d_name); 
			arg[2] = makePath(dest, d->d_name); 

			if(cp(3, arg) != 1)
			{
				return 0;
			}

			free(arg[1]);
			free(arg[2]);
		}
	}
	closedir(dp);
	return 1;
}

int copieFiDansFi(char *ori, char *cpy, struct stat statOri)
{
	int fdOri, fdCpy;
	int res;

	fdOri = open(ori, O_RDONLY);
	if(fdOri == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", ori);
		return 0;
	}

	fdCpy = open(cpy, O_WRONLY | O_CREAT | O_TRUNC, statOri.st_mode);
	if(fdCpy == -1)
	{
		fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", cpy);
		close(fdOri);
		return 0;
	}

	res = fileCopy(fdOri, fdCpy, statOri);

	if(close(fdOri) == -1)
	{
		return 0;
	}
	if(close(fdCpy) == -1)
	{
		return 0;
	}	
	if(res == -1)
	{
		return 0;
	}
	return 1;
}

int copieFiDansDo(char *ori, char *cpy, struct stat statOri)
{
	int res;
	char *nomFichier, *cheminCpy;
	nomFichier = strrchr(ori, (int) '/');
	
	if(nomFichier != NULL)
	{
		cheminCpy = makePath(cpy, &(nomFichier[1]));
	}
	else
	{
		cheminCpy = makePath(cpy, ori);
	}
	res = copieFiDansFi(ori, cheminCpy, statOri);
	free(cheminCpy);
	return res;
}

int cp(char *argv[], int argc)
{
	if(argc < 3)
	{
		fprintf(stderr, "Nombre d'arguments incorrects\n");
		return 0;
	}

	struct stat statOri;
	struct stat statCpy;

	if(stat(argv[1], &statOri) != -1) // on accède au contenu à copier
	{
		if(S_ISDIR(statOri.st_mode))
		{
			if(stat(argv[2], &statCpy) == -1)
			{
				if(mkdir(argv[2], statOri.st_mode) == -1)
				{
					fprintf(stderr, "Impossible de créer le dossier %s\n", argv[2]);
					return 0;
				}
				copieDoDansDo(argv[1], argv[2], statOri);
			}
			else if(S_ISDIR(statCpy.st_mode))
			{
				copieDoDansDo(argv[1], argv[2], statOri);
			}
			else
			{
				fprintf(stderr, "Impossible de copier un dossier dans un fichier\n");
			}
		}
		else
		{
			if((stat(argv[2], &statCpy) == -1) || S_ISREG(statCpy.st_mode))
			{
				copieFiDansFi(argv[1], argv[2], statOri);
			}
			else
			{
				copieFiDansDo(argv[1], argv[2], statOri);
			}
		}
	}
	else
	{
		fprintf(stderr, "Impossible d'acceder à %s\n", argv[1]);
		return 0;
	}
	return 1;
}
