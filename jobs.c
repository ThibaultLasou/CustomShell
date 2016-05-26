void jobs()
{
	int i;
	for(i=0;i<nbJobs;i++)
	{
		printf("[%d] ", i+1);
		if(jobsList[i].status == RUNNING)
		{
			printf("En cours d'exécution");
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
