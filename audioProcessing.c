
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
extern int16_t hpfCoeff[];

int16_t* filter_coeff;
int16_t filter_length = 0;

int16_t rxRightSample = 0;
int16_t rxLeftSample = 0;
int16_t leftRightFlag = 0;
int16_t txleftRightFlag = 0;
uint16_t muteFlag = 0;
int16_t delayLineL[LPF_COEFF_LEN], delayLineR[LPF_COEFF_LEN];

void ToggleMute()
{
    muteFlag = muteFlag ? 0 : 1;
}

uint16_t GetMute()
{
    return muteFlag;
}

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
            filter_coeff = NULL;
            filter_length = 0;
            break;
        case 1:
            filter_coeff = hpfCoeff;
            filter_length = HPF_COEFF_LEN;
            break;
        case 2:
            filter_coeff = lpfCoeff;
            filter_length = LPF_COEFF_LEN;
            break;
        default:
            filter_coeff = NULL;
            filter_length = 0;
            break;
    }
}

void HWI_I2S_Rx(void)
{
	if (leftRightFlag == 0)
	{
		rxLeftSample = MCBSP_read16(aicMcbsp);
		myfir((const int16_t *) &rxLeftSample, filter_coeff, &rxLeftSample, delayLineL, 1, filter_length);
		leftRightFlag = 1;
	}
	else
	{
		rxRightSample = MCBSP_read16(aicMcbsp);
		myfir((const int16_t *) &rxRightSample, filter_coeff, &rxRightSample, delayLineR, 1, filter_length);
		leftRightFlag = 0;
	}
}

void HWI_I2S_Tx(void)
{
    if (muteFlag)
    {
        rxLeftSample = 0;
        rxRightSample = 0;
    }
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
