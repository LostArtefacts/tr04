#include "sound.h"
#include "camera.h"
#include "specific/3dmath.h"
#include "specific/dxsound.h"
#include "specific/function_stubs.h"

void GetPanVolume(SoundSlot *slot)
{
    SAMPLE_INFO *info;
    sint32 nVolume, dz, nPan, dy, radius, dx;
    uint32 distance;

    info = &sample_infos[slot->nSampleInfo];
    if (slot->distance || slot->pos.x || slot->pos.y || slot->pos.z) {
        radius = info->radius << 10;
        dx = slot->pos.x - camera.pos.x;
        dy = slot->pos.y - camera.pos.y;
        dz = slot->pos.z - camera.pos.z;
        if (dx < -radius || dx > radius || dy < -radius || dy > radius || dz < -radius || dz > radius) {
            slot->distance = 0;
            slot->nPan = 0;
            slot->nVolume = 0;
        } else {
            distance = dx * dx + dy * dy + dz * dz;
            if (distance > (uint32)(radius * radius)) {
                slot->distance = 0;
                slot->nPan = 0;
                slot->nVolume = 0;
            } else {
                if (distance < 0x100000)
                    distance = 0;
                else
                    distance = phd_sqrt(distance) - 1024;
                nPan = (CamRot.y << 4) + phd_atan(dz, dx);
                nVolume = slot->OrigVolume;
                if (distance)
                    nVolume = (nVolume * (4096 - (phd_sin((distance << 14) / radius) >> 2))) >> 12;
                if (nVolume <= 0) {
                    slot->distance = 0;
                    slot->nPan = 0;
                    slot->nVolume = 0;
                } else {
                    if (nVolume > 0x7FFF)
                        nVolume = 0x7FFF;
                    slot->nVolume = nVolume;
                    slot->nPan = (uint16)nPan;
                    slot->distance = distance;
                }
            }
        }
    }
}

void StopSoundEffect(sint32 sfx)
{
    sint32 lut, end;
    int i;

    if (sound_active) {
        lut = sample_lut[sfx];
        end = lut + ((sample_infos[lut].flags >> 2) & 0xF);
        for (i = 0; i < 32; i++) {
            if (LaSlot[i].nSampleInfo >= lut && LaSlot[i].nSampleInfo < end) {
                S_SoundStopSample(i);
                LaSlot[i].nSampleInfo = -1;
            }
        }
    }
}
