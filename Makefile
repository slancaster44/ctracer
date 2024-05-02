CC = gcc
CFLAGS = -o tracer -march=native -Wno-pointer-arith -Wno-unused-result -fpack-struct=1
INCLUDE = -Iinclude
SOURCE = `find ./src -name *.c ! -name test.c ! -name demo.c ! -name benchmark.c`
LDFLAGS = -lm -lcjson

test:
	clear
	$(CC) -O0 $(CFLAGS) -g -Wpedantic -Wall -Wconversion $(INCLUDE) $(SOURCE) src/test.c $(LDFLAGS) -lefence
	./tracer

benchmark:
	clear
	$(CC) -O2 $(CFLAGS) $(INCLUDE) $(SOURCE) src/benchmark.c $(LDFLAGS)
	./tracer

demo:
	clear
	$(CC) -O2 $(CFLAGS) -g $(INCLUDE) $(SOURCE) src/demo.c $(LDFLAGS)
	time ./tracer

profile:
	clear
	$(CC) -O2 $(CFLAGS) $(INCLUDE) $(SOURCE) src/benchmark.c $(LDFLAGS) -pg
	./tracer
