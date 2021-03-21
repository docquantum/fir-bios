/*
 * custom_types.h
 *
 */

#ifndef CUSTOM_TYPES_H_
#define CUSTOM_TYPES_H_

#include "stdint.h"

typedef enum channel_t
{
    LEFT,
    RIGHT
} channel_t;

typedef enum command_t
{
    TOGGLE_MUTE,
    FILTER_NONE,
    FILTER_LPF,
    FILTER_HPF
} command_t;

typedef struct sample_t
{
    channel_t channel;
    int16_t data;
} sample_t;

typedef uint16_t bool;

#define MBX_AUDIO_LEN 96
#define MBX_MESSAGES_LEN 1

#endif /* CUSTOM_TYPES_H_ */
