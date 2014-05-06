CFLAGS=-Wall -std=c99 -ledit

clean:
	rm -f parsing

all: parsing

# cc -std=c99 -Wall parsing.c mpc.c -ledit -lm -o parsing
