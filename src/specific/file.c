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
