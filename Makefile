CC = gcc
EXEC = shell
SRC = main.c cmds.c exec.c

all: $(EXEC)

shell: 
	$(CC) $(SRC) -o $@ $^

debug:
	$(CC) $(SRC) -o $@ $^ -g

clean:
	rm *.o $(EXEC)
