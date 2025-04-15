CC = gcc
CFLAGS = -g -Wall -Wextra -pthread

PROGRAMS = 1cozinheiro \
Ncozinheiros \ NcozinheirosNpanela 

all: $(PROGRAMS)

clean:
	rm -f *~ $(PROGRAMS)