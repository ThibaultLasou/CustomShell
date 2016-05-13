#include "exec.h"

void makePaths(char **paths, char *exec, char ***finalPaths, int nbPaths)
{
	int i;
	*finalPaths = malloc(sizeof(char *)*nbPaths);
	for(i=0;i<nbPaths;i++)
	{
		(*finalPaths)[i] = malloc(sizeof(char)*(strlen(paths[0])+strlen(exec)+1));
		strcpy((*finalPaths)[i], paths[i]);
		strcat((*finalPaths)[i], "/");
		strcat((*finalPaths)[i], exec);
	}
}

void execute(char **newArgv)
{
	pid_t pid;
	int res, i, nbPaths;
	char **paths;
	char **finalPaths;
	nbPaths = parser(getenv("PATH"), &paths, ':');
	makePaths(paths, newArgv[0], &finalPaths, nbPaths);
	pid = fork();
	switch(pid)
	{
		case -1 :
			fprintf(stderr, "Erreur de forkation\n");
			exit(EXIT_FAILURE);
			break;
		case 0 :
			// Processus fils
			i = 0;
			do // On essaye tous les chemins possibles
			{
				res = execv(newArgv[0], newArgv);
				newArgv[0] = finalPaths[i];
				i++;
			}while(res == -1 && i<nbPaths);
			fprintf(stderr, "%s\n", strerror(errno));
			for(i=0;i<nbPaths;i++)
			{
				free(paths[i]);
				free(finalPaths[i]);
			}
			exit(EXIT_FAILURE);
		default :
			// Processus père
			waitpid(pid, NULL, 0);
			break;
	}
}

void launch(FILE* hist, char *buffer)
{
	char **newArgv;
	int newArgc, l, i;
	pid_t pid;
	newArgc = parser(buffer, &newArgv, ' ');
	if((newArgv[0])[0] == '!')
	{
		l = atoi(&((newArgv[0])[1]));
		relaunch(hist, l);
	}
	else
	{
		fprintf(hist, buffer);
		replaceTilde(newArgv);
		if(strcmp("cd", newArgv[0]) == 0)
		{
			cd(newArgv[1]);
		}
		else if(strcmp("tail", newArgv[0]) == 0)
		{
			tail(newArgv, newArgc);
		}
		else if(strcmp("cat", newArgv[0]) == 0)
		{
			cat(newArgv, newArgc);
		}
		else if(strcmp("exit", newArgv[0]) == 0)
		{
			exit(EXIT_SUCCESS);
		}
		else if(strcmp("history", newArgv[0]) == 0)
		{
			history(newArgv[1]);
		}
		else if(strcmp("touch", newArgv[0]) == 0)
		{
			touch(newArgv, newArgc);
		}
		else if(strcmp("cp", newArgv[0]) == 0)
		{
			cp(newArgv, newArgc);
		}
		else
		{
			fflush(hist);
			execute(newArgv);
		}
	}
	for(i=0;i<newArgc;i++)
	{
		free(newArgv[i]);
	}
	free(newArgv);
}

void relaunch(FILE *histo, int line)
{
	char buffer[BUF_SIZE];
	int i = 0;
	fseek(histo, 0, SEEK_SET);
	while(fgets(buffer, BUF_SIZE, histo) != NULL)
	{
		i++;
		if(i == line)
		{
			fseek(histo, 0, SEEK_END);
			printf("%s\n", buffer);
			launch(histo, buffer);
			return;
		}
	}
}
