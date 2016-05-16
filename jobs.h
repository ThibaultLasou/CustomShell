#ifndef  JOBS_INC
#define  JOBS_INC

#define MAX_JOB 15

enum {RUNNING, STOPPED};

extern t_job jobsList[MAX_JOB];
extern int nbJobs;

typedef struct job t_job;

struct job
{
	pid_t pid;
	char **cmd;
	int status;
};

void fg(pid_t pid);
void bg(pid_t pid);
void wait(pid_t pid);
void jobs();
void kill(pid_t pid);
void ps();

void initSigHandle();

#endif
