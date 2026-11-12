#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "function_stubs.h"
#include "../effects.h"
#include "../camera.h"
#include "../box.h"
#include "../spotcam.h"

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

sint32 LoadFile(const char *name, char **dest)
{
    FILE *file;
    sint32 size, count;

    Log(2, "LoadFile");
    Log(5, "File - %s", name);
    file = FileOpen(name);
    if (file) {
        size = FileSize(file);
        if (!*dest) {
            *dest = (char *)malloc(size);
        }
        count = fread(*dest, 1, size, file);
        Log(5, "Read - %d FileSize - %d", count, size);
        if (count != size) {
            Log(1, "Error Reading File");
            FileClose(file);
            free(*dest);
        } else {
            FileClose(file);
            return size;
        }
    }
    return 0;
}

uint8 LoadCameras(void)
{
    Log(2, "LoadCameras");
    number_cameras = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    if (number_cameras) {
        camera.fixed = (OBJECT_VECTOR *)game_malloc(number_cameras * sizeof(OBJECT_VECTOR), 0);
        memcpy(camera.fixed, FileData, number_cameras * sizeof(OBJECT_VECTOR));
        FileData += number_cameras * sizeof(OBJECT_VECTOR);
    }
    number_spotcams = *(sint16 *)FileData;
    FileData += sizeof(sint32);
    if (number_spotcams) {
        memcpy(SpotCam, FileData, number_spotcams * sizeof(SPOTCAM));
        FileData += number_spotcams * sizeof(SPOTCAM);
    }
    return 1;
}

uint8 LoadSoundEffects(void)
{
    Log(2, "LoadSoundEffects");
    number_sound_effects = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    Log(8, "Number of SFX %d", number_sound_effects);
    if (!number_sound_effects) {
        return 1;
    }
    sound_effects = (OBJECT_VECTOR *)game_malloc(number_sound_effects * sizeof(OBJECT_VECTOR), 0);
    memcpy(sound_effects, FileData, number_sound_effects * sizeof(OBJECT_VECTOR));
    FileData += number_sound_effects * sizeof(OBJECT_VECTOR);
    return 1;
}

uint8 LoadAnimatedTextures(void)
{
    sint32 num_anim_ranges;
    uint8 n;

    num_anim_ranges = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    aranges = (sint16 *)game_malloc(num_anim_ranges * 2, 0);
    memcpy(aranges, FileData, num_anim_ranges * 2);
    FileData += num_anim_ranges * sizeof(sint16);
    n = *(uint8 *)FileData;
    FileData += sizeof(char);
    nAnimUVRanges = n;
    return 1;
}

uint8 LoadCinematic(void)
{
    FileData += sizeof(sint16);
    return 1;
}

uint8 LoadAIInfo(void)
{
    sint32 num_ai;

    num_ai = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    if (num_ai) {
        nAIObjects = (sint16)num_ai;
        AIObjects = (AIOBJECT *)game_malloc(sizeof(AIOBJECT) * num_ai, 0);
        memcpy(AIObjects, FileData, sizeof(AIOBJECT) * num_ai);
        FileData += sizeof(AIOBJECT) * num_ai;
    }
    return 1;
}
