/*
 * cvsd.h
 *
 *  Created on: May 2, 2021
 *      Author: docquantum
 */

#ifndef CVSD_H_
#define CVSD_H_

uint16_t encode_sample(int16_t sample, int16_t channel);
int16_t decode_sample(uint16_t bit, int16_t channel);
uint16_t var_delta(uint16_t delay, uint16_t delta);

#endif /* CVSD_H_ */
