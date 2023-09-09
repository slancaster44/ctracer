#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

void* shmalloc(size_t size) {
    void* val = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (val == (void*) -1) {
        printf("mmap() failed, %d\n", errno);
        exit(1);
    }

    return val;
}

void shfree(void* ptr, size_t size) {
    munmap(ptr, size);
}