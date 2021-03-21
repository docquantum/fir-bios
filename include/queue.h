/*
 * queue.h
 *
 *  Created on: Mar 20, 2021
 *      Author: docquantum
 */

#ifndef INCLUDE_QUEUE_H_BAK_
#define INCLUDE_QUEUE_H_BAK_

#include "custom_types.h"
#include "stdint.h"

#define Q_MAX_SIZE (MBX_AUDIO_LEN * 2)

sample_t q_pop();

bool q_push(sample_t sample);

void q_empty();

bool q_is_empty();

bool q_is_full();

#endif /* INCLUDE_QUEUE_H_BAK_ */
