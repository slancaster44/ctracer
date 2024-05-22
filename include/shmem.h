#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @private
 * Allocate a memory area that can be accessed by all child processes
*/
void *shmalloc(unsigned size)
{
    void *val = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (val == (void *)-1)
    {
        printf("mmap() failed, %d\n", errno);
        exit(1);
    }

    return val;
}

/**
 * @private
 * Free a shared memory area allocated by shmalloc()
 */
void shfree(void *ptr, unsigned size)
{
    munmap(ptr, size);
}
