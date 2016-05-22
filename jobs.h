#ifndef  JOBS_INC
#define  JOBS_INC

#include "signal.h"
#include "utilities.h"
#define MAX_JOB 15

enum {RUNNING, STOPPED};

typedef struct job t_job;

extern int nbJobs;
extern int fgJob;

struct job
{
	pid_t pid;
	char *cmd;
	int status;
};

extern t_job jobsList[MAX_JOB];

void initJob(char *buffer, int pid);
void destrJob(int id);
void decalage(int id);
void fg(pid_t pid);
void bg(pid_t pid);
void waitJob(pid_t pid);
void jobs();
void killJob(pid_t pid);
void ps();

void initSigHandle();

void sigHandler(int num);
void stopHandler(int num);
void childHandler(int num);
#endif
