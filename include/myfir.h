/*
 * myfir.h
 *
 */

#ifndef MYFIR_H_
#define MYFIR_H_

#include "stdint.h"

#define LPF_COEFF_LEN 61

void myfir(const int16_t* input, const int16_t* filterCoeffs, int16_t* output,
           int16_t* delayLine, uint16_t numberOfInputSamples,
           uint16_t numberOfFilterCoeffs);

#endif /* MYFIR_H_ */
