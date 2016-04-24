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

int main(int argc, char **argv, char *envp[])
{
	char buffer[BUF_SIZE];
	FILE *hist;
	setEnvironnement();
	histPath = histoPath();
	hist = fopen(histPath,"a+");
	printPrefix();
	while(fgets(buffer, BUF_SIZE, stdin) != NULL)
	{
		if(buffer[0]!='\n')
		{
			launch(hist, buffer);
		}
		printPrefix();
	}
	printf("\n");
	free(histPath);
	return 0;
}	
