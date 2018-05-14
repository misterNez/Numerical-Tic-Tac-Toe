CC = g++
PROG = numtictactoe
OBJS = main.o tictactoe.o
DEPS = tictactoe.h

all: $(PROG)

$(PROG): $(OBJS) $(DEPS)
	$(CC) -o $@ $^
	
clean:
	rm -rf *.o $(PROG)
