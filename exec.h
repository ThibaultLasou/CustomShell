#ifndef  EXEC_INC
#define  EXEC_INC

#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void makePaths(char ***paths, char *exec);
void execute(char **newArgv);

#endif
