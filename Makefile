CC = gcc
EXEC = shell
SRC = main.c cmds.c exec.c utilities.c copie.c
OBJS = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC) : $(OBJS)
	$(CC) -o $(EXEC) $^

debug: $(OBJS)
	$(CC) -o $(EXEC)Debug $^ -g

%.o : %.c
	$(CC) -o $@ -c $<

clean:
	rm $(EXEC) debug
