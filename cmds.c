#include "cmds.h"

void cd(char *path)
{
	char *WD;
	if(chdir(path) == -1)
	{
		printf("%s\n", strerror(errno));
	}
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
	int i, finMot=0, debutMot=0, nbArgs=0, numArg = 0;
	for(i=0;buffer[i]!='\n';i++)
	{
		if(buffer[i] == ' ')
		{
			nbArgs++;
		}
	}
	*args = malloc(sizeof(char *)*nbArgs+2);
	do
	{
		for(i=debutMot;buffer[i]!=' ' && buffer[i]!='\n';i++);
		finMot = i;
		(*args)[numArg] = malloc(sizeof(char)*finMot-debutMot);
		strncpy((*args)[numArg], &(buffer[debutMot]), finMot-debutMot);
		(*args)[numArg][finMot-debutMot] = '\0';
		numArg++;
		debutMot = finMot+1;
	}while(buffer[i] != '\n');
	(*args)[numArg] = NULL;
}
