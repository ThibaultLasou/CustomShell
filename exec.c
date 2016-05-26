#include "exec.h"

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
		put_job_in_foreground(j, false);
	}
	else
	{
		put_job_in_background(j, false);
	}
}

void relaunch(int line)
{
	char buffer[BUF_SIZE];
	int i = 0;
	fseek(hist, 0, SEEK_SET);
	while(fgets(buffer, BUF_SIZE, hist) != NULL)
	{
		i++;
		if(i == line)
		{
			fseek(hist, 0, SEEK_END);
			printf("%s\n", buffer);
			launch(buffer);
			return;
		}
	}
}

void launch(char *buffer)
{
	bool foreground;
	job *j;
	if(strrchr(buffer, (int) '&') == &(buffer[strlen(buffer)-2]))
	{
		buffer[strlen(buffer)-2] = '\n';
		buffer[strlen(buffer)-1] = '\0';
		foreground = false;
	}
	else
	{
		foreground = true;
	}
	j = addJob(buffer);
	launch_job(j, foreground);
	do_job_notification();
}
