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
	char *args[4];
	if(arg == NULL)
	{
		args[0] = "cat";
		args[1] = "-n";
		args[2] = cmdsPath[HISTORY];
	}
	else
	{
		args[0] = "tail";
		args[1] = malloc(sizeof(char)*(strlen(arg)+4));
		sprintf(args[1],"-n=%s", arg);
		args[2] = cmdsPath[HISTORY];
		free(args[1]);
	}
}

bool makeCmdsPath(char *launchPath)
{
	char shellPath[PATH_MAX];
	char *pos;
	if(realpath(launchPath, shellPath) != NULL)
	{
		pos = strrchr(shellPath, (int) '/');
		*(pos+1) = '\0';
		printf("%s\n", shellPath);

		sprintf(cmdsPath[HISTORY],"%s.history", shellPath);
		sprintf(cmdsPath[CAT],"%scat", shellPath);
		sprintf(cmdsPath[TOUCH],"%stouch", shellPath);
		sprintf(cmdsPath[TAIL],"%stail", shellPath);
		
		printf("%s\n", cmdsPath[HISTORY]);
		printf("%s\n", cmdsPath[TOUCH]);
		printf("%s\n", cmdsPath[TAIL]);
		printf("%s\n", cmdsPath[CAT]);
		return true;
	}
	else
	{
		fprintf(stderr, "%s\n", strerror(errno));
		return false;
	}
}
