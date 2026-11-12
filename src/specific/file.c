#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "function_stubs.h"

void FileClose(FILE *file)
{
    Log(2, "FileClose");
    fclose(file);
}

sint32 FileSize(FILE *file)
{
    sint32 size;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

uint8 LoadCinematic(void)
{
    FileData += sizeof(sint16);
    return 1;
}
