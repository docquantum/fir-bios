/*
 * myfir.c
 *
 */

#include "myfir.h"

void myfir(const int16_t* input, const int16_t* filterCoeffs, int16_t* output,
           int16_t* delayLine, uint16_t numberOfInputSamples,
           uint16_t numberOfFilterCoeffs)
{
    int i, j, k;
    int32_t sum;

    if(numberOfFilterCoeffs <= 0)
    {
        for (i=0; i < numberOfInputSamples; i++)
            output[i] = input[i];
        return;
    }

    for (i = 0; i < numberOfInputSamples; i++)
    {
        // update delay line with current sample
        for (k = numberOfFilterCoeffs-1; k > 0; k--)
        {
            delayLine[k] = delayLine[k-1];
        }
        delayLine[0] = input[i];
        // Perform filter
        sum = 0;
        for (j = 0; j < numberOfFilterCoeffs; j++)
        {
            // main filter loop y[n] += x[n] * h0 + x[n-1]*h1 + ...
            sum = _smac(sum, delayLine[j], filterCoeffs[j]);
        }
        // update output
        output[i] = _rnd(sum) >> 16;
    }
    return;
}
