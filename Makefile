CC = gcc
CFLAGS = -g -Wall -Wextra -pthread

PROGRAMS = 1cozinheiro 

all: $(PROGRAMS)

clean:
	rm -f *~ $(PROGRAMS)