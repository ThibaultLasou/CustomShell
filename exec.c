#include "exec.h"

void makePaths(char **paths, char *exec, char ***finalPaths)
{
	/* c'est un peu comme makeCmd */
}

void execute(char **newArgv)
{
	pid_t pid;
	int res, i;
	char **paths;
	pid = fork();
	switch(pid)
	{
		case -1 :
			printf("Erreur de forkation\n");
			exit(EXIT_FAILURE);
			break;
		case 0 :
			// Processus fils
			i = 0;
			do // On essaye tous les chemins possibles
			{
				res = execv(newArgv[0], newArgv);
				newArgv[0] = paths[i];
				i++;
			}while(res == -1);
			printf("%s\n", strerror(errno));
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
	int newArgc;
	pid_t pid;
	newArgc = parser(buffer, &newArgv, ' ');
	if((newArgv[0])[0] == '!')
	{
		int l = atoi(&((newArgv[0])[1]));
		relaunch(hist, l);
	}
	else
	{
		fprintf(hist, buffer);
		if(strcmp("cd", newArgv[0]) == 0)
		{
			cd(newArgv[1]);
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
			history(hist);
		}
		else
		{
			fflush(hist);
			execute(newArgv);
		}
	}
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
