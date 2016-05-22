#include "jobs.h"

t_job jobsList[MAX_JOB];
int nbJobs = 0;
int fgJob = -1;

struct sigaction defaultInt;
struct sigaction defaultStp;

void initJob(char *buffer, int pid)
{
	jobsList[nbJobs].cmd = strdup(buffer);
	jobsList[nbJobs].pid = pid;
	jobsList[nbJobs].status = RUNNING;
	fgJob = nbJobs;
	nbJobs++;
}

void destrJob(int id)
{
	free(jobsList[id].cmd);
	if(id == fgJob)
	{
		fgJob = -1;
	}
	if(id != nbJobs-1)
	{
		decalage(id);
	}
	nbJobs--;
}

void decalage(int id)
{
	int i;
	for(i=id;i<nbJobs-1;i++)
	{
		jobsList[i].cmd = jobsList[i+1].cmd;
		jobsList[i].pid = jobsList[i+1].pid;
		jobsList[i].status = jobsList[i+1].status;
	}
}

void fg(int id)
{
	if(id < nbJobs)
	{
		if(jobsList[id].status == STOPPED)
		{
			kill(jobsList[id].pid, SIGCONT);
			jobsList[fgJob].status = RUNNING;
		}
		fgJob = id;
		waitpid(jobsList[id].pid, NULL, WCONTINUED);
	}
}

void bg(int id)
{
	if(jobsList[id].status == STOPPED)
	{
		kill(jobsList[id].pid, SIGCONT);
		jobsList[fgJob].status = RUNNING;
	}
}

void jobs()
{
	int i;
	for(i=0;i<nbJobs;i++)
	{
		printf("[%d] ", i+1);
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
	struct sigaction interHandler;
	interHandler.sa_handler = sigHandler;
	interHandler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGINT, &interHandler, &defaultInt);
	sigaction(SIGTSTP, &interHandler, &defaultStp);

	// SIGCHLD
	struct sigaction chldHandler;
	chldHandler.sa_handler = childHandler;
	chldHandler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGCHLD, &chldHandler, 0);
}

void sigHandler(int num)
{
	if(fgJob != -1 && jobsList[fgJob].status == RUNNING)
	{
		kill(jobsList[fgJob].pid, num);
		switch(num)
		{
			case SIGTSTP :
				jobsList[fgJob].status = STOPPED;
				fgJob = -1;
				break;
			case SIGINT :
				destrJob(fgJob);
				fgJob = -1;
				break;
		}
	}
	else
	{
		switch(num)
		{
			case SIGTSTP :
				defaultStp.sa_handler;
				break;
			case SIGINT :
				defaultInt.sa_handler;
				break;
		}
	}
	printf("\n");
}

void childHandler(int num)
{
	int i;
	for(i=0;i<nbJobs;i++)
	{
		if(kill(jobsList[i].pid, 0) == -1)
		{
			if(errno == ESRCH)
			{
				destrJob(i);
				if(fgJob == i)
				{
					fgJob = -1;
				}
			}
		}

	}
}
