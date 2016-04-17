#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cmds.h"
#include "exec.h"

/*
 * Affichage de l'invite de commande
 */
void printPrefix(struct utsname machine)
{
	if(strncmp(getenv("PWD"), getenv("HOME"), strlen(getenv("HOME"))) == 0)
	{
		printf("%s@%s:~%s >> ", getenv("LOGNAME"), machine.nodename, getenv("PWD")+strlen(getenv("HOME")));
	}
	else
	{
		printf("%s@%s:%s >> ", getenv("LOGNAME"), machine.nodename, getenv("PWD"));
	}
}
/*
 * */
int main(int argc, char **argv, char *envp[])
{
	char buffer[BUF_SIZE];
	char *loginName, *histPath;
	char **newArgv;
	int newArgc, res;
	pid_t pid;
	struct utsname machine;
	FILE *hist;
/*  int i;
	for(i=0;envp[i]!=NULL;i++)
	{
		printf("%s\n", envp[i]);
	}
*/

	// Ouverture du fichier d'historique
	histPath = malloc(sizeof(char)*(strlen(getenv("HOME"))+strlen("/.history")+1));
	sprintf(histPath,"%s/.history", getenv("HOME"));
	hist = fopen(histPath,"a+");
	
	uname(&machine);
	
	printPrefix(machine);
	while(fgets(buffer, BUF_SIZE, stdin) != NULL)
	{
		if(buffer[0]!='\n')
		{
			fprintf(hist, buffer);
			newArgc = parser(buffer, &newArgv, ' ');
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
		printPrefix(machine);
	}
	printf("\n");

	return 0;
}
