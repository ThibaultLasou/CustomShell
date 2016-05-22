CC = gcc
EXEC = shell
SRC = main.c cmds.c exec.c utilities.c jobs.c
HDRS = cmds.h exec.h utilities.h jobs.h
CMDS = touch cat tail copie
OBJS = $(SRC:.c=.o)

all: $(EXEC) $(CMDS)

$(EXEC) : $(OBJS)
	$(CC) -o $(EXEC) $^

$(CMDS): %:%.o 
	$(CC) -o $@ $^

debug: $(OBJS)
	$(CC) -o $(EXEC)Debug $^ -g

%.o : %.c
	$(CC) -o $@ -c $< -g

clean:
	rm $(EXEC) $(EXEC)Debug *.o
