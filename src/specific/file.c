#include <windows.h>
#include <d3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include "file.h"
#include "function_stubs.h"
#include "../effects.h"
#include "../camera.h"
#include "../box.h"
#include "../spotcam.h"
#include "../gameflow.h"
#include "../room.h"
#include "../anim.h"
#include "../draw.h"
#include "lighting.h"
#include "drawroom.h"
#include "dxsound.h"
#include "../sound.h"
#include "../laramisc.h"
#include "../setup.h"
#include "../lara.h"
#include "../objnums.h"
#include "../tomb4fx.h"
#include "winmain.h"
#include "loadsave.h"
#include "dxshell.h"
#include "specificfx.h"
#include "audio.h"

#ifndef TR4_EXE
FILE *FileOpen(const char *name)
{
    FILE *file;
    char path_name[80];

    memset(path_name, 0, 80);
    path_name[0] = cd_drive;
    path_name[1] = ':';
    path_name[2] = '\\';
    strcat(path_name, name);
    Log(5, "FileOpen - %s", path_name);
    file = fopen(path_name, "rb");
    if (!file)
        Log(1, "Unable To Open %s", path_name);
    return file;
}
#endif

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

uint8 LoadObjects(void)
{
    OBJECT_INFO *obj;
    STATIC_INFO *stat;
    sint16 **mesh;
    sint16 **mesh_size;
    sint32 size, num, slot, i;

    Log(2, "LoadObjects");
    memset(objects, 0, sizeof(objects));
    memset(static_objects, 0, sizeof(STATIC_INFO) * NUMBER_STATIC_OBJECTS);
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    mesh_base = (sint16 *)game_malloc(size * sizeof(sint16), 0);
    memcpy(mesh_base, FileData, size * sizeof(sint16));
    FileData += size * sizeof(sint16);
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    meshes = (sint16 **)game_malloc(2 * size * sizeof(sint16 *), 0);
    memcpy(meshes, FileData, size * sizeof(sint16 *));
    FileData += size * sizeof(sint16 *);
    for (i = 0; i < size; i++)
        meshes[i] = mesh_base + (sint32)meshes[i] / 2;
    num_meshes = size;
    num_anims = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    anims = (ANIM_STRUCT *)game_malloc(sizeof(ANIM_STRUCT) * num_anims, 0);
    memcpy(anims, FileData, sizeof(ANIM_STRUCT) * num_anims);
    FileData += sizeof(ANIM_STRUCT) * num_anims;
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    changes = (CHANGE_STRUCT *)game_malloc(sizeof(CHANGE_STRUCT) * size, 0);
    memcpy(changes, FileData, sizeof(CHANGE_STRUCT) * size);
    FileData += sizeof(CHANGE_STRUCT) * size;
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    ranges = (RANGE_STRUCT *)game_malloc(sizeof(RANGE_STRUCT) * size, 0);
    memcpy(ranges, FileData, sizeof(RANGE_STRUCT) * size);
    FileData += sizeof(RANGE_STRUCT) * size;
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    commands = (sint16 *)game_malloc(sizeof(sint16) * size, 0);
    memcpy(commands, FileData, sizeof(sint16) * size);
    FileData += sizeof(sint16) * size;
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    bones = (sint32 *)game_malloc(sizeof(sint32) * size, 0);
    memcpy(bones, FileData, sizeof(sint32) * size);
    FileData += sizeof(sint32) * size;
    size = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    frames = (sint16 *)game_malloc(sizeof(sint16) * size, 0);
    memcpy(frames, FileData, sizeof(sint16) * size);
    FileData += sizeof(sint16) * size;
    for (i = 0; i < num_anims; i++)
        anims[i].frame_ptr = (sint16 *)((sint32)anims[i].frame_ptr + (sint32)frames);
    num = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    for (i = 0; i < num; i++) {
        slot = *(sint32 *)FileData;
        FileData += sizeof(sint32);
        objects[slot].nmeshes = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        objects[slot].mesh_index = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        objects[slot].bone_index = *(sint32 *)FileData;
        FileData += sizeof(sint32);
        objects[slot].frame_base = *(sint16 **)FileData;
        FileData += sizeof(sint16 *);
        objects[slot].anim_index = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        objects[slot].loaded = 1;
    }
    CreateSkinningData();
    for (i = 0; i < NUMBER_OBJECTS; i++) {
        obj = &objects[i];
        obj->mesh_index *= 2;
    }
    mesh = meshes;
    mesh_size = &meshes[num_meshes];
    memcpy(mesh_size, mesh, num_meshes * 4);
    for (i = 0; i < num_meshes; i++) {
        *mesh++ = *mesh_size;
        *mesh++ = *mesh_size;
        mesh_size++;
    }
    InitialiseObjects();
    num = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    for (i = 0; i < num; i++) {
        slot = *(sint32 *)FileData;
        FileData += sizeof(sint32);
        static_objects[slot].mesh_number = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        memcpy(&static_objects[slot].x_minp, FileData, 6 * sizeof(sint16));
        FileData += 6 * sizeof(sint16);
        memcpy(&static_objects[slot].x_minc, FileData, 6 * sizeof(sint16));
        FileData += 6 * sizeof(sint16);
        static_objects[slot].flags = *(sint16 *)FileData;
        FileData += sizeof(sint16);
    }
    for (i = 0; i < NUMBER_STATIC_OBJECTS; i++) {
        stat = &static_objects[i];
        stat->mesh_number *= 2;
    }
    ProcessMeshData(num_meshes * 2);
    return 1;
}

uint8 LoadSprites(void)
{
    PHDSPRITESTRUCT sprite;
    sint32 num_sprites, num_slots, slot, i;

    Log(2, "LoadSprites");
    FileData += 3;
    num_sprites = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    spriteinfo = (SPRITESTRUCT *)game_malloc(sizeof(SPRITESTRUCT) * num_sprites, 0);
    for (i = 0; i < num_sprites; i++) {
        memcpy(&sprite, FileData, sizeof(PHDSPRITESTRUCT));
        FileData += sizeof(PHDSPRITESTRUCT);
        spriteinfo[i].height = sprite.height;
        spriteinfo[i].offset = sprite.offset;
        spriteinfo[i].tpage = sprite.tpage;
        spriteinfo[i].width = sprite.width;
        spriteinfo[i].x1 = (float)sprite.x1 * (1.0F / 256.0F);
        spriteinfo[i].y1 = (float)sprite.y1 * (1.0F / 256.0F);
        spriteinfo[i].x2 = (float)sprite.x2 * (1.0F / 256.0F);
        spriteinfo[i].y2 = (float)sprite.y2 * (1.0F / 256.0F);
        spriteinfo[i].x1 += 1.0F / 256.0F;
        spriteinfo[i].y1 += 1.0F / 256.0F;
        spriteinfo[i].x2 -= 1.0F / 256.0F;
        spriteinfo[i].y2 -= 1.0F / 256.0F;
        spriteinfo[i].tpage++;
    }
    num_slots = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    for (i = 0; i < num_slots; i++) {
        slot = *(sint32 *)FileData;
        FileData += sizeof(sint32);
        if (slot < NUMBER_OBJECTS) {
            objects[slot].nmeshes = *(sint16 *)FileData;
            FileData += sizeof(sint16);
            objects[slot].mesh_index = *(sint16 *)FileData;
            FileData += sizeof(sint16);
            objects[slot].loaded = 1;
        } else {
            slot -= NUMBER_OBJECTS;
            static_objects[slot].mesh_number = *(sint16 *)FileData;
            FileData += sizeof(sint16);
            static_objects[slot].mesh_number = *(sint16 *)FileData;
            FileData += sizeof(sint16);
        }
    }
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

uint8 LoadTextureInfos(void)
{
    PHDTEXTURESTRUCT tex;
    sint32 val, i;

    Log(2, "LoadTextureInfos");
    FileData += 3;
    val = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    Log(5, "Texture Infos : %d", val);
    textinfo = (TEXTURESTRUCT *)game_malloc(val * sizeof(TEXTURESTRUCT), 0);
    for (i = 0; i < val; i++) {
        memcpy(&tex, FileData, sizeof(PHDTEXTURESTRUCT));
        FileData += sizeof(PHDTEXTURESTRUCT);
        textinfo[i].drawtype = tex.drawtype;
        textinfo[i].tpage = tex.tpage & 0x7FFF;
        textinfo[i].flag = tex.tpage ^ (tex.tpage ^ tex.flag) & 0x7FFF;
        textinfo[i].u1 = (float)tex.u1 * (1.0F / 65535.0F);
        textinfo[i].v1 = (float)tex.v1 * (1.0F / 65535.0F);
        textinfo[i].u2 = (float)tex.u2 * (1.0F / 65535.0F);
        textinfo[i].v2 = (float)tex.v2 * (1.0F / 65535.0F);
        textinfo[i].u3 = (float)tex.u3 * (1.0F / 65535.0F);
        textinfo[i].v3 = (float)tex.v3 * (1.0F / 65535.0F);
        textinfo[i].u4 = (float)tex.u4 * (1.0F / 65535.0F);
        textinfo[i].v4 = (float)tex.v4 * (1.0F / 65535.0F);
    }
    AdjustUV(val);
    Log(5, "Created %d Texture Pages", nTextures - 1);
    return 1;
}

uint8 LoadItems(void)
{
    ITEM_INFO *item;
    ROOM_INFO *r;
    FLOOR_INFO *floor;
    MESH_INFO *mesh;
    sint32 i, j, x, z;
    sint16 y;

    Log(2, "LoadItems");
    num_items = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    if (!num_items)
        return 1;
    items = (ITEM_INFO *)game_malloc(256 * sizeof(ITEM_INFO), 0);
    level_items = num_items;
    InitialiseItemArray(256);
    for (i = 0; i < num_items; i++) {
        item = &items[i];
        item->object_number = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        item->room_number = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        item->pos.x_pos = *(sint32 *)FileData;
        FileData += sizeof(sint32);
        item->pos.y_pos = *(sint32 *)FileData;
        FileData += sizeof(sint32);
        item->pos.z_pos = *(sint32 *)FileData;
        FileData += sizeof(sint32);
        item->pos.y_rot = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        item->shade = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        item->trigger_flags = *(sint16 *)FileData;
        FileData += sizeof(sint16);
        item->flags = *(sint16 *)FileData;
        FileData += sizeof(sint16);
    }
    for (i = 0; i < num_items; i++)
        InitialiseItem(i);
    for (i = 0; i < number_rooms; i++) {
        r = &room[i];
        for (j = r->num_meshes, mesh = r->mesh; j > 0; j--, mesh++) {
            x = (mesh->x - r->x) >> 10;
            z = (mesh->z - r->z) >> 10;
            floor = &r->floor[x * r->x_size + z];
            if (!(boxes[floor->box].overlap_index & 0x4000) && (gfCurrentLevel != 4 || i != 19 && i != 23 && i != 16)) {
                y = floor->floor << 8;
                if (y <= mesh->y - static_objects[mesh->static_number].y_maxc + 512 && y < mesh->y - static_objects[mesh->static_number].y_minc) {
                    if (!static_objects[mesh->static_number].x_maxc || !static_objects[mesh->static_number].x_minc
                        || !static_objects[mesh->static_number].z_maxc || !static_objects[mesh->static_number].z_minc
                        || (static_objects[mesh->static_number].x_maxc ^ static_objects[mesh->static_number].x_minc) & 0x8000
                        && (static_objects[mesh->static_number].z_maxc ^ static_objects[mesh->static_number].z_minc) & 0x8000)
                        floor->stopper = 1;
                }
            }
        }
    }
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

uint8 LoadSamples(void)
{
    sint32 num_samples, uncomp_size, comp_size, i;
    static sint32 num_sample_infos;

    Log(2, "LoadSamples");
    sample_lut = (sint16 *)game_malloc(370 * sizeof(sint16), 0);
    memcpy(sample_lut, FileData, 370 * sizeof(sint16));
    FileData += 370 * sizeof(sint16);
    num_sample_infos = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    Log(8, "Number Of Sample Infos %d", num_sample_infos);
    if (!num_sample_infos) {
        Log(1, "No Sample Infos");
        return 0;
    }
    sample_infos = (SAMPLE_INFO *)game_malloc(sizeof(SAMPLE_INFO) * num_sample_infos, 0);
    memcpy(sample_infos, FileData, sizeof(SAMPLE_INFO) * num_sample_infos);
    FileData += sizeof(SAMPLE_INFO) * num_sample_infos;
    i = *(sint32 *)FileData;
    FileData += sizeof(sint32);
    if (!i) {
        Log(1, "No Samples");
        return 0;
    }
    Log(8, "Number Of Samples %d", i);
    fread(&num_samples, 1, 4, level_fp);
    InitSampleDecompress();
    for (i = 0; i < num_samples; i++) {
        fread(&uncomp_size, 1, 4, level_fp);
        fread(&comp_size, 1, 4, level_fp);
        fread(samples_buffer, comp_size, 1, level_fp);
        if (!DXCreateSampleADPCM(samples_buffer, comp_size, uncomp_size, i)) {
            FreeSampleDecompress();
            return 0;
        }
    }
    FreeSampleDecompress();
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

unsigned int __stdcall LoadLevel(void *name)
{
    OBJECT_INFO *obj;
    TEXTURESTRUCT *tex;
    char *pData;
    sint32 version, size, compressedSize, i;

    Log(2, "LoadLevel");
    FreeLevel();
    memset(malloc_ptr, 0, MALLOC_SIZE);
    memset(&lara, 0, sizeof(LARA_INFO));
    Textures = (TEXTURE *)AddStruct(Textures, nTextures, sizeof(TEXTURE));
    nTextures = 1;
    Textures[0].tex = 0;
    Textures[0].surface = 0;
    Textures[0].width = 0;
    Textures[0].height = 0;
    Textures[0].bump = 0;
    S_InitLoadBar(20);
    S_LoadBar();
    CompressedData = 0;
    FileData = 0;
    level_fp = 0;
    level_fp = FileOpen((const char *)name);
    if (level_fp) {
        sint16 RTPages, OTPages, BTPages;

        fread(&version, 1, 4, level_fp);
        fread(&RTPages, 1, 2, level_fp);
        fread(&OTPages, 1, 2, level_fp);
        fread(&BTPages, 1, 2, level_fp);
        Log(7, "Process Level Data");
        LoadTextures(RTPages, OTPages, BTPages);
        fread(&size, 1, 4, level_fp);
        fread(&compressedSize, 1, 4, level_fp);
        CompressedData = (char *)malloc(compressedSize);
        FileData = (char *)malloc(size);
        fread(CompressedData, compressedSize, 1, level_fp);
        Decompress(FileData, CompressedData, compressedSize, size);
        free(CompressedData);
        pData = FileData;
        S_LoadBar();
        LoadRooms();
        S_LoadBar();
        LoadObjects();
        S_LoadBar();
        LoadSprites();
        S_LoadBar();
        LoadCameras();
        S_LoadBar();
        LoadSoundEffects();
        S_LoadBar();
        LoadBoxes();
        S_LoadBar();
        LoadAnimatedTextures();
        S_LoadBar();
        LoadTextureInfos();
        S_LoadBar();
        LoadItems();
        S_LoadBar();
        LoadAIInfo();
        S_LoadBar();
        LoadCinematic();
        S_LoadBar();
        if (acm_ready && !App.SoundDisabled)
            LoadSamples();
        free(pData);
        S_LoadBar();
        for (i = 0; i < 3; i++) {
            obj = &objects[WATERFALL1 + i];
            if (obj->loaded) {
                tex = &textinfo[mesh_vtxbuf[obj->mesh_index]->gt4[4] & 0x7FFF];
                AnimatingWaterfalls[i] = tex;
                AnimatingWaterfallsV[i] = (sint32)tex->v1;
            }
        }
        S_LoadBar();
        S_GetUVRotateTextures();
        InitTarget_2();
        S_LoadBar();
        MallocD3DLights();
        CreateD3DLights();
        SetupGame();
        S_LoadBar();
        SetFadeClip(0, 1);
        reset_cutseq_vars();
        FileClose(level_fp);
    }
    LevelLoadingThread.active = 0;
    _endthreadex(1);
    return 1;
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

#ifndef TR4_EXE
sint32 S_LoadLevelFile(sint32 num)
{
    char name[80];

    Log(2, "S_LoadLevelFile");
    strcpy(name, &gfFilenameWad[gfFilenameOffset[num]]);
    strcat(name, ".TR4");
    LevelLoadingThread.active = 1;
    LevelLoadingThread.ended = 0;
    LevelLoadingThread.handle = _beginthreadex(0, 0, LoadLevel, name, 0, &LevelLoadingThread.address);
    while (LevelLoadingThread.active)
        ;
    return 1;
}
#endif
