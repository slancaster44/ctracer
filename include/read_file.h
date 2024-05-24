#ifndef READ_FILE_H
#define READ_FILE_H

#include <stdio.h>

/**
 * @def READ_FILE
 * Read data from a file
 * 
 * @param BUF_PTR - Will point to the contents of the file after READ_FILE() is called
 * @param FILE_SIZE - After READ_FILE() is called, FILE_SIZE will contain the number of bytes read
 * @param FILE_NAME - The name of the file to read
 * 
 * @example 
 * <code>
 *      char* file_contents;
 *      int file_size
 *      READ_FILE(file_contents, file_size, "myfile.txt")
 * </code>
 */
#define READ_FILE(BUF_PTR, FILE_SIZE, FILE_NAME)                             \
    FILE *fp = fopen(FILE_NAME, "r");                                        \
    if (fp == NULL)                                                          \
    {                                                                        \
        printf("Error: unable to open file '%s'\n", FILE_NAME);              \
        exit(1);                                                             \
    }                                                                        \
                                                                             \
    fseek(fp, 0L, SEEK_END);                                                 \
    FILE_SIZE = sizeof(char) * ((unsigned long)ftell(fp) + 1);               \
    rewind(fp);                                                              \
                                                                             \
    BUF_PTR = alloca(FILE_SIZE);                                             \
    fread(BUF_PTR, 1, FILE_SIZE, fp);                                        \
    fclose(fp);                                                              \
                                                                             \
    BUF_PTR[FILE_SIZE - 1] = '\0';

#endif
