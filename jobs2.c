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

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, bool foreground)
{
	pid_t pid;
	char **paths, **finalPaths;
	int i, res, nbPaths;

	/* Put the process into the process group and give the process group
	   the terminal, if appropriate.
	   This has to be done both by the shell and in the individual
	   child processes because of potential race conditions.  */
	pid = getpid();
	if(pgid == 0)
	{
		pgid = pid;
	}	
	setpgid (pid, pgid);
	if(foreground)
	{
		tcsetpgrp (shell_terminal, pgid);
	}

	/* Set the handling for job control signals back to the default.  */
	signal (SIGINT, SIG_DFL);
	signal (SIGQUIT, SIG_DFL);
	signal (SIGTSTP, SIG_DFL);
	signal (SIGTTIN, SIG_DFL);
	signal (SIGTTOU, SIG_DFL);
	signal (SIGCHLD, SIG_DFL);

	/* Set the standard input/output channels of the new process.  */
	if(infile != STDIN_FILENO)
	{
		dup2(infile, STDIN_FILENO);
		close(infile);
	}
	if(outfile != STDOUT_FILENO)
	{
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}
	if(errfile != STDERR_FILENO)
	{
		dup2(errfile, STDERR_FILENO);
		close(errfile);
	}    

	if(strcmp("touch", p->argv[0]) == 0)
	{
		free(p->argv[0]);
		p->argv[0] = cmdsPath[TOUCH]; 
	}
	else if(strcmp("tail", p->argv[0]) == 0)
	{
		free(p->argv[0]);
		p->argv[0] = cmdsPath[TAIL]; 
	}
	else if(strcmp("cat", p->argv[0]) == 0)
	{
		free(p->argv[0]);
		p->argv[0] = cmdsPath[CAT]; 
	}
	else if(strcmp("cp", p->argv[0]) == 0)
	{
		free(p->argv[0]);
		p->argv[0] = cmdsPath[CP]; 
	}

	i = 0;
	nbPaths = parser(getenv("PATH"), &paths, PATH_SEP);
	makePaths(paths, p->argv[0], &finalPaths, nbPaths);
	do // On essaye tous les chemins possibles
	{
		res = execv(p->argv[0], p->argv);
		p->argv[0] = finalPaths[i];
		i++;
	}while(res == -1 && i<nbPaths);
	if(res == -1)
	{
		fprintf(stderr, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void launch_job(job *j, bool foreground)
{
	process *p;
	pid_t pid;
	int mypipe[2], infile, outfile;

	infile = j->in;
	for(p = j->first_process; p; p = p->next)
	{
		/* Set up pipes, if necessary.  */
		if(p->next)
		{
			if(pipe (mypipe) < 0)
			{
				perror("pipe");
				exit(1);
			}
			outfile = mypipe[1];
		}
		else
		{
			outfile = j->out;
		}
		/* Fork the child processes.  */
		replaceTilde(p->argv);
		if(strcmp("cd", p->argv[0]) == 0)
		{
			cd(p->argv[1]);
		}
		else if(strcmp("exit", p->argv[0]) == 0 || strcmp("quit", p->argv[0]) == 0)
		{
			exit(EXIT_SUCCESS);
		}
		else if(strcmp("history", p->argv[0]) == 0)
		{
			history(p->argv[1]);
		}
		else if(strcmp("fg", p->argv[0]) == 0)
		{
			fg(atoi(p->argv[1]));
		}
		else if(strcmp("bg", p->argv[0]) == 0)
		{
			bg(atoi(p->argv[1]));
		}
		else if(strcmp("jobs", p->argv[0]) == 0)
		{
			jobs();
		}
		else
		{
			pid = fork();
			if(pid == 0) 
			{
				/* This is the child process.  */
				launch_process (p, j->pgid, infile,	outfile, j->err, foreground);
			}
			else if(pid < 0)
			{
				/* The fork failed.  */
				fprintf(stderr, "%s", strerror(errno));
				exit(EXIT_FAILURE);
			}
			else
			{
				/* This is the parent process.  */
				p->pid = pid;
				if(j->pgid == 0)
				{
					j->pgid = pid;
				}
				setpgid(pid, j->pgid);
				printf("child pid : %d, group pid : %d\n", pid, j->pgid);
			}
		}
		/* Clean up after pipes.  */
		if(infile != STDIN_FILENO)
		{
			close(infile);
		}
		if(outfile != STDOUT_FILENO)
		{
			close(outfile);
		}
		infile = mypipe[0];
	}

	if(foreground)
	{
		printf("TOP\n");
		put_job_in_foreground(j, false);
	}
	else
	{
		put_job_in_background(j, false);
	}
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
	
	printf("POP : %s %d\n", j->command, j->first_process->pid);
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
		printf("WAAITING FOR %d\n", j->pgid);
		pid = waitpid(-j->pgid, &status, 0);

		if(pid == -1)
		{
			fprintf(stderr, "AAA%s\n", strerror(errno));
		}
		printf("POP : %d\n", pid);
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
	process *p;

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
		put_job_in_foreground (j, 1);
	else
		put_job_in_background (j, 1);
}

