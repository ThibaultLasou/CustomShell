#include "jobs.h"

t_job jobsList[MAX_JOB];
int nbJobs = 0;
int fgJob = -1;

void initJob(char *buffer, int pid)
{
	jobsList[nbJobs].cmd = strdup(buffer);
	jobsList[nbJobs].pid = pid;
	jobsList[nbJobs].status = RUNNING;
	fgJob = nbJobs;
	printf("%d\n",jobsList[fgJob].pid);
	nbJobs++;
}

void fg(int id)
{
	if(id < nbJobs)
	{
		fgJob = id;		
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
	struct sigaction stpHandler;
	stpHandler.sa_handler = stopHandler;
	stpHandler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGTSTP, &stpHandler, 0);

	// SIGCHLD
	struct sigaction chldHandler;
	chldHandler.sa_handler = childHandler;
	chldHandler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGCHLD, &chldHandler, 0);
}

void interruptHandler(int num)
{
	fprintf(stderr, "gauffration");
	if(fgJob != -1)
	{
		fprintf(stderr, "gauffre");
		printf("%d",jobsList[fgJob].pid);
		kill(jobsList[fgJob].pid, num);
	}
	else
	{
		fprintf(stderr, "gauffrette");
		exit(EXIT_SUCCESS); // à voir
	}
	printf("\n");
}

void stopHandler(int num)
{
	if(fgJob != -1 && jobsList[fgJob].status == RUNNING)
	{
		kill(jobsList[fgJob].pid, num);
	}
	else
	{
		kill(getpid(), num);
	}
	printf("\n");
}

void childHandler(int num)
{

}
