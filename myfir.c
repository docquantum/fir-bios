/*
 * myfir.c
 *
 */

#include "myfir.h"

const int16_t lpfCoeff[] =
{
       -12,      0,      7,     18,     34,     52,     70,     84,     88,     77,     46,     -7,    -80,   -170,   -268,   -360,
      -428,   -456,   -423,   -314,   -120,    165,    533,    969,   1450,   1945,   2419,   2834,   3159,   3365,   3436,   3365,
      3159,   2834,   2419,   1945,   1450,    969,    533,    165,   -120,   -314,   -423,   -456,   -428,   -360,   -268,   -170,
       -80,     -7,     46,     77,     88,     84,     70,     52,     34,     18,      7,      0,    -12,
};

void myfir(const int16_t* input, const int16_t* filterCoeffs, int16_t* output,
           int16_t* delayLine, uint16_t numberOfInputSamples,
           uint16_t numberOfFilterCoeffs)
{
    int i, j, k;
    int32_t sum;

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
}
