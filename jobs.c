#include "jobs.h"

void fg(pid_t pid)
{
	
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
	struct sigaction int_handler;
	int_handler.sa_handler=handle_int};
	int_handler.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT;
	sigaction(SIGINT, &int_handler,0);
}

void handle_int(int num)
{
	uint8_t hasKilled = 0;
	if(hasCurrentChildID)
	{
		kill(childID[currentChildID].pid, SIGINT);
		hasKilled = 1;
	}

	if(!hasKilled)
		interrupt=1;
	printf("\n");
}
