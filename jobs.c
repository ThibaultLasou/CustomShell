#include "jobs.h"

void fg(int id)
{
	if(id < nbJobs)
	{
		fgJob = 		
	}
}

void jobs()
{
	int i;
	for(i=0;i<nbJobs;i++)
	{
		printf("[%d] ");
		if(jobsList[i].status == RUNNING)
		{
			printf("En cours d'éxécution");
		}
		else if(jobsList[i].status == STOPPED)
		{
			printf("Stoppé");
		}
		printf("\t %s\n", jobsList[i].cmd);
	}
}

void initSigHandle()
{
	// SIGINT : Ctrl+C
	struct sigaction interHandler;
	interHandler.sa_handler = interruptHandler;
	interHandler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGINT, &interHandler, 0);

	// SIGTSTP : Ctrl+Z
	struct sigaction stopHandler;
	stopHandler.sa_handler = stopHandler;
	stopHandler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGTSTP, &stopHandler, 0);

	// SIGCHLD
	struct sigaction chldHandler;
	chldHandler.sa_handler = childHandler;
	chldHandler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGCHLD, &childHandler, 0);
}

void interruptHandler(int num)
{
	if(fgJob != -1)
	{
		kill(jobsList[fgJob].pid, SIGINT);
	}
	else
	{
		exit(EXIT_SUCCESS); // à voir
	}
	printf("\n");
}

void stopHandler(int num)
{
	
}
