#include "exec.h"

void makePaths(char ***paths, char *exec)
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
