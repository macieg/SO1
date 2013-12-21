CFLAGS=-Wall -std=c99

all: err.o manager executor

err.o: err.h err.c
	gcc $(CFLAGS) -c err.c -o err.o

manager: manager.c
	gcc $(CFLAGS) err.o manager.c -o manager 

executor: executor.c
	gcc $(CFLAGS) err.o executor.c -o executor

clean:
	rm *.o manager executor
