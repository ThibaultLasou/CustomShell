CC = gcc
EXEC = shell
SRC = main.c cmds.c exec.c
OBJS = $(SRC:.c=.o)

all: $(EXEC)

shell: $(OBJS)
	$(CC) -o $@ $^ $(LIB)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm *.o $(EXEC)
