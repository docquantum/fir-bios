
#include <std.h>

#include <log.h>

#include "bioscfg.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "csl_mcbsp.h"
#include "string.h"
#include "myfir.h"

extern MCBSP_Handle aicMcbsp;

extern int16_t lpfCoeff[];

int16_t* filter_coeff;

int16_t rxRightSample;
int16_t rxLeftSample;
int16_t leftRightFlag = 0;
int16_t txleftRightFlag = 0;
int16_t outputL, outputR, inputL, inputR;
int16_t delayLineL[LPF_COEFF_LEN], delayLineR[LPF_COEFF_LEN];

void audioProcessingInit(void)
{
    memset(delayLineL, 0, LPF_COEFF_LEN);
    memset(delayLineR, 0, LPF_COEFF_LEN);
	rxRightSample = 0;
	rxLeftSample = 0;
}

void SetFilter(Uint8 type)
{
    switch(type)
    {
        case 0:
            filter_coeff = lpfCoeff;
            break;
        default:
            filter_coeff = lpfCoeff;
            break;
    }
}

void HWI_I2S_Rx(void)
{
	if (leftRightFlag == 0)
	{
		rxLeftSample = MCBSP_read16(aicMcbsp);
		myfir((const int16_t *) &rxLeftSample, lpfCoeff, &rxLeftSample, delayLineL, 1, LPF_COEFF_LEN);
		leftRightFlag = 1;
	}
	else
	{
		rxRightSample = MCBSP_read16(aicMcbsp);
		myfir((const int16_t *) &rxRightSample, lpfCoeff, &rxRightSample, delayLineR, 1, LPF_COEFF_LEN);
		leftRightFlag = 0;
	}
}

void HWI_I2S_Tx(void)
{
	if (txleftRightFlag == 0)
	{
		MCBSP_write16(aicMcbsp,rxLeftSample);
		txleftRightFlag = 1;
	}
	else
	{
		MCBSP_write16(aicMcbsp,rxRightSample);
		txleftRightFlag = 0;
	}
}
