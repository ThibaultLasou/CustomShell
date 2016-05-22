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

void execute(char **newArgv, struct pipeInfo pi)
{
	pid_t pid;
	int res, i, nbPaths;
	char **paths;
	char **finalPaths;
	nbPaths = parser(getenv("PATH"), &paths, PATH_SEP);
	makePaths(paths, newArgv[0], &finalPaths, nbPaths);

	if(strcmp("touch", newArgv[0]) == 0)
	{
		free(newArgv[0]);
		newArgv[0] = cmdsPath[TOUCH]; 
	}
	else if(strcmp("tail", newArgv[0]) == 0)
	{
		free(newArgv[0]);
		newArgv[0] = cmdsPath[TAIL]; 
	}
	else if(strcmp("cat", newArgv[0]) == 0)
	{
		free(newArgv[0]);
		newArgv[0] = cmdsPath[CAT]; 
	}
	else if(strcmp("cp", newArgv[0]) == 0)
	{
		free(newArgv[0]);
		newArgv[0] = cmdsPath[CP]; 
	}
	if(pi.piped == true)
	{
		if(pi.place != LAST)
		{
			close(pi.pipeOutfd[0]);
			dup2(pi.pipeOutfd[1], STDOUT_FILENO);
		}
		if(pi.place != FIRST)
		{
			close(pi.pipeInfd[1]);
			dup2(pi.pipeInfd[0], STDIN_FILENO);
		}
	}
	i = 0;
	do // On essaye tous les chemins possibles
	{
		res = execv(newArgv[0], newArgv);
		newArgv[0] = finalPaths[i];
		i++;
	}while(res == -1 && i<nbPaths);
	for(i=0;i<nbPaths;i++)
	{
		free(paths[i]);
		free(finalPaths[i]);
	}
	exit(EXIT_FAILURE);
}

void launch(char *buffer, struct pipeInfo pi)
{
	char **newArgv;
	int newArgc, i;
	pid_t pid;

	newArgc = parser(buffer, &newArgv, ARGU_SEP);
	replaceTilde(newArgv);
	if(strcmp("cd", newArgv[0]) == 0)
	{
		cd(newArgv[1]);
	}
	else if(strcmp("exit", newArgv[0]) == 0 || strcmp("quit", newArgv[0]) == 0)
	{
		exit(EXIT_SUCCESS);
	}
	else if(strcmp("history", newArgv[0]) == 0)
	{
		history(newArgv[1]);
	}
	else if(strcmp("fg", newArgv[0]) == 0)
	{
		fg(atoi(newArgv[1]));
	}
	else if(strcmp("bg", newArgv[0]) == 0)
	{
		bg(atoi(newArgv[1]));
	}
	else if(strcmp("jobs", newArgv[0]) == 0)
	{
		jobs();
	}

	else
	{
		pid = fork();
		switch(pid)
		{
			case -1 :
				fprintf(stderr, "Erreur de forkation\n");
				break;
			case 0 : 
				execute(newArgv, pi);
				break;
			default :
				// Processus père
				initJob(buffer, pid);
				if(pi.piped == true)
				{
					close(pi.pipeOutfd[1]);
				}
				waitpid(pid, NULL, 0);
				break;
		}
	}

	for(i=0;i<newArgc;i++)
	{
		if(newArgv[i] != cmdsPath[TAIL] && 
			newArgv[i] != cmdsPath[TOUCH] && 
			newArgv[i] != cmdsPath[CAT])
		{
			free(newArgv[i]);
		}
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
	int nbRedir, nbPipes, redirFile, i, l;
	char **redir, **pipes, **redirPath, pipeBuffer;
	struct pipeInfo pi;
	int defStdin, defStdout;
	pid_t pid;

	if(buffer[0] == '!')
	{
		l = atoi(&(buffer[1]));
		relaunch(hist, l);
	}
	else
	{
		fprintf(hist, buffer);
		fflush(hist);
		nbRedir = parser(buffer, &redir, REDI_SEP);
		nbPipes = parser(redir[0], &pipes, PIPE_SEP);
		if(nbPipes > 1 || nbRedir > 1)
		{
			pi.piped = true;
			pid = fork();
			switch(pid)
			{
				case 0:	
					for(i=0;i<nbPipes;i++)
					{
						//on garde une copie des flux standards
						defStdin = dup(STDIN_FILENO);
						defStdout = dup(STDOUT_FILENO);
						if(i != 0) // si ce n'est pas la première commande, on doit récupérer les résultats des programmes précédents
						{
							pipe(pi.pipeInfd);
							while(read(pi.pipeOutfd[0], &pipeBuffer, 1)>0)
							{
								write(pi.pipeInfd[1], &pipeBuffer, 1);// on transfert les résultats précédents dans le nouveau pipe pour parler au fils suivant
							}
							// on ferme les entrées de pipe dont on n'a plus besoin
							close(pi.pipeInfd[1]);
							close(pi.pipeOutfd[0]);
						}
						if(i != nbPipes -1)
						{
							pipe(pi.pipeOutfd); //on ouvre le nouveau pipe de résultat
						}
						//si c'est la dernière partie de la commande
						else if(nbRedir > 1) // redirection dans un fichier
						{
							parser(redir[1], &redirPath, ARGU_SEP);
							redirFile = creat(redirPath[0], S_IRWXU);
							dup2(redirFile, STDOUT_FILENO);
							close(redirFile);
						}
						else // on remet stdout en sortie
						{
							dup2(defStdout, STDOUT_FILENO);
						}
						if(i == 0)
						{
							pi.place = FIRST;
						}
						else if(i == nbPipes - 1)
						{
							pi.place = LAST;
						}
						else
						{
							pi.place = MID;
						}
						launch(pipes[i], pi);
					}
					dup2(defStdin, STDIN_FILENO);
					dup2(defStdout, STDOUT_FILENO);
					exit(EXIT_SUCCESS);
					break;
				case -1 :
					fprintf(stderr, "Erreur de forkation\n");
					break;
				default : 
					initJob(buffer, pid);
					waitpid(pid, NULL, 0);
			}
		}
		else
		{
			pi.piped = false;
			launch(pipes[0], pi);
		}
	}
}
