/*
 * cvsd.c
 *
 *  Created on: Apr 30, 2021
 *      Author: docquantum
 */

#include "custom_types.h"
#include "cvsd.h"

uint16_t encode_sample(int16_t sample, int16_t channel)
{
    static uint16_t delayL = 0, delayR = 0;
    static int16_t referenceL = 0, referenceR = 0;
    static uint16_t deltaL = 1024, deltaR = 1024;

    uint16_t bit = sample >= (channel ? referenceR : referenceL);

    if (channel == 1)
        delayR = (delayR << 1) | bit;
    else
        delayL = (delayL << 1) | bit;

    if (channel == 1)
        deltaR = var_delta(delayR, deltaR);
    else
        deltaL = var_delta(delayL, deltaL);

    if (bit == 1)
        if (channel == 1)
            referenceR = _sadd(referenceR, deltaR);
        else
            referenceL = _sadd(referenceL, deltaL);
    else
        if (channel == 1)
            referenceR = _ssub(referenceR, deltaR);
        else
            referenceL = _ssub(referenceL, deltaL);
    return bit;
}

int16_t decode_sample(uint16_t bit, int16_t channel)
{
    static uint16_t delayL = 0, delayR = 0;
    static int16_t referenceL = 0, referenceR = 0;
    static uint16_t deltaL = 1024, deltaR = 1024;

    if (channel == 1)
        delayR = (delayR << 1) | bit;
    else
        delayL = (delayL << 1) | bit;

    if (channel == 1)
        deltaR = var_delta(delayR, deltaR);
    else
        deltaL = var_delta(delayL, deltaL);

    if (bit == 1)
        if (channel == 1)
            referenceR = _sadd(referenceR, deltaR);
        else
            referenceL = _sadd(referenceL, deltaL);
    else
        if (channel == 1)
            referenceR = _ssub(referenceR, deltaR);
        else
            referenceL = _ssub(referenceL, deltaL);
    return channel ? referenceR : referenceL;
}

uint16_t var_delta(uint16_t delay, uint16_t delta)
{
    delay = (delay & 0x7);
    if (delay == 0x0 || delay == 0x7)
    {
        return _ssub(delta << 1, delta >> 2);
    }
    else if (delta > 0x3ff)
    {
        return _sadd(delta >> 1, delta >> 3);
    }
    return delta;
}
