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

void printPrefix(struct utsname machin)
{
	if(strncmp(getenv("PWD"), getenv("HOME"), strlen(getenv("HOME"))) == 0)
	{
		printf("%s@%s:~%s >> ", getenv("LOGNAME"), machin.nodename, getenv("PWD")+strlen(getenv("HOME")));
	}
	else
	{
		printf("%s@%s:%s >> ", getenv("LOGNAME"), machin.nodename, getenv("PWD"));
	}
}

int main(int argc, char **argv, char *envp[])
{
	char buffer[BUF_SIZE];
	char *loginName, *machineName;
	char **newArgv;
	int newArgc, res;
	pid_t pid;
	struct utsname machin;
	FILE *hist;

	uname(&machin);
	hist = fopen("history","a+");
	printPrefix(machin);
	while(fgets(buffer, BUF_SIZE, stdin) != NULL)
	{
		fprintf(hist, buffer);
		makeCmd(buffer, &newArgv);
		if(strcmp("cd", newArgv[0]) == 0)
		{
			cd(newArgv[1]);
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
			pid = fork();
			switch(pid)
			{
				case -1 :
					printf("Erreur de forkation\n");
					exit(EXIT_FAILURE);
					break;
				case 0 :
					/* Processus fils */
					res = execv(newArgv[0], newArgv);
					if(res == -1)
					{
						printf("%s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
					exit(EXIT_SUCCESS);
				default :
					waitpid(pid, NULL, 0);
					break;
			}
		}
		printPrefix(machin);
	}
	printf("\n");
	return 0;
}
