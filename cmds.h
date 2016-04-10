#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 512

/*
 * struct cmd
 * {
 *		char *cmd
 *		void *fonc;
 * }
 */

void cd(char *path);
void makeCmd(char *buffer, char ***args);
