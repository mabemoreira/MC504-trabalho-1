CC = gcc
CFLAGS = -g -Wall -Wextra -pthread

PROGRAMS = 1cozinheiro \
Ncozinheiros

all: $(PROGRAMS)

clean:
	rm -f *~ $(PROGRAMS)