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

void execute(char **newArgv, bool piped, int place, int pipeInfd[2], int pipeOutfd[2])
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
			printf("piped : %d, place : %d\n", piped, place);
			if(piped == true)
			{
				if(place != LAST)
				{
					printf("Fermeture read sortie et stdout = write sortie\n");
					close(pipeOutfd[0]);
					dup2(pipeOutfd[1], STDOUT_FILENO);
				}
				if(place != FIRST)
				{
					printf("Fermeture write entrée et stdin = read entrée\n");
					close(pipeInfd[1]);
					dup2(pipeInfd[0], STDIN_FILENO);
				}
			}
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
			if(piped == true)
			{
				close(pipeOutfd[1]);
			}
			// Processus père
			waitpid(pid, NULL, 0);
			break;
	}
}

void launch(FILE* hist, char *buffer, bool piped, int place, int pipeInfd[2], int pipeOutfd[2])
{
	char **newArgv;
	int newArgc, l, i;
	pid_t pid;
	newArgc = parser(buffer, &newArgv, ARGU_SEP);
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
			execute(newArgv, piped, place, pipeInfd, pipeOutfd);
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
			setPipe(histo, buffer);
			return;
		}
	}
}

void setPipe(FILE* hist, char *buffer)
{
	int nbRedir, nbPipes, redirFile, i;
	char **redir, **pipes, **redirPath, *pipeBuffer[BUF_SIZE];
	int pipeInfd[2];
	int pipeOutfd[2];
	int place = FIRST;
	int defStdin, defStdout;
	bool piped = false;

	nbRedir = parser(buffer, &redir, REDI_SEP);
	nbPipes = parser(redir[0], &pipes, PIPE_SEP);
	if(nbPipes > 1 || nbRedir > 1)
	{
		piped = true;
		printf("Piped\n");
	}
	for(i=0;i<nbPipes;i++)
	{
		if(piped == true)
		{
			defStdin = dup(STDIN_FILENO);
			defStdout = dup(STDOUT_FILENO);
			if(i != 0)
			{
				printf("Ouverture nouveau pipe entrée\n");
				pipe(pipeInfd);
				printf("Transfert\n");
				while(read(pipeOutfd[0], pipeBuffer, BUF_SIZE)>0)
				{
					write(pipeInfd[1], pipeBuffer, BUF_SIZE);
				}
				close(pipeInfd[1]);
				close(pipeOutfd[0]);
			}
			if(i != nbPipes -1)
			{
				pipe(pipeOutfd);
				printf("Ouverture nouveau pipe sortie\n");
			}
			else if(nbRedir > 1)
			{
				parser(redir[1], &redirPath, ARGU_SEP);
				redirFile = creat(redirPath[0], S_IRWXU);
			}
			else
			{
				printf("Re stdout\n");
				dup2(defStdout, STDOUT_FILENO);
			}
			if(i == 0)
			{
				place = FIRST;
			}
			else if(i == nbPipes - 1)
			{
				place = LAST;
			}
			else
			{
				place = MID;
			}
			printf("%d\n", place);
		}
		launch(hist, pipes[i], piped, place, pipeInfd, pipeOutfd);
	}
	if(piped == true)
	{
		dup2(defStdin, STDIN_FILENO);
	}
}
