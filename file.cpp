#include <stdio.h>

char* load_entire_file(const char* filename, const char* mode)
{

    FILE* file = fopen(filename, mode);

    if(!file)
    {
        printf("Input File \"%s\" does not exist.\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t flen = ftell(file);
    rewind(file);

    char* buffer = new char[flen+1];
    fread(buffer, 1, flen, file);
    buffer[flen] = '\0';
    fclose(file);

    return buffer;
}
