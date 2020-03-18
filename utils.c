#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include "utils.h"

#define BUFFER_SIZE 1024

char *readFile(char *filename)
{
    FILE    *file;
    char    *buffer;
    long    numbytes;

    if((file = fopen(filename, "r")) == NULL)
    {
        printf("ERROR open file %s: %s\n", filename, strerror(errno));
    }

    fseek(file, 0L, SEEK_END);
    numbytes = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if((buffer = calloc(numbytes, sizeof(char))) == NULL)
    {
        printf("ERROR allocating memory: %s\n", strerror(errno));
    }

    fread(buffer, sizeof(char), numbytes, file);
    fclose(file);

    return buffer;
}
