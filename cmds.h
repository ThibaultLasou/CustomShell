#ifndef  CMDS_INC
#define  CMDS_INC

#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 512

void touch(char **args);
void cat(char **args);
void cd(char *path);
void history(FILE *histo);
void makeCmd(char *buffer, char ***args);

#endif
