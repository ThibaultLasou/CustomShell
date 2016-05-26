#include "cmds.h"

void cd(char *path)
{
	char *WD;
	/* si cd sans arguments positionner dans HOME */
	if(path == NULL)
	{
		path = getenv("HOME");
	}
	if(chdir(path) == -1)
	{
		fprintf(stderr, "%s\n", strerror(errno));
	}
	/* Changer la variable d'environnement*/
	WD = getcwd(NULL, 0);
	setenv("PWD", WD, 1);
	free(WD);
}

void history(char *arg)
{
	char *cmd;
	if(arg == NULL)
	{
		cmd = malloc(sizeof(char)*strlen(cmdsPath[HISTORY])+strlen("cat -n")+1);
		sprintf(cmd, "cat -n %s", cmdsPath[HISTORY]);
	}
	else
	{
		cmd = malloc(sizeof(char)*strlen(cmdsPath[HISTORY])+strlen("tail -n=")+strlen(arg)+1);
		sprintf(cmd, "tail -n=%s %s", arg, cmdsPath[HISTORY]);
	}
	launch(cmd);
}

bool makeCmdsPath(char *launchPath)
{
	char shellPath[PATH_MAX];
	char *pos;
	if(realpath(launchPath, shellPath) != NULL)
	{
		pos = strrchr(shellPath, (int) '/');
		*(pos+1) = '\0';

		sprintf(cmdsPath[HISTORY],"%s.history", shellPath);
		sprintf(cmdsPath[CAT],"%scat", shellPath);
		sprintf(cmdsPath[TOUCH],"%stouch", shellPath);
		sprintf(cmdsPath[TAIL],"%stail", shellPath);
		sprintf(cmdsPath[CP],"%scp", shellPath);
		
		return true;
	}
	else
	{
		fprintf(stderr, "%s\n", strerror(errno));
		return false;
	}
}
