CC = gcc
CFLAGS = -g -c

target: mypthread.a test

mypthread.a: mypthread.o

mypthread.o: mypthread.h
	$(CC) -pthread $(CFLAGS) mypthread.c

test: mypthread_test.c mypthread.h mypthread.c
	$(CC) mypthread_test.c -g -L. -lmypthread -o test

clean:
	rm -rf test 
