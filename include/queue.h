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

#define Q_MAX_SIZE 96

int16_t q_pop();

int16_t q_push(int16_t sample);

void q_empty();

int16_t q_is_empty();

int16_t q_is_full();

#endif /* INCLUDE_QUEUE_H_BAK_ */
