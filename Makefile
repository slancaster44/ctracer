CC = gcc
CFLAGS = -O2 -o tracer -march=native -Wno-pointer-arith -Wno-unused-result
INCLUDE = -Iinclude
SOURCE = `find ./src -name *.c ! -name test.c ! -name demo.c ! -name benchmark.c`
LDFLAGS = -lm -lcjson

test:
	clear
	$(CC) $(CFLAGS) -g -Wpedantic -Wall -Wconversion -Wswitch-enum $(INCLUDE) $(SOURCE) src/test.c $(LDFLAGS) -lefence
	./tracer

benchmark:
	clear
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) src/benchmark.c $(LDFLAGS)
	./tracer

demo:
	clear
	$(CC) $(CFLAGS) -g $(INCLUDE) $(SOURCE) src/demo.c $(LDFLAGS)
	./tracer

profile:
	clear
	$(CC) $(CFLAGS) $(INCLUDE) $(SOURCE) src/benchmark.c $(LDFLAGS) -pg
	./tracer
