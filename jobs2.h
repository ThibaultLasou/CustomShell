#ifndef  JOBS2_INC
#define  JOBS2_INC

#include <termios.h>

#include "cmds.h"

typedef struct s_process process;
typedef struct s_job job;

extern pid_t shell_pgid;
extern int shell_terminal;
extern job *first_job;

/* A process is a single process.  */
struct s_process
{
	process *next;       /* next process in pipeline */
	char **argv;                /* for exec */
	pid_t pid;                  /* process ID */
	bool completed;             /* true if process has completed */
	bool stopped;               /* true if process has stopped */
	int status;                 /* reported status value */
};

/* A job is a pipeline of processes.  */
struct s_job
{
	job *next;           /* next active job */
	char *command;              /* command line, used for messages */
	process *first_process;     /* list of processes in this job */
	pid_t pgid;                 /* process group ID */
	struct termios tmodes;
	bool notified;              /* true if user told about stopped job */
	int in, out, err;  /* standard i/o channels */
};

job *addJob(char *command);
job *createJob(char *command);
process *createProcess(char *command);

job *find_job(pid_t pgid);
bool job_is_stopped(job *j);
bool job_is_completed(job *j);

void init_shell();

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, bool foreground);
void launch_job(job *j, bool foreground);
void continue_job(job *j, bool foreground);
void put_job_in_foreground(job *j, bool cont);
void put_job_in_background(job *j, bool cont);
void wait_for_job(job *j);

int mark_process_status(pid_t pid, int status);
void update_status();

void format_job_info(job *j, const char *status);
void do_job_notification();
void mark_job_as_running(job *j);

#endif   /* ----- #ifndef JOBS2_INC  ----- */
