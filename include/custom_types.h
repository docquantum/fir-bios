/*
 * custom_types.h
 *
 */

#ifndef CUSTOM_TYPES_H_
#define CUSTOM_TYPES_H_

#include "stdint.h"

typedef enum command_t
{
    TOGGLE_MUTE,
    FILTER_NONE,
    FILTER_LPF,
    FILTER_HPF
} command_t;

#endif /* CUSTOM_TYPES_H_ */
