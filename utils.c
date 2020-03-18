#include <stdio.h>
#include <malloc.h>
#include "utils.h"

#define BUFFER_SIZE 1024

char *readFile(char *path)
{
    FILE *file = fopen(path, "r");
    char *str = malloc(BUFFER_SIZE);
    int c, i = 0, j = 1;

    while ((c = fgetc(file)) != EOF)
    {
        if (i == j * BUFFER_SIZE)
        {
            str = realloc(str, ++j * BUFFER_SIZE);
        }

        str[i++] = (char) c;
    }

    fclose(file);

    return str;
}

