#ifndef DRAW_H
#define DRAW_H

#include "items.h"
#include "room.h"

typedef struct static_info {
    sint16 mesh_number;
    sint16 flags;
    sint16 x_minp;
    sint16 x_maxp;
    sint16 y_minp;
    sint16 y_maxp;
    sint16 z_minp;
    sint16 z_maxp;
    sint16 x_minc;
    sint16 x_maxc;
    sint16 y_minc;
    sint16 y_maxc;
    sint16 z_minc;
    sint16 z_maxc;
} STATIC_INFO;

void DrawAnimatingItem(ITEM_INFO *item);
void phd_PopMatrix_I(void);
void phd_PushMatrix_I(void);
void phd_RotY_I(sint16 ang);
void phd_RotX_I(sint16 ang);
void phd_RotZ_I(sint16 ang);
void phd_TranslateRel_I(sint32 x, sint32 y, sint32 z);
void phd_TranslateRel_ID(sint32 x, sint32 y, sint32 z, sint32 x2, sint32 y2, sint32 z2);
void gar_RotYXZsuperpack_I(sint16 **pprot1, sint16 **pprot2, sint32 skip);
void gar_RotYXZsuperpack(sint16 **pprot, sint32 skip);
void InterpolateMatrix(void);
sint32 GetFrames(ITEM_INFO *item, sint16 *frm[], sint32 *rate);
sint16 *GetBestFrame(ITEM_INFO *item);
void phd_PutPolygons(sint16 *objptr, sint32 clip);
void InitInterpolate(sint32 frac, sint32 rate);
void phd_PutPolygons_I(sint16 *ptr, sint32 clip);
void phd_RotYXZ_I(sint16 y, sint16 x, sint16 z);
sint16 *GetBoundsAccurate(ITEM_INFO *item);
void DrawRooms(sint16 CurrentRoom);
void SetRoomBounds(sint16 *door, sint32 rn, ROOM_INFO *actualRoom);
sint32 DrawPhaseGame(void);
void GetRoomBounds(void);
void S_InsertRoom(sint16 room_number);
void CalculateObjectLighting(ITEM_INFO *item, sint16 *frame);
void CalculateObjectLightingLara(void);
void UpdateSkyLightning(void);
void DrawEffect(sint16 fx_num);
void InterpolateArmMatrix(sint32 *mx);

extern STATIC_INFO static_objects[60];
extern sint16 LightningCount;
extern sint16 dLightningRand;
extern sint16 LightningRand;
extern uint16 LightningRGB[3];
extern uint16 LightningRGBs[3];
extern sint32 *IMptr;
extern sint16 interpolated_bounds[6];
extern sint32 IM_rate;
extern sint32 IM_frac;
extern sint32 IMstack[768];
extern sint16 no_rotation[12];
extern sint32 draw_room_list[128];
extern sint32 room_list_start;
extern sint32 room_list_end;
extern sint32 number_draw_rooms;
extern sint16 draw_rooms[200];
extern sint32 outside;
extern sint32 outside_left;
extern sint32 outside_right;
extern sint32 outside_top;
extern sint32 outside_bottom;
extern sint32 current_room;
extern PHD_VECTOR vbufdoor[4];

#endif
