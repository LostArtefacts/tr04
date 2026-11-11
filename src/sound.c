#include "sound.h"
#include "camera.h"
#include "gameflow.h"
#include "room.h"
#include "specific/3dmath.h"
#include "specific/dxsound.h"
#include "specific/function_stubs.h"

sint32 SoundEffect(sint32 sfx, PHD_3DPOS *pos, sint32 flags)
{
    SAMPLE_INFO *info;
    PHD_3DPOS pos2;
    sint32 lut, radius, dx, dy, dz, volume, OrigVolume, pitch, rnd, sample, flag, vol, slot;
    uint32 distance;
    uint16 pan;
    int i;

    if (sfx == SFX_LARA_NO) {
        switch (Gameflow->Language) {
        case JAPAN:
            sfx = SFX_LARA_NO_JAPAN;
            break;
        case GERMAN:
        case ITALIAN:
        case SPANISH:
            sfx = SFX_LARA_NO;
            break;
        case FRENCH:
            sfx = SFX_LARA_NO_FRENCH;
            break;
        }
    }
    if (!sound_active || !(flags & SFX_ALWAYS) && (flags & SFX_WATER) != (room[camera.pos.room_number].flags & ROOM_UNDERWATER))
        return 0;
    lut = sample_lut[sfx];
    if (lut == -1) {
        nullsub_1("Non present sample:%d", sfx);
        sample_lut[sfx] = -2;
        return 0;
    }
    if (lut == -2)
        return 0;
    info = &sample_infos[lut];
    if (info->randomness) {
        if ((GetRandomDraw() & 0xFF) > info->randomness)
            return 0;
    }
    radius = (info->radius + 1) << 10;
    pan = 0;
    if (pos) {
        dx = pos->x_pos - camera.pos.x;
        dy = pos->y_pos - camera.pos.y;
        dz = pos->z_pos - camera.pos.z;
        if (dx < -radius || dx > radius || dy < -radius || dy > radius || dz < -radius || dz > radius)
            return 0;
        distance = dx * dx + dy * dy + dz * dz;
        if (distance > (uint32)(radius * radius))
            return 0;
        if (distance < 0x100000)
            distance = 0;
        else
            distance = phd_sqrt(distance) - 1024;
        if (!(info->flags & 0x1000))
            pan = (CamRot.y << 4) + phd_atan(dz, dx);
    } else {
        pos2.x_pos = 0;
        pos2.y_pos = 0;
        pos2.z_pos = 0;
        distance = 0;
        pos = &pos2;
    }
    volume = info->volume << 6;
    if (info->flags & 0x4000)
        volume -= GetRandomDraw() << 12 >> 15;
    OrigVolume = volume;
    if (distance)
        volume = (volume * (4096 - (phd_sin((distance << 14) / radius) >> 2))) >> 12;
    if (volume <= 0)
        return 0;
    if (volume > 0x7FFF)
        volume = 0x7FFF;
    if (flags & SFX_SETPITCH)
        pitch = (flags >> 8) & 0xFFFFFF;
    else
        pitch = 0x10000;
    pitch += info->pitch << 9;
    if (info->flags & 0x2000)
        pitch += ((6000 * GetRandomDraw()) >> 14) - 6000;
    if (info->number < 0)
        return 0;
    rnd = (info->flags >> 2) & 0xF;
    if (rnd == 1)
        sample = info->number;
    else
        sample = info->number + ((rnd * GetRandomDraw()) >> 15);
    flag = info->flags & 3;
    switch (flag) {
    case 1:
        for (i = 0; i < 32; i++) {
            if (LaSlot[i].nSampleInfo == lut) {
                if (S_SoundSampleIsPlaying(i))
                    return 0;
                LaSlot[i].nSampleInfo = -1;
            }
        }
        break;
    case 2:
        for (i = 0; i < 32; i++) {
            if (LaSlot[i].nSampleInfo == lut) {
                S_SoundStopSample(i);
                LaSlot[i].nSampleInfo = -1;
                break;
            }
        }
        break;
    case 3:
        for (i = 0; i < 32; i++) {
            if (LaSlot[i].nSampleInfo == lut) {
                if (volume <= LaSlot[i].nVolume)
                    return 0;
                LaSlot[i].OrigVolume = OrigVolume;
                LaSlot[i].nVolume = volume;
                LaSlot[i].nPan = pan;
                LaSlot[i].nPitch = pitch;
                LaSlot[i].distance = distance;
                LaSlot[i].pos.x = pos->x_pos;
                LaSlot[i].pos.y = pos->y_pos;
                LaSlot[i].pos.z = pos->z_pos;
                return 1;
            }
        }
        break;
    }
    if (flag == 3)
        dx = S_SoundPlaySampleLooped(sample, (uint16)volume, pitch, pan, distance);
    else
        dx = S_SoundPlaySample(sample, (uint16)volume, pitch, pan, distance);
    if (dx >= 0) {
        LaSlot[dx].OrigVolume = OrigVolume;
        LaSlot[dx].nVolume = volume;
        LaSlot[dx].nPan = pan;
        LaSlot[dx].nPitch = pitch;
        LaSlot[dx].nSampleInfo = lut;
        LaSlot[dx].distance = distance;
        LaSlot[dx].pos.x = pos->x_pos;
        LaSlot[dx].pos.y = pos->y_pos;
        LaSlot[dx].pos.z = pos->z_pos;
        return 1;
    }
    if (dx != -1) {
        if (sample >= 0)
            nullsub_1("Can't play SFX %d", sfx);
        info->number = -1;
        return 0;
    }
    vol = 0x8000000;
    slot = -1;
    for (i = 1; i < 32; i++) {
        if (LaSlot[i].nSampleInfo >= 0 && LaSlot[i].nVolume <= vol) {
            vol = LaSlot[i].nVolume;
            slot = i;
        }
    }
    if (volume >= vol) {
        S_SoundStopSample(slot);
        LaSlot[slot].nSampleInfo = -1;
        if (flag == 3)
            dx = S_SoundPlaySampleLooped(sample, (uint16)volume, pitch, pan, distance);
        else
            dx = S_SoundPlaySample(sample, (uint16)volume, pitch, pan, distance);
        if (dx >= 0) {
            LaSlot[dx].OrigVolume = OrigVolume;
            LaSlot[dx].nVolume = volume;
            LaSlot[dx].nPan = pan;
            LaSlot[dx].nPitch = pitch;
            LaSlot[dx].nSampleInfo = lut;
            LaSlot[dx].distance = distance;
            LaSlot[dx].pos.x = pos->x_pos;
            LaSlot[dx].pos.y = pos->y_pos;
            LaSlot[dx].pos.z = pos->z_pos;
            return 1;
        }
    }
    return 0;
}

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

void SOUND_Stop(void)
{
    int i;

    if (sound_active) {
        S_SoundStopAllSamples();
        for (i = 0; i < 32; i++)
            LaSlot[i].nSampleInfo = -1;
    }
}

void SOUND_Init(void)
{
    int i;

    nullsub_1(32);
    for (i = 0; i < 32; i++)
        LaSlot[i].nSampleInfo = -1;
    sound_active = 1;
}

void SayNo(void)
{
    sint32 fx;

    fx = SFX_LARA_NO;
    if (Gameflow->Language == FRENCH)
        fx = SFX_LARA_NO_FRENCH;
    else if (Gameflow->Language == JAPAN)
        fx = SFX_LARA_NO_JAPAN;
    SoundEffect(fx, 0, SFX_ALWAYS);
}
