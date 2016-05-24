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
#include "jobs2.h"

int main(int argc, char **argv, char *envp[])
{
	char buffer[BUF_SIZE];
	FILE *hist;
	bool foreground;
	job *j;
	setEnvironnement();
//	initSigHandle();
	init_shell();
	if(!makeCmdsPath(argv[0]))
	{
		exit(EXIT_FAILURE);
	}
	hist = fopen(cmdsPath[HISTORY],"a+");
	if(hist == NULL)
	{
		fprintf(stderr, "%s\n", strerror(errno));
	}
	printPrefix();
	while(fgets(buffer, BUF_SIZE, stdin) != NULL)
	{
		if(buffer[0]!='\n')
		{
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
			//setPipe(hist, buffer);
		}
		printPrefix();
	}
	printf("\n");
	return 0;
}	
