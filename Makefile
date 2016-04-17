CC = gcc
EXEC = shell
SRC = main.c cmds.c exec.c utilities.c

all: $(EXEC)

shell: 
	$(CC) $(SRC) -o $@ $^

debug:
	$(CC) $(SRC) -o $@ $^ -g

clean:
	rm $(EXEC) debug
