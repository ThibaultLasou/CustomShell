#include "jobs2.h"

pid_t shell_pgid;
int shell_terminal;
/* The active jobs are linked into a list.  This is its head.   */
job *first_job = NULL;
struct termios shell_tmodes;

job *addJob(char *command)
{
	job *j = first_job;
	if(j != NULL)
	{
		while(j->next != NULL)
		{
			j = j->next;
		}
		j->next = createJob(command);
		return j->next;
	}
	else
	{
		first_job = createJob(command);
		return first_job;
	}
}

job *createJob(char *command)
{
	job *j;
	process *p;
	char **redirs, **fileRedirPath, **processes;
	int i, nbRedir, nbProcess, redirFile = STDOUT_FILENO;
	
	nbRedir = parser(command, &redirs, REDI_SEP);
	if(nbRedir > 1)
	{
		parser(redirs[1], &fileRedirPath, ARGU_SEP);
		redirFile = creat(fileRedirPath[0], S_IRWXU);
	}
	nbProcess = parser(redirs[0], &processes, PIPE_SEP);
	
	j = malloc(sizeof(job));
	j->next = NULL;
	j->command = command;
	j->in = STDIN_FILENO;
	j->out = redirFile;
	j->err = STDERR_FILENO;
	j->pgid = 0;
	j->notified = false;

	j->first_process = createProcess(processes[0]);
	p = j->first_process;
	for(i=1;i<nbProcess;i++)
	{
		p->next = createProcess(processes[i]);
		p = p->next;
	}
	return j;
}

process *createProcess(char *command)
{
	process *p;
	p = malloc(sizeof(process));
	p->next = NULL;
	p->completed = false;
	p->stopped = false;
	p->pid = -1;
	parser(command, &(p->argv), ARGU_SEP);
	
	return p;
}

void destrProc(process *p)
{
	int i=0;
	while(p->argv[i] != NULL)
	{
		free(p->argv[i]);
		i++;
	}
	free(p->argv);
	destrProc(p->next);
	free(p);
}

void destrJob(job *j)
{
	job *pJ = first_job;
	if(j == pJ)
	{
		first_job = NULL;
	}
	else
	{
		while(pJ->next != j)
		{
			pJ = pJ->next;
		}
		pJ->next = j->next;
	}
	free(j->command);
	destrProc(j->first_process);
	free(j);
}

/* Find the active job with the indicated pgid.  */
job *find_job(pid_t pgid)
{
	job *j;

	for(j = first_job; j; j = j->next)
	{
		if(j->pgid == pgid)
		{
			return j;
		}
	}
	return NULL;
}

/* Return true if all processes in the job have stopped or completed.  */
bool job_is_stopped(job *j)
{
	process *p;

	for(p = j->first_process; p; p = p->next)
	{	
		if(!p->completed && !p->stopped)
		{
			return false;
		}
	}
	return true;
}

/* Return true if all processes in the job have completed.  */
bool job_is_completed(job *j)
{
	process *p;

	for(p = j->first_process; p; p = p->next)
	{
		if(!p->completed)
		{
			return false;
		}
	}
	return true;
}


/* Make sure the shell is running interactively as the foreground job
   before proceeding. */

void init_shell()
{

	/* See if we are running interactively.  */
	shell_terminal = STDIN_FILENO;

	/* Ignore interactive and job-control signals.  */
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	/* Put ourselves in our own process group.  */
	shell_pgid = getpid();
	if(setpgid(shell_pgid, shell_pgid) < 0)
	{
		perror("Couldn't put the shell in its own process group");
		exit(1);
	}

	/* Grab control of the terminal.  */
	tcsetpgrp(shell_terminal, shell_pgid);

	/* Save default terminal attributes for shell.  */
	tcgetattr(shell_terminal, &shell_tmodes);
}

/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a SIGCONT signal to wake it up before we block.  */
void put_job_in_foreground(job *j, bool cont)
{
	/* Put the job into the foreground.  */
	tcsetpgrp(shell_terminal, j->pgid);

	/* Send the job a continue signal, if necessary.  */
	if(cont)
	{
		tcsetattr(shell_terminal, TCSADRAIN, &j->tmodes);
		if (kill(- j->pgid, SIGCONT) < 0)
		{
			fprintf(stderr, "%s", strerror(errno));
		}
	}

	/* Wait for it to report.  */
	wait_for_job(j);

	/* Put the shell back in the foreground.  */
	
	if(tcsetpgrp(shell_terminal, shell_pgid) == -1)
	{
		fprintf(stderr, "%s", strerror(errno));
	}

	/* Restore the shell's terminal modes.  */
	tcgetattr(shell_terminal, &j->tmodes);
	tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);
}

/* Put a job in the background.  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up.  */
void put_job_in_background(job *j, bool cont)
{
	/* Send the job a continue signal, if necessary.  */
	if(cont)
	{
		if(kill(-j->pgid, SIGCONT) < 0)
		{
			fprintf(stderr, "%s", strerror(errno));
		}
	}
}

/* Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise.  */
int mark_process_status(pid_t pid, int status)
{
	job *j;
	process *p;

	if(pid > 0)
	{
		/* Update the record for the process.  */
		for(j = first_job; j; j = j->next)
		{
			for(p = j->first_process; p; p = p->next)
			{
				if (p->pid == pid)
				{
					p->status = status;
					if(WIFSTOPPED(status))
					{	
						p->stopped = true;
					}
					else
					{
						p->completed = true;
						if(WIFSIGNALED (status))
						{
							fprintf(stderr, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG (p->status));
						}
					}
					return 0;
				}
			}
		}
		fprintf(stderr, "No child process %d.\n", pid);
		return -1;
	}
	else if(pid == 0 || errno == ECHILD)
	/* No processes ready to report.  */
	{
		return -1;
	}
}

/* Check for processes that have status information available,
   without blocking.  */
void update_status()
{
	int status;
	pid_t pid;

	do
	{
		pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
	}
	while(!mark_process_status (pid, status));
}

/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */
void wait_for_job(job *j)
{
	int status;
	pid_t pid;

	do
	{
		pid = waitpid(-j->pgid, &status, WUNTRACED);

		if(pid == -1)
		{
		//	fprintf(stderr, "%s\n", strerror(errno));
		}
	}while(!mark_process_status(pid, status) && !job_is_stopped(j) && !job_is_completed(j));
}

/* Format information about job status for the user to look at.  */
void format_job_info(job *j, const char *status)
{
	fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */
void do_job_notification()
{
	job *j, *jnext;

	/* Update status information for child processes.  */
	update_status ();

	for (j = first_job; j; j = jnext)
	{
		jnext = j->next;

		/* If all processes have completed, tell the user the job has
		   completed and delete it from the list of active jobs.  */
		if (job_is_completed (j)) {
			format_job_info (j, "completed");
			destrJob(j);
		}

		/* Notify the user about stopped jobs,
		   marking them so that we won't do this more than once.  */
		else if (job_is_stopped (j) && !j->notified) {
			format_job_info (j, "stopped");
			j->notified = 1;
		}
	}
}

/* Mark a stopped job J as being running again.  */
void mark_job_as_running (job *j)
{
	process *p;

	for (p = j->first_process; p; p = p->next)
		p->stopped = 0;
	j->notified = 0;
}

/* Continue the job J.  */
void continue_job (job *j, bool foreground)
{
	mark_job_as_running (j);
	if (foreground)
		put_job_in_foreground (j, true);
	else
		put_job_in_background (j, true);
}

void fg(int place)
{
	int i = 1;
	job *j = first_job;
	while(i != place && j != NULL)
	{
		j = j->next;
		i++;
	}
	if(j == NULL)
	{
		fprintf(stderr, "No job %d\n", place);
	}
	else
	{
		if(job_is_stopped(j))
		{
			put_job_in_foreground(j, true);
		}
		else
		{
			put_job_in_foreground(j, false);
		}
	}
}

void bg(int place)
{
	int i = 1;
	job *j = first_job;
	while(i != place && j != NULL)
	{
		j = j->next;
		i++;
	}
	if(j == NULL)
	{
		fprintf(stderr, "No job %d\n", place);
	}
	else
	{
		if(job_is_stopped(j))
		{
			put_job_in_background(j, true);
		}
		else
		{
			put_job_in_background(j, false);
		}
	}
}

void jobs()
{
	int i;
	job *j = first_job;
	for(i=1;j!=NULL;i++)
	{
		printf("[%d] ", i);
		if(job_is_completed(j))
		{
			printf("Terminé");
		}
		else if(job_is_stopped(j))
		{
			printf("Stoppé");
		}
		else
		{
			printf("En cours d'exécution");
		}
		printf("\t %s\n", j->command);
		j = j->next;
	}
}
