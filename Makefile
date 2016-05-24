CC = gcc
EXEC = shell
SRC = main.c cmds.c exec.c utilities.c jobs.c jobs2.c
HDRS = cmds.h exec.h utilities.h jobs.h jobs2.h
CMDS = touch cat tail cp
OBJS = $(SRC:.c=.o)

all: $(EXEC) $(CMDS)

$(EXEC) : $(OBJS) $(HDRS)
	$(CC) -o $(EXEC) $^

$(CMDS): %:%.o 
	$(CC) -o $@ $^

%.o : %.c 
	$(CC) -o $@ -c $< -g

clean:
	rm $(EXEC) *.o $(CMDS)
