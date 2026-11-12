#include <windows.h>
#include <d3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "function_stubs.h"
#include "../effects.h"
#include "../camera.h"
#include "../box.h"
#include "../spotcam.h"
#include "../gameflow.h"
#include "../room.h"
#include "lighting.h"
#include "dxsound.h"

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

uint8 Decompress(char *pDest, char *pCompressed, sint32 compressedSize, sint32 size)
{
    z_stream stream;

    Log(2, "Decompress");
    memset(&stream, 0, sizeof(z_stream));
    stream.avail_in = compressedSize;
    stream.avail_out = size;
    stream.next_out = (uint8 *)pDest;
    stream.next_in = (uint8 *)pCompressed;
    inflateInit(&stream);
    inflate(&stream, Z_FINISH);
    if (stream.total_out != size) {
        Log(1, "Error Decompressing Data");
        return 0;
    }
    inflateEnd(&stream);
    Log(5, "Decompression OK");
    return 1;
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

uint8 LoadBoxes(void)
{
    BOX_INFO *box;
    sint32 size, i, j;

    Log(2, "LoadBoxes");
    num_boxes = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    boxes = (BOX_INFO *)game_malloc(sizeof(BOX_INFO) * num_boxes, 0);
    memcpy(boxes, FileData, sizeof(BOX_INFO) * num_boxes);
    FileData += sizeof(BOX_INFO) * num_boxes;
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    overlap = (sint16 *)game_malloc(sizeof(uint16) * size, 0);
    memcpy(overlap, FileData, sizeof(uint16) * size);
    FileData += sizeof(uint16) * size;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            ground_zone[j][i] = (sint16 *)game_malloc(sizeof(sint16) * num_boxes, 0);
            memcpy(ground_zone[j][i], FileData, sizeof(sint16) * num_boxes);
            FileData += sizeof(sint16) * num_boxes;
        }
        ground_zone[4][i] = (sint16 *)game_malloc(sizeof(sint16) * num_boxes, 0);
        memcpy(ground_zone[4][i], FileData, sizeof(sint16) * num_boxes);
        FileData += sizeof(sint16) * num_boxes;
    }
    for (i = 0; i < num_boxes; i++) {
        box = &boxes[i];
        if (box->overlap_index & 0x8000) {
            box->overlap_index |= 0x4000;
        } else if ((gfLevelFlags & GF_TRAIN) && box->height > -256) {
            box->overlap_index |= 0xC000;
        }
    }
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

void S_GetUVRotateTextures(void)
{
    TEXTURESTRUCT *tex;
    sint16 *pRange;
    sint32 i, j;

    pRange = aranges + 1;
    for (i = 0; i < nAnimUVRanges; i++, pRange++) {
        for (j = (int)*(pRange++); j >= 0; j--, pRange++) {
            tex = &textinfo[*pRange];
            AnimatingTexturesV[i][j][0] = tex->v1;
        }
        pRange--;
    }
}

void FreeLevel(void)
{
    MESH_DATA **vbuf;
    MESH_DATA *mesh;
    ROOM_INFO *r;
    sint32 i;

    Log(2, "FreeLevel");
    vbuf = mesh_vtxbuf;
    for (i = 0; i < num_level_meshes; i++, vbuf++) {
        mesh = *vbuf;
        if (mesh->SourceVB) {
            Log(4, "Released %s @ %x - RefCnt = %d", "Mesh VB", mesh->SourceVB, IDirect3DVertexBuffer_Release((LPDIRECT3DVERTEXBUFFER)mesh->SourceVB));
            mesh->SourceVB = 0;
        }
    }
    if (room) {
        r = room;
        for (i = 0; i < number_rooms; i++, r++) {
            if (r->SourceVB) {
                Log(4, "Released %s @ %x - RefCnt = %d", "Source VB", r->SourceVB, IDirect3DVertexBuffer_Release(r->SourceVB));
                r->SourceVB = 0;
            } else {
                Log(1, "%s Attempt To Release NULL Ptr", "Source VB");
            }
        }
    }
    Log(5, "Free Textures");
    FreeTextures();
    Log(5, "Free Lights");
    FreeD3DLights();
    DXFreeSounds();
    free(OutsideRoomTable);
    free(OutsideRoomOffsets);
    malloc_ptr = malloc_buffer;
    malloc_free = malloc_size;
}
