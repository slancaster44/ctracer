CC = gcc
CFLAGS = -O2 -o tracer -march=native
INCLUDE = -Iinclude
SOURCE = `find ./src -name *.c ! -name test.c ! -name main.c ! -name benchmark.c`
LDFLAGS = -lm

test:
	clear
	$(CC) $(CFLAGS) -g $(INCLUDE) $(SOURCE) src/test.c $(LDFLAGS)
	valgrind ./tracer
	rm -r ./tracer

benchmark:
	clear
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) src/benchmark.c $(LDFLAGS)
	./tracer
	rm -r ./tracer

default:
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) src/test.c $(LDFLAGS)