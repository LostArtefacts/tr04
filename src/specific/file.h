#ifndef SPECIFIC_FILE_H
#define SPECIFIC_FILE_H

#include <stdio.h>
#include "../3dglodef.h"

typedef struct z_stream_s {
    uint8 *next_in;
    uint32 avail_in;
    uint32 total_in;
    uint8 *next_out;
    uint32 avail_out;
    uint32 total_out;
    char *msg;
    void *state;
    void *zalloc;
    void *zfree;
    void *opaque;
    sint32 data_type;
    uint32 adler;
    uint32 reserved;
} z_stream;

#define Z_FINISH 4
#define inflateInit(strm) inflateInit_((strm), "1.1.3", sizeof(z_stream))

sint32 inflateInit_(z_stream *strm, const char *version, sint32 stream_size);
sint32 inflate(z_stream *strm, sint32 flush);
sint32 inflateEnd(z_stream *strm);

extern TEXTURESTRUCT *AnimatingWaterfalls[3];
extern sint32 AnimatingWaterfallsV[3];

extern char *FileData;
extern MESH_DATA **mesh_vtxbuf;
extern sint32 num_level_meshes;
extern char *OutsideRoomTable;
extern sint16 *OutsideRoomOffsets;
extern sint32 number_cameras;
extern FILE *level_fp;
extern sint16 *aranges;
extern sint32 nAnimUVRanges;
extern TEXTURESTRUCT *textinfo;
extern SPRITESTRUCT *spriteinfo;
extern float AnimatingTexturesV[16][8][3];

FILE *FileOpen(const char *name);
void FreeTextures(void);
void FileClose(FILE *file);
sint32 FileSize(FILE *file);
uint8 LoadCinematic(void);
sint32 LoadFile(const char *name, char **dest);
uint8 LoadSoundEffects(void);
uint8 LoadCameras(void);
uint8 LoadAnimatedTextures(void);
uint8 LoadAIInfo(void);
uint8 LoadBoxes(void);
void S_GetUVRotateTextures(void);
uint8 Decompress(char *pDest, char *pCompressed, sint32 compressedSize, sint32 size);
void FreeLevel(void);
uint8 LoadSamples(void);

#endif
