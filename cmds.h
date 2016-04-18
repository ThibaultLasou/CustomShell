#ifndef  CMDS_INC
#define  CMDS_INC

#include "utilities.h"

void touch(char **args, int argc);
void cat(char **args, int argc);
void cd(char *path);
void history(FILE *histo);
#endif
