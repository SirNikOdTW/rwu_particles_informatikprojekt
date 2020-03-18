#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include "utils.h"

char *readFile(char *filename, char *mode, long *size)
{
    FILE *file;
    char *buffer;
    long bytes;

    if((file = fopen(filename, mode)) == NULL)
    {
        printf("ERROR open file %s: %s\n", filename, strerror(errno));
    }

    fseek(file, 0L, SEEK_END);
    bytes = ftell(file);
    fseek(file, 0L, SEEK_SET);

    if((buffer = calloc(bytes, sizeof(char))) == NULL)
    {
        printf("ERROR allocating memory: %s\n", strerror(errno));
    }

    fread(buffer, sizeof(char), bytes, file);
    fclose(file);

    if (size != NULL)
    {
        *size = bytes;
    }

    return buffer;
}
